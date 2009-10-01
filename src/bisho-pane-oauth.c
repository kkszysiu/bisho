/*
 * Copyright (C) 2009 Intel Corporation.
 *
 * Author: Ross Burton <ross@linux.intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gnome-keyring.h>
#include <libsoup/soup.h>
#include <rest/oauth-proxy.h>
#include "service-info.h"
#include "bisho-utils.h"
#include "bisho-pane-oauth.h"

/* TODO: use mojito-keyring */
static const GnomeKeyringPasswordSchema oauth_schema = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
    { "server", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { "consumer-key", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { NULL, 0 }
  }
};

typedef enum {
  LOGGED_OUT,
  WORKING,
  CONTINUE_AUTH_10,
  CONTINUE_AUTH_10a,
  LOGGED_IN,
} ButtonState;

struct _BishoPaneOauthPrivate {
  ServiceInfo *info; /* TODO remove */
  RestProxy *proxy;
  GtkWidget *pin_label;
  GtkWidget *pin_entry;
  GtkWidget *button;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_PANE_OAUTH, BishoPaneOauthPrivate))
G_DEFINE_TYPE (BishoPaneOauth, bisho_pane_oauth, BISHO_TYPE_PANE);

static void update_widgets (BishoPaneOauth *pane, ButtonState state);

static char *
create_url (ServiceInfo *info, const char *token)
{
  SoupURI *base, *uri;
  char *s;

  g_assert (info);
  g_assert (token);

  base = soup_uri_new (info->oauth.base_url);
  uri = soup_uri_new_with_base (base, info->oauth.authorize_function);
  soup_uri_free (base);

  soup_uri_set_query_from_fields (uri,
                                  "oauth_token", token,
                                  "oauth_callback", info->oauth.callback ?: "",
                                  NULL);

  s = soup_uri_to_string (uri, FALSE);
  soup_uri_free (uri);
  return s;
}

G_GNUC_UNUSED static const char * unused_for_now[] = {
  N_("You don't seem to have a network connection, this won't work."),
  N_("Sorry, we can't log in to %s. %s"),
  N_("You could check that the computer's clock is correct."),
  N_("You could try again.")
};

static void
log_in_clicked (GtkWidget *button, gpointer user_data)
{
  BishoPaneOauth *pane = BISHO_PANE_OAUTH (user_data);
  BishoPaneOauthPrivate *priv = pane->priv;
  ServiceInfo *info = priv->info;
  GError *error = NULL;
  char *url;

  update_widgets (pane, WORKING);

  /* TODO: async */
  oauth_proxy_request_token (OAUTH_PROXY (priv->proxy),
                             info->oauth.request_token_function,
                             info->oauth.callback,
                             &error);
  if (error) {
    char *s;
    g_message ("Error from %s: %s", info->name, error->message);
    s = g_strdup_printf (_("Sorry, we can't log in to %s"), info->display_name);
    bisho_pane_set_banner (BISHO_PANE (pane), s);
    g_free (s);

    update_widgets (pane, LOGGED_OUT);
    return;
  }

  url = create_url (info, oauth_proxy_get_token (OAUTH_PROXY (priv->proxy)));
  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (button)), url, GDK_CURRENT_TIME, NULL);

  if (info->oauth.callback == NULL) {
    update_widgets (pane, CONTINUE_AUTH_10);
  } else {
    /* TODO: insert check for 1.0a? */
    if (strcmp (info->oauth.callback, "oob") == 0) {
      update_widgets (pane, CONTINUE_AUTH_10a);
    } else {
      update_widgets (pane, CONTINUE_AUTH_10);
      /* TODO: should be
         update_widgets (pane, WORKING);
         but myspace breaks this at the moment */
    }
  }
}


static void
delete_done_cb (GnomeKeyringResult result, gpointer user_data)
{
  BishoPaneOauth *pane = BISHO_PANE_OAUTH (user_data);

  if (result == GNOME_KEYRING_RESULT_OK)
    update_widgets (pane, LOGGED_OUT);
  else
    update_widgets (pane, LOGGED_IN);
}

