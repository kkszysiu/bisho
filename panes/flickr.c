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
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include <gnome-keyring.h>
#include <libsoup/soup.h>
#include <rest-extras/flickr-proxy.h>
#include <rest/rest-xml-parser.h>
#include <libsocialweb-keystore/sw-keystore.h>
#include "service-info.h"
#include "bisho-module.h"
#include "bisho-utils.h"
/* TODO: merge */
#include "flickr.h"

/* TODO: use sw-keyring */
static const GnomeKeyringPasswordSchema flickr_schema = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
    { "server", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { "api-key", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { NULL, 0 }
  }
};

#define FLICKR_SERVER "http://flickr.com/"

struct _BishoPaneFlickrPrivate {
  const char *api_key;
  const char *shared_secret;
  RestProxy *proxy;
  GtkWidget *button;
  gchar *frob;
  char *user_name;
};

typedef enum {
  LOGGED_OUT,
  WORKING,
  LOGGED_IN,
} ButtonState;

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_PANE_FLICKR, BishoPaneFlickrPrivate))
G_DEFINE_DYNAMIC_TYPE (BishoPaneFlickr, bisho_pane_flickr, BISHO_TYPE_PANE);

static void update_widgets (BishoPaneFlickr *data, ButtonState state);

static RestXmlNode *
get_xml (RestProxyCall *call)
{
  static RestXmlParser *parser = NULL;
  RestXmlNode *root;

  if (parser == NULL)
    parser = rest_xml_parser_new ();

  root = rest_xml_parser_parse_from_data (parser,
                                          rest_proxy_call_get_payload (call),
                                          rest_proxy_call_get_payload_length (call));

  if (root == NULL) {
    g_message ("Invalid XML from Flickr:\n%s",
               rest_proxy_call_get_payload (call));
    goto done;
  }

  if (strcmp (root->name, "rsp") != 0) {
    g_message ("Unexpected response from Flickr:\n%s",
               rest_proxy_call_get_payload (call));
    rest_xml_node_unref (root);
    root = NULL;
    goto done;
  }

  if (strcmp (rest_xml_node_get_attr (root, "stat"), "ok") != 0) {
    RestXmlNode *node;
    const char *msg;

    node = rest_xml_node_find (root, "err");
    msg = rest_xml_node_get_attr (node, "msg");

    /* TODO bisho_utils_message (NULL, "Flickr", msg); */
    g_message ("Error from Flickr: %s", msg);

    rest_xml_node_unref (root);
    root = NULL;
    goto done;
  }

  done:
  g_object_unref (call);
  return root;
}

static void
log_in_clicked (GtkWidget *button, gpointer user_data)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (user_data);
  BishoPaneFlickrPrivate *priv = pane->priv;
  char *frob, *url;
  RestProxyCall *call;
  RestXmlNode *root;

  update_widgets (pane, WORKING);

  call = rest_proxy_new_call (priv->proxy);
  rest_proxy_call_set_function (call, "flickr.auth.getFrob");

  if (!rest_proxy_call_sync (call, NULL))
    g_error ("Cannot get frob");

  root = get_xml (call);
  priv->frob = g_strdup (rest_xml_node_find (root, "frob")->content);
  rest_xml_node_unref (root);

  url = flickr_proxy_build_login_url (FLICKR_PROXY (priv->proxy), priv->frob);
  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (button)), url, GDK_CURRENT_TIME, NULL);
}


static void
delete_done_cb (GnomeKeyringResult result, gpointer user_data)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (user_data);
  SwClientService *service;

  if (result == GNOME_KEYRING_RESULT_OK){
    update_widgets (pane, LOGGED_OUT);
    service = sw_client_get_service (BISHO_PANE (pane)->socialweb, BISHO_PANE (pane)->info->name);
    sw_client_service_credentials_updated (service);
  }
  else
    update_widgets (pane, LOGGED_IN);
}

static void
log_out_clicked (GtkButton *button, gpointer user_data)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (user_data);
  BishoPaneFlickrPrivate *priv = pane->priv;

  gnome_keyring_delete_password (&flickr_schema, delete_done_cb, user_data, NULL,
                                 "server", FLICKR_SERVER,
                                 "api-key", priv->api_key,
                                 NULL);

  update_widgets (pane, LOGGED_OUT);
}

