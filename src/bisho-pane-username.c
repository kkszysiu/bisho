#include <config.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include "bisho-pane-username.h"

#define DATA_GCONF_KEY "bisho:gconf-key"

struct _BishoPaneUsernamePrivate {
  GConfClient *gconf;
  GtkWidget *table;
  GtkWidget *entry;
  char *key;
  guint rows;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_PANE_USERNAME, BishoPaneUsernamePrivate))
G_DEFINE_TYPE (BishoPaneUsername, bisho_pane_username, BISHO_TYPE_PANE);

static gboolean
on_entry_left (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  ServiceInfo *info = NULL;
  const char *key;
  char *message;

  g_object_get (pane, "service", &info, NULL);
  g_assert (info);

  key = g_object_get_data (G_OBJECT (widget), DATA_GCONF_KEY);
  g_assert (key);

  gconf_client_set_string (pane->priv->gconf, key,
                           gtk_entry_get_text (GTK_ENTRY (widget)),
                           NULL);

  message = g_strdup_printf (_("%s login changed."), info->display_name);
  bisho_pane_set_banner (BISHO_PANE (pane), message);
  g_free (message);

  return FALSE;
}

static void
bisho_pane_username_init (BishoPaneUsername *self)
{
  self->priv = GET_PRIVATE (self);
  self->priv->gconf = gconf_client_get_default ();

  self->priv->table = gtk_table_new (0, 0, FALSE);
  g_object_set (self->priv->table,
                "row-spacing", 6,
                "column-spacing", 6,
                NULL);
  gtk_widget_show (self->priv->table);
  gtk_container_add (GTK_CONTAINER (BISHO_PANE (self)->content), self->priv->table);
}

static void
bisho_pane_username_class_init (BishoPaneUsernameClass *klass)
{
  g_type_class_add_private (klass, sizeof (BishoPaneUsernamePrivate));
}

GtkWidget *
bisho_pane_username_new (ServiceInfo *info)
{
  g_assert (info);
  g_assert (info->auth == AUTH_USERNAME);

  return g_object_new (BISHO_TYPE_PANE_USERNAME,
                       "service", info,
                       NULL);
}

void
bisho_pane_username_add_entry (BishoPaneUsername *pane, const char *label, const char *key)
{
  BishoPaneUsernamePrivate *priv;
  GtkWidget *label_w, *entry;
  char *gconf_key, *value;
  ServiceInfo *info;

  g_return_if_fail (BISHO_IS_PANE_USERNAME (pane));
  g_return_if_fail (label);
  g_return_if_fail (key);

  priv = pane->priv;
  g_object_get (pane, "service", &info, NULL);
  g_assert (info);

  label_w = gtk_label_new (label);
  gtk_widget_show (label_w);
  gtk_table_attach (GTK_TABLE (priv->table), label_w,
                    0, 1, priv->rows, priv->rows + 1, GTK_FILL, GTK_FILL, 0, 0);

  entry = gtk_entry_new ();
  gtk_entry_set_width_chars (GTK_ENTRY (entry), 30);
  g_signal_connect (entry, "focus-out-event", G_CALLBACK (on_entry_left), pane);
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (priv->table), entry,
                    1, 2, priv->rows, priv->rows + 1, GTK_FILL, GTK_FILL, 0, 0);

  gconf_key = g_strdup_printf ("/apps/mojito/services/%s/%s", info->name, key);
  g_object_set_data_full (G_OBJECT (entry), DATA_GCONF_KEY, gconf_key, g_free);

  value = gconf_client_get_string (priv->gconf, gconf_key, NULL);
  if (value) {
    gtk_entry_set_text (GTK_ENTRY (entry), value);
    g_free (value);
  }

  priv->rows++;
}
