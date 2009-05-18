#include <gtk/gtk.h>
#include <gconf/gconf-client.h>
#include "service-info.h"
#include "bisho-window.h"
#include "entry.h"

static gboolean done_init = FALSE;
static GConfClient *gconf = NULL;
/* TODO: very bad */
static BishoWindow *todo_window = NULL;

#define DATA_GCONF_KEY "bisho:gconf-key"

static void
init (BishoWindow *window)
{
  g_assert (gconf == NULL);

  gconf = gconf_client_get_default ();

  gconf_client_add_dir (gconf, "/apps/mojito/services", GCONF_CLIENT_PRELOAD_RECURSIVE, NULL);

  todo_window = window;

  done_init = TRUE;
}

static void
set_gconf_key (ServiceInfo *info, const char *key, const char *value)
{
  /* TODO: block gconf notify when we have one */
  gconf_client_set_string (gconf, key, value, NULL);

  bisho_window_change_banner (todo_window, info);
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

GtkWidget *
new_entry_from_gconf (BishoWindow *window, ServiceInfo *info, const char *key_suffix)
{
  GtkWidget *entry;
  char *key, *value;

  g_assert (info);
  g_assert (key_suffix);

  if (!done_init) init (window);

  key = g_strdup_printf ("/apps/mojito/services/%s/%s", info->name, key_suffix);

  entry = gtk_entry_new ();
  g_object_set_data_full (G_OBJECT (entry), DATA_GCONF_KEY, key, g_free);

  value = gconf_client_get_string (gconf, key, NULL);
  gtk_entry_set_text (GTK_ENTRY (entry), value);
  g_free (value);

  /* TODO: connect to gconf notify */
  g_signal_connect (entry, "focus-out-event", G_CALLBACK (on_gconf_entry_left), info);

  return entry;
}
