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
#include <rest/flickr-proxy.h>
#include <rest/rest-xml-parser.h>
#include "service-info.h"
#include "bisho-flickr-pane.h"

/* TODO: use mojito-keyring */
static const GnomeKeyringPasswordSchema flickr_schema = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
    { "server", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { "api-key", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { NULL, 0 }
  }
};

#define FLICKR_SERVER "http://flickr.com/"

/* TODO: make this a widget subclass and this private data */
typedef struct {
  ServiceInfo *info;
  RestProxy *proxy;
  GtkWidget *label;
  GtkWidget *button;
} WidgetData;

typedef enum {
  LOGGED_OUT,
  WORKING,
  CONTINUE_AUTH,
  LOGGED_IN,
} ButtonState;

static void update_widgets (WidgetData *data, ButtonState state, const char *name);

static GtkWidget *
make_disclaimer_label (ServiceInfo *info)
{
  char *s;
  GtkWidget *label;

  s = g_strdup_printf (_("You'll need an account with %s and an Internet connection to use this web service."),
                       info->display_name);
  label = gtk_label_new (s);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  g_free (s);

  return label;
}

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
    g_message ("Invalid XML from Flickr:\n%s\n",
               rest_proxy_call_get_payload (call));
    goto done;
  }

  if (strcmp (root->name, "rsp") != 0) {
    g_message ("Unexpected response from Flickr:\n%s\n",
               rest_proxy_call_get_payload (call));
    rest_xml_node_unref (root);
    root = NULL;
    goto done;
  }

  if (strcmp (rest_xml_node_get_attr (root, "stat"), "ok") != 0) {
    RestXmlNode *node;
    node = rest_xml_node_find (root, "err");
    g_message ("Error from Flickr: %s", rest_xml_node_get_attr (node, "msg"));
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
  WidgetData *data = user_data;
  char *url;
  RestProxyCall *call;
  RestXmlNode *node;

  update_widgets (data, WORKING, NULL);

  /* TODO: async */
  call = rest_proxy_new_call (data->proxy);
  rest_proxy_call_set_function (call, "flickr.auth.getFrob");

  if (!rest_proxy_call_run (call, NULL, NULL))
    g_error ("Cannot get frob");

  node = get_xml (call);

  data->info->flickr.frob = g_strdup (rest_xml_node_find (node, "frob")->content);
  rest_xml_node_unref (node);

  url = flickr_proxy_build_login_url (FLICKR_PROXY (data->proxy), data->info->flickr.frob);
  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (button)), url, GDK_CURRENT_TIME, NULL);

  /* TODO wait for dbus call from callback */
  update_widgets (data, CONTINUE_AUTH, NULL);
}


static void
delete_done_cb (GnomeKeyringResult result, gpointer user_data)
{
  WidgetData *data = user_data;

  if (result == GNOME_KEYRING_RESULT_OK)
    update_widgets (data, LOGGED_OUT, NULL);
  else
    update_widgets (data, LOGGED_IN, NULL);
}

static void
log_out_clicked (GtkButton *button, gpointer user_data)
{
  WidgetData *data = user_data;

  gnome_keyring_delete_password (&flickr_schema, delete_done_cb, user_data, NULL,
                                 "server", FLICKR_SERVER,
                                 "api-key", data->info->flickr.api_key,
                                 NULL);

  update_widgets (data, LOGGED_OUT, NULL);
}

static void
got_auth (RestXmlNode *node, WidgetData *data)
{
  RestXmlNode *user;
  const char *name;

  user = rest_xml_node_find (node, "user");
  name = rest_xml_node_get_attr (user, "fullname");
  if (name == NULL || name[0] == '\0')
    name = rest_xml_node_get_attr (user, "username");

  update_widgets (data, LOGGED_IN, name);
}

static void
continue_clicked (GtkWidget *button, gpointer user_data)
{
  WidgetData *data = user_data;
  RestProxyCall *call;
  RestXmlNode *node;
  const char *token;

  update_widgets (data, WORKING, NULL);

  call = rest_proxy_new_call (data->proxy);
  rest_proxy_call_set_function (call, "flickr.auth.getToken");
  rest_proxy_call_add_param (call, "frob", data->info->flickr.frob);

  if (!rest_proxy_call_run (call, NULL, NULL))
    g_error ("Cannot get token");

  node = get_xml (call);

  if (node == NULL) {
    update_widgets (data, LOGGED_OUT, NULL);
    return;
  }

  token = rest_xml_node_find (node, "token")->content;
  flickr_proxy_set_token (FLICKR_PROXY (data->proxy), token);

  got_auth (node, data);

  /* TODO async */
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *attrs;
  guint32 id;
  attrs = gnome_keyring_attribute_list_new ();
  gnome_keyring_attribute_list_append_string (attrs, "server", FLICKR_SERVER);
  gnome_keyring_attribute_list_append_string (attrs, "api-key", data->info->flickr.api_key);

  result = gnome_keyring_item_create_sync (NULL,
                                           GNOME_KEYRING_ITEM_GENERIC_SECRET,
                                           data->info->display_name,
                                           attrs, token,
                                           TRUE, &id);

  if (result == GNOME_KEYRING_RESULT_OK) {
    gnome_keyring_item_grant_access_rights_sync (NULL,
                                                 "mojito",
                                                 LIBEXECDIR "/mojito-core",
                                                 id, GNOME_KEYRING_ACCESS_READ);
  } else {
    update_widgets (data, LOGGED_OUT, NULL);
  }

  rest_xml_node_unref (node);
}

