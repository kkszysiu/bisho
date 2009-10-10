#include <config.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include "bisho-pane-username.h"

struct _BishoPaneUsernamePrivate {
  GConfClient *gconf;
  GtkWidget *entry;
  char *key;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_PANE_USERNAME, BishoPaneUsernamePrivate))
G_DEFINE_TYPE (BishoPaneUsername, bisho_pane_username, BISHO_TYPE_PANE);

static gboolean
on_entry_left (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  ServiceInfo *info = NULL;
  char *message;

  g_object_get (pane, "service", &info, NULL);
  g_assert (info);

  gconf_client_set_string (pane->priv->gconf,
                           pane->priv->key,
                           gtk_entry_get_text (GTK_ENTRY (widget)),
                           NULL);

  message = g_strdup_printf (_("%s login changed."), info->display_name);
  bisho_pane_set_banner (BISHO_PANE (pane), message);
  g_free (message);

  return FALSE;
}

static void
bisho_pane_username_constructed (GObject *object)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (object);
  ServiceInfo *info = NULL;
  char *value;

  pane->priv->gconf = gconf_client_get_default ();

  g_object_get (pane, "service", &info, NULL);
  g_assert (info);

  pane->priv->key = g_strdup_printf ("/apps/mojito/services/%s/user", info->name);

  value = gconf_client_get_string (pane->priv->gconf, pane->priv->key, NULL);
  gtk_entry_set_text (GTK_ENTRY (pane->priv->entry), value);
  g_free (value);
}

static void
bisho_pane_username_init (BishoPaneUsername *self)
{
  GtkWidget *content, *table, *label;

  self->priv = GET_PRIVATE (self);

  content = BISHO_PANE (self)->content;
  g_assert (content);

  table = gtk_table_new (1, 2, FALSE);

  label = gtk_label_new (_("Username:"));
  gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

  self->priv->entry = gtk_entry_new ();
  g_signal_connect (self->priv->entry, "focus-out-event", G_CALLBACK (on_entry_left), self);
  gtk_table_attach_defaults (GTK_TABLE (table), self->priv->entry, 1, 2, 0, 1);

  gtk_widget_show_all (table);
  gtk_box_pack_start (GTK_BOX (content), table, FALSE, FALSE, 0);
}

static void
bisho_pane_username_class_init (BishoPaneUsernameClass *klass)
{
  GObjectClass *o_class = G_OBJECT_CLASS (klass);

  o_class->constructed = bisho_pane_username_constructed;

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
