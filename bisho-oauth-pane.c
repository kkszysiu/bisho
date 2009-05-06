#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gnome-keyring.h>
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

typedef struct {
  GtkWidget *label;
  GtkWidget *button;
} WidgetData;

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

static void
update_widgets (WidgetData *data, gboolean logged_in)
{
  if (logged_in) {
    gtk_label_set_text (GTK_LABEL (data->label), _("Logged in"));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Log me out"));
  } else {
    gtk_label_set_text (GTK_LABEL (data->label), _("Log in pending"));
    gtk_button_set_label (GTK_BUTTON (data->button), _("Log me in"));
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
                               "server", info->oauth.server,
                               "consumer-key", info->oauth.consumer_key,
                               NULL);

  return table;
}