static void
update_widgets (WidgetData *data, ButtonState state, const char *name)
{
  g_signal_handlers_disconnect_by_func (data->button, log_out_clicked, data);
  g_signal_handlers_disconnect_by_func (data->button, continue_clicked, data);
  g_signal_handlers_disconnect_by_func (data->button, log_in_clicked, data);

  /* TODO: display user name */

  switch (state) {
  case LOGGED_OUT:
    gtk_widget_set_sensitive (data->button, TRUE);
    gtk_label_set_text (GTK_LABEL (data->label), _("Log in pending"));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Log me in"));
    g_signal_connect (data->button, "clicked", G_CALLBACK (log_in_clicked), data);
    break;
  case WORKING:
    gtk_widget_set_sensitive (data->button, FALSE);
    gtk_label_set_text (GTK_LABEL (data->label), _("Log in pending..."));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Working..."));
    break;
  case CONTINUE_AUTH:
    gtk_widget_set_sensitive (data->button, TRUE);
    gtk_label_set_text (GTK_LABEL (data->label), _("Once you have logged in to Flickr, press Continue."));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Continue"));
    g_signal_connect (data->button, "clicked", G_CALLBACK (continue_clicked), data);
    break;
  case LOGGED_IN:
    gtk_widget_set_sensitive (data->button, TRUE);
    if (name) {
      char *s;
      s = g_strdup_printf (_("Logged in as %s"), name);
      gtk_label_set_text (GTK_LABEL (data->label), s);
      g_free (s);
    } else {
      gtk_label_set_text (GTK_LABEL (data->label), _("Logged in"));
    }
    gtk_button_set_label (GTK_BUTTON (data->button), _("Log me out"));
    g_signal_connect (data->button, "clicked", G_CALLBACK (log_out_clicked), data);
    break;
  }
}

static void
find_key_cb (GnomeKeyringResult result,
             const char *string,
             gpointer user_data)
{
  WidgetData *data = user_data;

  if (result == GNOME_KEYRING_RESULT_OK) {
    RestProxyCall *call;
    RestXmlNode *node;

    flickr_proxy_set_token (FLICKR_PROXY (data->proxy), string);

    call = rest_proxy_new_call (data->proxy);
    rest_proxy_call_set_function (call, "flickr.auth.checkToken");

    /* TODO async */
    if (!rest_proxy_call_run (call, NULL, NULL))
      g_error ("Cannot check token");

    node = get_xml (call);
    if (node) {
      got_auth (node, data);
      rest_xml_node_unref (node);
    } else {
      /* The token isn't valid so fake a log out */
      log_out_clicked (NULL, data);
    }
  } else {
    update_widgets (data, LOGGED_OUT, NULL);
  }
}

GtkWidget *
bisho_flickr_pane_new (ServiceInfo *info)
{
  GtkWidget *table, *label;
  WidgetData *data;

  g_assert (info);
  g_assert (info->auth == AUTH_FLICKR);
  g_assert (info->flickr.api_key);
  g_assert (info->flickr.shared_secret);

  data = g_slice_new0 (WidgetData);
  data->info = info;
  data->proxy = flickr_proxy_new (info->flickr.api_key, info->flickr.shared_secret);
  rest_proxy_set_user_agent (data->proxy, "Bisho/" VERSION);

  table = gtk_table_new (2, 2, TRUE);

  label = gtk_label_new (_("<b>Status:</b>"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  data->label = gtk_label_new ("");
  gtk_label_set_line_wrap (GTK_LABEL (data->label), TRUE);
  gtk_misc_set_alignment (GTK_MISC (data->label), 0.0, 0.5);
  gtk_widget_show (data->label);
  gtk_table_attach (GTK_TABLE (table), data->label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

  data->button = gtk_button_new ();
  gtk_widget_show (data->button);
  gtk_table_attach (GTK_TABLE (table), data->button, 1, 2, 0, 1, 0, 0, 0, 0);

  label = make_disclaimer_label (info);
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

  update_widgets (data, LOGGED_OUT, NULL);

  gnome_keyring_find_password (&flickr_schema, find_key_cb, data, NULL,
                               "server", FLICKR_SERVER,
                               "api-key", info->flickr.api_key,
                               NULL);

  return table;
}
