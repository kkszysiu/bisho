#include <gtk/gtk.h>
#include <gconf/gconf-client.h>
#include "service-info.h"

static GConfClient *gconf = NULL;

#define DATA_GCONF_KEY "bisho:gconf-key"

static void
set_gconf_key (ServiceInfo *info, const char *key, const char *value)
{
  /* TODO: block gconf notify when we have one */
  gconf_client_set_string (gconf, key, value, NULL);
}

static gboolean
on_gconf_entry_left (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
  ServiceInfo *info = user_data;
  const char *key;

  key = g_object_get_data (G_OBJECT (widget), DATA_GCONF_KEY);
  set_gconf_key (info, key, gtk_entry_get_text (GTK_ENTRY (widget)));

  return FALSE;
}

static char *
get_gconf_key (ServiceInfo *info, const char *key)
{
  char *value;
  GError *error = NULL;

  value = gconf_client_get_string (gconf, key, &error);
  if (error) {
    g_message ("Cannot get key %s: %s", key, error->message);
    g_error_free (error);
  }

  return value;
}

GtkWidget *
new_entry_from_gconf (ServiceInfo *info, const char *key_suffix)
{
  GtkWidget *entry;
  char *key, *value;

  g_assert (info);
  g_assert (key);

  if (!gconf) gconf = gconf_client_get_default ();

  key = g_strdup_printf ("/apps/mojito/services/%s/%s", info->name, key_suffix);

  entry = gtk_entry_new ();
  g_object_set_data_full (G_OBJECT (entry), DATA_GCONF_KEY, key, g_free);

  value = get_gconf_key (info, key);
  gtk_entry_set_text (GTK_ENTRY (entry), value);
  g_free (value);

  /* TODO: connect to gconf notify */
  g_signal_connect (entry, "focus-out-event", G_CALLBACK (on_gconf_entry_left), info);

  return entry;
}