static void
log_out_clicked (GtkButton *button, gpointer user_data)
{
  BishoPaneOauth *pane = BISHO_PANE_OAUTH (user_data);
  BishoPaneOauthPrivate *priv = pane->priv;

  gnome_keyring_delete_password (&oauth_schema, delete_done_cb, user_data, NULL,
                                 "server", priv->info->oauth.base_url,
                                 "consumer-key", priv->info->oauth.consumer_key,
                                 NULL);

  update_widgets (pane, LOGGED_OUT);
}

static void
bisho_pane_oauth_continue_auth (BishoPane *_pane, GHashTable *params)
{
  BishoPaneOauth *pane = BISHO_PANE_OAUTH (_pane);
  BishoPaneOauthPrivate *priv = pane->priv;
  GError *error = NULL;
  char *encoded;
  const char *verifier;

  /* TODO: check the current state */
  /* TODO: handle the arguments */

  update_widgets (pane, WORKING);

  /*
   * If the server is using 1.0a then we need to provide a verifier.  If the
   * callback is "oob" then we need to ask for the verifier, otherwise it's in
   * the parameters we've been passed.
   */
  if (oauth_proxy_is_oauth10a (OAUTH_PROXY (priv->proxy))) {
    /* If 1.0a then a callback must have been specified */
    if (strcmp (priv->info->oauth.callback, "oob") == 0) {
      verifier = gtk_entry_get_text (GTK_ENTRY (priv->pin_entry));
      gtk_widget_hide (priv->pin_label);
      gtk_widget_hide (priv->pin_entry);
    } else {
      verifier = g_hash_table_lookup (params, "oauth_verifier");
    }
  } else {
    verifier = NULL;
  }

  /* TODO: async */
  oauth_proxy_access_token (OAUTH_PROXY (priv->proxy),
                            priv->info->oauth.access_token_function,
                            verifier,
                            &error);
  if (error) {
    /* TODO */
    g_warning ("%s", error->message);
    update_widgets (pane, LOGGED_OUT);
    return;
  }

  encoded = bisho_utils_encode_tokens
    (oauth_proxy_get_token (OAUTH_PROXY (priv->proxy)),
     oauth_proxy_get_token_secret (OAUTH_PROXY (priv->proxy)));

  /* TODO async */
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *attrs;
  guint32 id;
  attrs = gnome_keyring_attribute_list_new ();
  gnome_keyring_attribute_list_append_string (attrs, "server", priv->info->oauth.base_url);
  gnome_keyring_attribute_list_append_string (attrs, "consumer-key", priv->info->oauth.consumer_key);

  result = gnome_keyring_item_create_sync (NULL,
                                           GNOME_KEYRING_ITEM_GENERIC_SECRET,
                                           priv->info->display_name,
                                           attrs, encoded,
                                           TRUE, &id);

  if (result == GNOME_KEYRING_RESULT_OK) {
    gnome_keyring_item_grant_access_rights_sync (NULL,
                                                 "mojito",
                                                 LIBEXECDIR "/mojito-core",
                                                 id, GNOME_KEYRING_ACCESS_READ);
    update_widgets (pane, LOGGED_IN);
  } else {
    update_widgets (pane, LOGGED_OUT);
  }
}

static void
continue_clicked (GtkWidget *button, gpointer user_data)
{
  bisho_pane_oauth_continue_auth (BISHO_PANE (user_data), NULL);
}