static void
got_auth (RestXmlNode *node, BishoPaneFlickr *pane)
{
  RestXmlNode *user;
  const char *name;

  user = rest_xml_node_find (node, "user");
  name = rest_xml_node_get_attr (user, "fullname");
  if (name == NULL || name[0] == '\0')
    name = rest_xml_node_get_attr (user, "username");

  g_free (pane->priv->user_name);
  pane->priv->user_name = g_strdup (name);

  update_widgets (pane, LOGGED_IN);
}

static void
bisho_pane_flickr_continue_auth (BishoPane *_pane, GHashTable *params)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (_pane);
  BishoPaneFlickrPrivate *priv = pane->priv;
  ServiceInfo *info = BISHO_PANE (pane)->info;
  SwClientService *service;
  RestProxyCall *call;
  RestXmlNode *node;
  const char *token;
  GError *error = NULL;
  const gchar *frob;

  if (params == NULL || g_hash_table_lookup (params, "frob") == NULL) {
    if (!priv->frob)
    {
      g_message ("Frob not provided in callback, cannot continue");
      /* TODO bisho_utils_message (NULL, "Flickr", NULL); */
      update_widgets (pane, LOGGED_OUT);
      return;
    } else {
      frob = priv->frob;
    }
  } else {
    frob = g_hash_table_lookup (params, "frob");
  }

  update_widgets (pane, WORKING);

  call = rest_proxy_new_call (priv->proxy);
  rest_proxy_call_set_function (call, "flickr.auth.getToken");
  rest_proxy_call_add_param (call, "frob", frob);

  if (priv->frob)
  {
    g_free (priv->frob);
    frob = NULL;
  }

  if (!rest_proxy_call_sync (call, &error)) {
    bisho_pane_set_banner_error (BISHO_PANE (pane), error);
    g_message ("Cannot get token: %s", error->message);
    g_error_free (error);
    update_widgets (pane, LOGGED_OUT);
    return;
  }

  node = get_xml (call);

  if (node == NULL) {
    update_widgets (pane, LOGGED_OUT);
    return;
  }

  token = rest_xml_node_find (node, "token")->content;
  flickr_proxy_set_token (FLICKR_PROXY (priv->proxy), token);

  got_auth (node, pane);

  /* TODO async */
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *attrs;
  guint32 id;
  attrs = gnome_keyring_attribute_list_new ();
  gnome_keyring_attribute_list_append_string (attrs, "server", FLICKR_SERVER);
  gnome_keyring_attribute_list_append_string (attrs, "api-key", priv->api_key);

  result = gnome_keyring_item_create_sync (NULL,
                                           GNOME_KEYRING_ITEM_GENERIC_SECRET,
                                           info->display_name,
                                           attrs, token,
                                           TRUE, &id);

  if (result == GNOME_KEYRING_RESULT_OK) {
    gnome_keyring_item_grant_access_rights_sync (NULL,
                                                 "libsocialweb",
                                                 LIBEXECDIR "/libsocialweb-core",
                                                 id, GNOME_KEYRING_ACCESS_READ);
  } else {
    g_message ("Cannot update keyring: %s", gnome_keyring_result_to_message (result));
    update_widgets (pane, LOGGED_OUT);
  }

  rest_xml_node_unref (node);

  service = sw_client_get_service (BISHO_PANE (pane)->socialweb, info->name);
  sw_client_service_credentials_updated (service);
}

static void
continue_clicked (GtkWidget *button, gpointer user_data)
{
  bisho_pane_flickr_continue_auth (BISHO_PANE (user_data), NULL);
}

static void
update_widgets (BishoPaneFlickr *pane, ButtonState state)
{
  BishoPaneFlickrPrivate *priv = pane->priv;

  g_signal_handlers_disconnect_by_func (priv->button, log_out_clicked, pane);
  g_signal_handlers_disconnect_by_func (priv->button, log_in_clicked, pane);
  g_signal_handlers_disconnect_by_func (priv->button, continue_clicked, pane);

  switch (state) {
  case LOGGED_OUT:
    bisho_pane_set_user (BISHO_PANE (pane), NULL, NULL);
    bisho_pane_set_banner (BISHO_PANE (pane), NULL);
    gtk_widget_show (priv->button);
    gtk_button_set_label (GTK_BUTTON (priv->button), _("Log me in"));
    g_signal_connect (priv->button, "clicked", G_CALLBACK (log_in_clicked), pane);
    break;
  case WORKING:
    bisho_pane_set_banner (BISHO_PANE (pane), _("Connecting..."));
    gtk_button_set_label (GTK_BUTTON (priv->button), _("Continue"));
    g_signal_connect (priv->button, "clicked", G_CALLBACK (continue_clicked), pane);
    break;
  case LOGGED_IN:
    bisho_pane_set_banner (BISHO_PANE (pane), _("Log in succeeded. You'll see new items in a couple of minutes."));
    bisho_pane_set_user (BISHO_PANE (pane), NULL, priv->user_name);
    gtk_widget_show (priv->button);
    gtk_button_set_label (GTK_BUTTON (priv->button), _("Log me out"));
    g_signal_connect (priv->button, "clicked", G_CALLBACK (log_out_clicked), pane);
    break;
  }
}

