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

static void update_widgets (WidgetData *data, ButtonState state);

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
log_in_clicked (GtkWidget *button, gpointer user_data)
{
  WidgetData *data = user_data;
  ServiceInfo *info = data->info;
  GError *error = NULL;
  char *url;

  update_widgets (data, WORKING);

  /* TODO: async */
  oauth_proxy_auth_step (OAUTH_PROXY (data->proxy), info->oauth.request_token_function, &error);
  if (error) {
    /* TODO */
    g_warning ("%s", error->message);
    update_widgets (data, LOGGED_OUT);
    return;
  }

  url = create_url (info, oauth_proxy_get_token (OAUTH_PROXY (data->proxy)));
  gtk_show_uri (gtk_widget_get_screen (GTK_WIDGET (button)), url, GDK_CURRENT_TIME, NULL);

  /* TODO wait for dbus call from callback */
  update_widgets (data, CONTINUE_AUTH);
}


static void
delete_done_cb (GnomeKeyringResult result, gpointer user_data)
{
  WidgetData *data = user_data;

  if (GNOME_KEYRING_RESULT_OK)
    update_widgets (data, LOGGED_OUT);
  else
    update_widgets (data, LOGGED_IN);
}

static void
log_out_clicked (GtkButton *button, gpointer user_data)
{
  WidgetData *data = user_data;

  gnome_keyring_delete_password (&oauth_schema, delete_done_cb, user_data, NULL,
                                 "server", data->info->oauth.base_url,
                                 "consumer-key", data->info->oauth.consumer_key,
                                 NULL);

  update_widgets (data, LOGGED_OUT);
}

static char *
encode (const char *token, const char *secret)
{
  char *encoded_token, *encoded_secret;
  char *string;

  g_assert (token);
  g_assert (secret);

  encoded_token = g_base64_encode ((guchar*)token, strlen (token));
  encoded_secret = g_base64_encode ((guchar*)secret, strlen (secret));

  string = g_strconcat (encoded_token, " ", encoded_secret, NULL);

  g_free (encoded_token);
  g_free (encoded_secret);

  return string;
}

static void
continue_clicked (GtkWidget *button, gpointer user_data)
{
  WidgetData *data = user_data;
  GError *error = NULL;
  char *encoded;

  update_widgets (data, WORKING);

  /* TODO: async */
  oauth_proxy_auth_step (OAUTH_PROXY (data->proxy), data->info->oauth.access_token_function, &error);
  if (error) {
    /* TODO */
    g_warning ("%s", error->message);
    update_widgets (data, LOGGED_OUT);
    return;
  }

  encoded = encode (oauth_proxy_get_token (OAUTH_PROXY (data->proxy)),
                           oauth_proxy_get_token_secret (OAUTH_PROXY (data->proxy)));

  /* TODO async */
  GnomeKeyringResult result;
  GnomeKeyringAttributeList *attrs;
  guint32 id;
  attrs = gnome_keyring_attribute_list_new ();
  gnome_keyring_attribute_list_append_string (attrs, "server", data->info->oauth.base_url);
  gnome_keyring_attribute_list_append_string (attrs, "consumer-key", data->info->oauth.consumer_key);

  result = gnome_keyring_item_create_sync (NULL,
                                           GNOME_KEYRING_ITEM_GENERIC_SECRET,
                                           data->info->display_name,
                                           attrs, encoded,
                                           TRUE, &id);

  if (result == GNOME_KEYRING_RESULT_OK) {
    gnome_keyring_item_grant_access_rights_sync (NULL,
                                                 "mojito",
                                                 LIBEXECDIR "/mojito-core",
                                                 id, GNOME_KEYRING_ACCESS_READ);
    update_widgets (data, LOGGED_IN);
  } else {
    update_widgets (data, LOGGED_OUT);
  }
}

static void
update_widgets (WidgetData *data, ButtonState state)
{
  g_signal_handlers_disconnect_by_func (data->button, log_out_clicked, data);
  g_signal_handlers_disconnect_by_func (data->button, continue_clicked, data);
  g_signal_handlers_disconnect_by_func (data->button, log_in_clicked, data);

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
    gtk_label_set_text (GTK_LABEL (data->label), _("Press Continue to continue the log in."));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Continue"));
    g_signal_connect (data->button, "clicked", G_CALLBACK (continue_clicked), data);
    break;
  case LOGGED_IN:
    gtk_widget_set_sensitive (data->button, TRUE);
    gtk_label_set_text (GTK_LABEL (data->label), _("Logged in"));
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

  if (result == GNOME_KEYRING_RESULT_OK)
    update_widgets (data, LOGGED_IN);
  else
    update_widgets (data, LOGGED_OUT);
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
  data->proxy = oauth_proxy_new (info->oauth.consumer_key,
                                 info->oauth.consumer_secret,
                                 info->oauth.base_url, FALSE);

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

  update_widgets (data, LOGGED_OUT);

  gnome_keyring_find_password (&oauth_schema, find_key_cb, data, NULL,
                               "server", info->oauth.base_url,
                               "consumer-key", info->oauth.consumer_key,
                               NULL);

  return table;
}
