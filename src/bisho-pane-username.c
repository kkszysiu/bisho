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
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include "bisho-pane-username.h"

#define DATA_GCONF_KEY "bisho:gconf-key"

struct _BishoPaneUsernamePrivate {
  GConfClient *gconf;
  GtkWidget *table;
  GtkWidget *button;
  char *key;
  guint rows;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_PANE_USERNAME, BishoPaneUsernamePrivate))
G_DEFINE_TYPE (BishoPaneUsername, bisho_pane_username, BISHO_TYPE_PANE);

static void
login_widget_foreach (GtkWidget *widget, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  const char *key;

  if (!GTK_IS_ENTRY (widget))
    return;

  key = g_object_get_data (G_OBJECT (widget), DATA_GCONF_KEY);
  g_assert (key);

  gconf_client_set_string (pane->priv->gconf, key,
                           gtk_entry_get_text (GTK_ENTRY (widget)),
                           NULL);
}

static void
on_login_clicked (GtkButton *button, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  ServiceInfo *info = NULL;
  char *message;

  gtk_container_foreach (GTK_CONTAINER (pane->priv->table),
                         login_widget_foreach, pane);

  g_object_get (pane, "service", &info, NULL);
  g_assert (info);

  message = g_strdup_printf (_("Log in succeeded. "
                               "You'll see new things from %s in a couple of minutes."),
                             info->display_name);
  bisho_pane_set_banner (BISHO_PANE (pane), message);
  g_free (message);
}

static void
on_entry_activated (GtkEntry *entry, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);

  gtk_widget_child_focus (GTK_WIDGET (pane), GTK_DIR_TAB_FORWARD);
}

static void
logout_widget_foreach (GtkWidget *widget, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  const char *key;

  if (!GTK_IS_ENTRY (widget))
    return;

  gtk_entry_set_text (GTK_ENTRY (widget), "");

  key = g_object_get_data (G_OBJECT (widget), DATA_GCONF_KEY);
  g_assert (key);

  gconf_client_unset (pane->priv->gconf, key, NULL);
}

static void
on_logout_clicked (GtkButton *button, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  ServiceInfo *info = NULL;
  char *message;

  gtk_container_foreach (GTK_CONTAINER (pane->priv->table),
                         logout_widget_foreach, pane);

  g_object_get (pane, "service", &info, NULL);
  g_assert (info);

  message = g_strdup_printf (_("Log out succeeded. "
                               "All trace of %s has been removed from your computer."),
                             info->display_name);
  bisho_pane_set_banner (BISHO_PANE (pane), message);
  g_free (message);
}

static void
bisho_pane_username_init (BishoPaneUsername *self)
{
  GtkWidget *hbox, *vbox, *align, *vbox2, *image, *remove;

  self->priv = GET_PRIVATE (self);
  self->priv->gconf = gconf_client_get_default ();

  hbox = gtk_hbox_new (FALSE, 6);
  gtk_widget_show (hbox);
  gtk_container_add (GTK_CONTAINER (BISHO_PANE (self)->content), hbox);

  vbox = gtk_vbox_new (FALSE, 6);
  gtk_widget_show (vbox);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, TRUE, TRUE, 0);

  self->priv->table = gtk_table_new (0, 0, FALSE);
  g_object_set (self->priv->table,
                "row-spacing", 6,
                "column-spacing", 6,
                NULL);
  gtk_widget_show (self->priv->table);
  gtk_container_add (GTK_CONTAINER (vbox), self->priv->table);

  align = gtk_alignment_new (1, 0, 0, 0);
  gtk_widget_show (align);
  gtk_box_pack_start (GTK_BOX (vbox), align, FALSE, FALSE, 0);

  self->priv->button = gtk_button_new_with_label (_("Log in"));
  g_signal_connect (self->priv->button, "clicked", G_CALLBACK (on_login_clicked), self);
  gtk_widget_show (self->priv->button);
  gtk_container_add (GTK_CONTAINER (align), self->priv->button);

  vbox2 = gtk_vbox_new (FALSE, 6);
  gtk_widget_show (vbox2);
  gtk_box_pack_start (GTK_BOX (hbox), vbox2, FALSE, FALSE, 0);

  remove = gtk_button_new ();
  g_signal_connect (remove, "clicked", G_CALLBACK (on_logout_clicked), self);
  gtk_button_set_relief (GTK_BUTTON (remove), GTK_RELIEF_NONE);
  image = gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image);
  gtk_container_add (GTK_CONTAINER (remove), image);
  gtk_widget_show (remove);
  gtk_box_pack_start (GTK_BOX (vbox2), remove, FALSE, FALSE, 0);
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

  return g_object_new (BISHO_TYPE_PANE_USERNAME,
                       "service", info,
                       NULL);
}

void
bisho_pane_username_add_entry (BishoPaneUsername *pane, const char *label, const char *key, gboolean visible)
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
  gtk_entry_set_visibility (GTK_ENTRY (entry), visible);
  gtk_entry_set_width_chars (GTK_ENTRY (entry), 30);
  g_signal_connect (entry, "activate", G_CALLBACK (on_entry_activated), pane);
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (priv->table), entry,
                    1, 2, priv->rows, priv->rows + 1, GTK_FILL, GTK_FILL, 0, 0);

  gconf_key = g_strdup_printf ("/apps/libsocialweb/services/%s/%s", info->name, key);
  g_object_set_data_full (G_OBJECT (entry), DATA_GCONF_KEY, gconf_key, g_free);

  value = gconf_client_get_string (priv->gconf, gconf_key, NULL);
  if (value) {
    gtk_entry_set_text (GTK_ENTRY (entry), value);
    g_free (value);
  }

  priv->rows++;
}