static void
update_widgets (BishoPaneOauth *pane, ButtonState state)
{
  BishoPaneOauthPrivate *priv;

  g_assert (BISHO_IS_PANE_OAUTH (pane));
  priv = pane->priv;

  g_signal_handlers_disconnect_by_func (priv->button, log_out_clicked, pane);
  g_signal_handlers_disconnect_by_func (priv->button, continue_clicked, pane);
  g_signal_handlers_disconnect_by_func (priv->button, log_in_clicked, pane);

  switch (state) {
  case LOGGED_OUT:
    bisho_pane_set_banner (BISHO_PANE (pane), NULL);
    gtk_widget_set_sensitive (priv->button, TRUE);
    gtk_button_set_label (GTK_BUTTON (priv->button), _("Log me in"));
    g_signal_connect (priv->button, "clicked", G_CALLBACK (log_in_clicked), pane);
    break;
  case WORKING:
    gtk_widget_set_sensitive (priv->button, FALSE);
    gtk_button_set_label (GTK_BUTTON (priv->button), _("Working..."));
    break;
  case CONTINUE_AUTH_10:
    {
      char *s;

      gtk_widget_set_sensitive (priv->button, TRUE);

      s = g_strdup_printf (_("Once you have logged in to %s, press Continue."),
                           priv->info->display_name);
      bisho_pane_set_banner (BISHO_PANE (pane), s);
      g_free (s);

      gtk_button_set_label (GTK_BUTTON (priv->button), _("Continue"));
      g_signal_connect (priv->button, "clicked", G_CALLBACK (continue_clicked), pane);
    }
    break;
  case CONTINUE_AUTH_10a:
    {
      char *s;

      gtk_widget_show (priv->pin_label);
      gtk_widget_show (priv->pin_entry);
      gtk_widget_set_sensitive (priv->button, TRUE);

      s = g_strdup_printf (_("Once you have logged in to %s, enter the code they give you and press Continue."),
                           priv->info->display_name);
      bisho_pane_set_banner (BISHO_PANE (pane), s);
      g_free (s);

      gtk_button_set_label (GTK_BUTTON (priv->button), _("Continue"));
      g_signal_connect (priv->button, "clicked", G_CALLBACK (continue_clicked), pane);
    }
    break;
  case LOGGED_IN:
    gtk_widget_set_sensitive (priv->button, TRUE);
    bisho_pane_set_banner (BISHO_PANE (pane), _("Log in succeeded. You'll see new items in a couple of minutes."));
    gtk_button_set_label (GTK_BUTTON (priv->button), _("Log me out"));
    g_signal_connect (priv->button, "clicked", G_CALLBACK (log_out_clicked), pane);
    break;
  }
}

static void
find_key_cb (GnomeKeyringResult result,
             const char *string,
             gpointer user_data)
{
  BishoPaneOauth *pane = BISHO_PANE_OAUTH (user_data);

  if (result == GNOME_KEYRING_RESULT_OK)
    update_widgets (pane, LOGGED_IN);
  else
    update_widgets (pane, LOGGED_OUT);
}

static void
bisho_pane_oauth_class_init (BishoPaneOauthClass *klass)
{
  BishoPaneClass *pane_class = BISHO_PANE_CLASS (klass);

  pane_class->continue_auth = bisho_pane_oauth_continue_auth;

  g_type_class_add_private (klass, sizeof (BishoPaneOauthPrivate));
}

static void
bisho_pane_oauth_init (BishoPaneOauth *self)
{
  self->priv = GET_PRIVATE (self);
}

GtkWidget *
bisho_pane_oauth_new (ServiceInfo *info)
{
  BishoPaneOauth *pane;
  BishoPaneOauthPrivate *priv;
  GtkWidget *content, *align, *box;

  g_assert (info);
  g_assert (info->auth == AUTH_OAUTH);

  pane = g_object_new (BISHO_TYPE_PANE_OAUTH,
                       "service", info,
                       NULL);
  priv = pane->priv;

  priv->info = info;
  priv->proxy = oauth_proxy_new (info->oauth.consumer_key,
                                info->oauth.consumer_secret,
                                info->oauth.base_url, FALSE);
  rest_proxy_set_user_agent (priv->proxy, "Bisho/" VERSION);

  content = BISHO_PANE (pane)->content;

  align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_widget_show (align);
  gtk_container_add (GTK_CONTAINER (content), align);

  box = gtk_hbox_new (FALSE, 8);
  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (align), box);

  priv->pin_label = gtk_label_new (_("Code:"));
  gtk_box_pack_start (GTK_BOX (box), priv->pin_label, FALSE, FALSE, 0);

  priv->pin_entry = gtk_entry_new ();
  gtk_box_pack_start (GTK_BOX (box), priv->pin_entry, FALSE, FALSE, 0);

  priv->button = gtk_button_new ();
  gtk_widget_show (priv->button);
  gtk_box_pack_start (GTK_BOX (box), priv->button, FALSE, FALSE, 0);

  update_widgets (pane, LOGGED_OUT);

  gnome_keyring_find_password (&oauth_schema, find_key_cb, pane, NULL,
                               "server", info->oauth.base_url,
                               "consumer-key", info->oauth.consumer_key,
                               NULL);

  return (GtkWidget *)pane;
}
