#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gnome-keyring.h>
#include <libsoup/soup.h>
#include <rest/oauth-proxy.h>
#include "service-info.h"
#include "bisho-oauth-pane.h"

/* TODO: use mojito-keyring */
static const GnomeKeyringPasswordSchema oauth_schema = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
    { "server", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { "consumer-key", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
    { NULL, 0 }
  }
};

/* TODO: make this a widget subclass and this private data */
typedef struct {
  ServiceInfo *info;
  GtkWidget *label;
  GtkWidget *button;
} WidgetData;

static void update_widgets (WidgetData *data, gboolean logged_in);

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

static char *
create_url (ServiceInfo *info, const char *token)
{
  SoupURI *base, *uri;
  char *s;

  g_assert (info);
  g_assert (info->oauth.callback);
  g_assert (token);

  base = soup_uri_new (info->oauth.base_url);
  uri = soup_uri_new_with_base (base, info->oauth.authorize_function);
  soup_uri_free (base);

  soup_uri_set_query_from_fields (uri,
                                  "oauth_token", token,
                                  "oauth_callback", info->oauth.callback,
                                  NULL);

  s = soup_uri_to_string (uri, FALSE);
  soup_uri_free (uri);
  return s;
}

static void
log_in_clicked (GtkButton *button, gpointer user_data)
{
  WidgetData *data = user_data;
  ServiceInfo *info = data->info;
  RestProxy *proxy;
  GError *error = NULL;
  char *url;

  proxy = oauth_proxy_new (info->oauth.consumer_key, info->oauth.consumer_secret, info->oauth.base_url, FALSE);

  /* TODO: async and change button title/disable*/
  oauth_proxy_auth_step (OAUTH_PROXY (proxy), info->oauth.request_token_function, &error);
  if (error) {
    /* TODO */
    g_warning ("%s", error->message);
    return;
  }

  url = create_url (info, oauth_proxy_get_token (OAUTH_PROXY (proxy)));
  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (button)), url, GDK_CURRENT_TIME, NULL);

  /* TODO wait for dbus call from callback. For now pop up a lame dialog or something */
}


static void
delete_done_cb (GnomeKeyringResult result, gpointer user_data)
{
  WidgetData *data = user_data;
  update_widgets (data, result != GNOME_KEYRING_RESULT_OK);
}

static void
log_out_clicked (GtkButton *button, gpointer user_data)
{
  WidgetData *data = user_data;

  gnome_keyring_delete_password (&oauth_schema, delete_done_cb, user_data, NULL,
                                 "server", data->info->oauth.base_url,
                                 "consumer-key", data->info->oauth.consumer_key,
                                 NULL);
}

static void
update_widgets (WidgetData *data, gboolean logged_in)
{
  g_signal_handlers_disconnect_by_func (data->button, log_out_clicked, data);
  g_signal_handlers_disconnect_by_func (data->button, log_in_clicked, data);

  if (logged_in) {
    gtk_label_set_text (GTK_LABEL (data->label), _("Logged in"));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Log me out"));
    g_signal_connect (data->button, "clicked", G_CALLBACK (log_out_clicked), data);
  } else {
    gtk_label_set_text (GTK_LABEL (data->label), _("Log in pending"));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Log me in"));
    g_signal_connect (data->button, "clicked", G_CALLBACK (log_in_clicked), data);
  }
}

static void
find_key_cb (GnomeKeyringResult result,
             const char *string,
             gpointer user_data)
{
  WidgetData *data = user_data;
  update_widgets (data, result == GNOME_KEYRING_RESULT_OK);
}

GtkWidget *
bisho_oauth_pane_new (ServiceInfo *info)
{
  GtkWidget *table, *label;
  WidgetData *data;

  g_assert (info);
  g_assert (info->auth == AUTH_OAUTH);

  data = g_slice_new0 (WidgetData);
  data->info = info;

  table = gtk_table_new (2, 2, TRUE);

  label = gtk_label_new (_("<b>Status:</b>"));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_label_set_use_markup (GTK_LABEL (label), TRUE);
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  data->label = gtk_label_new ("");
  gtk_misc_set_alignment (GTK_MISC (data->label), 0.0, 0.5);
  gtk_widget_show (data->label);
  gtk_table_attach (GTK_TABLE (table), data->label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

  data->button = gtk_button_new ();
  gtk_widget_show (data->button);
  gtk_table_attach (GTK_TABLE (table), data->button, 1, 2, 0, 1, 0, 0, 0, 0);

  label = make_disclaimer_label (info);
  gtk_widget_show (label);
  gtk_table_attach (GTK_TABLE (table), label, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

  update_widgets (data, FALSE);

  gnome_keyring_find_password (&oauth_schema, find_key_cb, data, NULL,
                               "server", info->oauth.base_url,
                               "consumer-key", info->oauth.consumer_key,
                               NULL);

  return table;
}