static void
check_token_cb (RestProxyCall *call, const GError *error, GObject *weak_object, gpointer user_data)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (user_data);
  RestXmlNode *node;

  if (error) {
    bisho_pane_set_banner_error (BISHO_PANE (pane), error);
    g_message ("Cannot check token: %s", error->message);
  } else {
    node = get_xml (call);
    if (node) {
      got_auth (node, pane);
      rest_xml_node_unref (node);
    } else {
      /* The token isn't valid so fake a log out */
      log_out_clicked (NULL, pane);
    }
  }
}

static void
find_key_cb (GnomeKeyringResult result,
             const char *string,
             gpointer user_data)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (user_data);
  BishoPaneFlickrPrivate *priv = pane->priv;

  if (result == GNOME_KEYRING_RESULT_OK) {
    GError *error = NULL;
    RestProxyCall *call;

    flickr_proxy_set_token (FLICKR_PROXY (priv->proxy), string);

    call = rest_proxy_new_call (priv->proxy);
    rest_proxy_call_set_function (call, "flickr.auth.checkToken");

    if (rest_proxy_call_async (call, check_token_cb, NULL, pane, &error)) {
      update_widgets (pane, WORKING);
    } else {
      bisho_pane_set_banner_error (BISHO_PANE (pane), error);
      g_message ("Cannot check token: %s", error->message);
      g_error_free (error);
    }
  } else {
    update_widgets (pane, LOGGED_OUT);
  }
}

static const char *
bisho_pane_flickr_get_auth_type (BishoPaneClass *klass)
{
  return "flickr";
}

static void
bisho_pane_flickr_constructed (GObject *object)
{
  BishoPaneFlickr *pane = BISHO_PANE_FLICKR (object);
  BishoPaneFlickrPrivate *priv = pane->priv;

  bisho_pane_follow_connected (BISHO_PANE (pane), priv->button);

  /* TODO: use GInitable */
  if (!sw_keystore_get_key_secret ("flickr",
                                   &priv->api_key,
                                   &priv->shared_secret)) {
    return;
  }

  priv->proxy = flickr_proxy_new (priv->api_key, priv->shared_secret);
  rest_proxy_set_user_agent (priv->proxy, "Bisho/" VERSION);

  update_widgets (pane, WORKING);

  gnome_keyring_find_password (&flickr_schema, find_key_cb, pane, NULL,
                               "server", FLICKR_SERVER,
                               "api-key", priv->api_key,
                               NULL);
}

static void
bisho_pane_flickr_class_init (BishoPaneFlickrClass *klass)
{
  GObjectClass *o_class = G_OBJECT_CLASS (klass);
  BishoPaneClass *pane_class = BISHO_PANE_CLASS (klass);

  o_class->constructed = bisho_pane_flickr_constructed;
  pane_class->get_auth_type = bisho_pane_flickr_get_auth_type;
  pane_class->continue_auth = bisho_pane_flickr_continue_auth;

  g_type_class_add_private (klass, sizeof (BishoPaneFlickrPrivate));
}

static void
bisho_pane_flickr_class_finalize (BishoPaneFlickrClass *klass)
{
}

static void
bisho_pane_flickr_init (BishoPaneFlickr *pane)
{
  BishoPaneFlickrPrivate *priv;
  GtkWidget *content, *align, *box;

  pane->priv = GET_PRIVATE (pane);
  priv = pane->priv;

  content = BISHO_PANE (pane)->content;

  align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_widget_show (align);
  gtk_container_add (GTK_CONTAINER (content), align);

  box = gtk_hbox_new (FALSE, 8);
  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (align), box);

  priv->button = gtk_button_new ();
  gtk_widget_show (priv->button);
  gtk_box_pack_start (GTK_BOX (box), priv->button, FALSE, FALSE, 0);
}

void
bisho_module_load (BishoModule *module)
{
  bisho_pane_flickr_register_type ((GTypeModule *)module);
}

