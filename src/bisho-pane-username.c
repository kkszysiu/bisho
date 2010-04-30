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
#include <gconf/gconf-client.h>
#include <gtk/gtk.h>
#include "bisho-pane-username.h"

#define DATA_GCONF_KEY "bisho:gconf-key"

struct _BishoPaneUsernamePrivate {
  SwClientService *service;
  gboolean started; /* so we don't show logged in banners on startup */
  gboolean can_verify;
  GConfClient *gconf;
  GtkWidget *table;
  GtkWidget *button;
  GtkWidget *logout_button;
  char *key;
  guint rows;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_PANE_USERNAME, BishoPaneUsernamePrivate))
G_DEFINE_TYPE (BishoPaneUsername, bisho_pane_username, BISHO_TYPE_PANE);

static void
add_banner (BishoPaneUsername *u_pane, gboolean success)
{
  BishoPane *pane = (BishoPane *)u_pane;
  char *message;

  /* Don't do success banners if we've not fully started */
  if (success && !u_pane->priv->started)
    return;

  if (success) {
    message = g_strdup_printf (_("Log in succeeded. "
                                 "You'll see new things from %s in a couple of minutes."),
                               pane->info->display_name);
  } else {
    message = g_strdup_printf (_("Sorry, we can't log into %s."),
                               pane->info->display_name);
  }

  bisho_pane_set_banner (pane, message);
  g_free (message);
}

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

  gtk_container_foreach (GTK_CONTAINER (pane->priv->table),
                         login_widget_foreach, pane);

  /* If we are not watching for the verify signal, show the banner now */
  if (!pane->priv->can_verify) {
    add_banner (pane, TRUE);
  }
}

static void
on_entry_activated (GtkEntry *entry, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);

  gtk_widget_child_focus (GTK_WIDGET (pane), GTK_DIR_TAB_FORWARD);
  /* TODO: examine the focus chain and if the next widget is the button,
     activate it */
}

static void
entries_have_content (GtkWidget *widget, gpointer user_data)
{
  gboolean *has_content = user_data;

  if (!GTK_IS_ENTRY (widget))
    return;

  if (gtk_entry_get_text_length (GTK_ENTRY (widget)) > 0)
    *has_content = TRUE;
}

static void
on_entry_changed (GtkEditable *editable, gpointer user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);
  gboolean has_content = FALSE;

  gtk_container_foreach (GTK_CONTAINER (pane->priv->table),
                         entries_have_content, &has_content);

  gtk_widget_set_sensitive (pane->priv->logout_button, has_content);
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
on_caps_changed (SwClientService *service, const char **caps, gpointer user_data)
{
  BishoPaneUsername *u_pane = BISHO_PANE_USERNAME (user_data);
  gboolean configured = sw_client_service_has_cap (caps, IS_CONFIGURED);
  gboolean valid = sw_client_service_has_cap (caps, CREDENTIALS_VALID);
  gboolean invalid = sw_client_service_has_cap (caps, CREDENTIALS_INVALID);

  if (configured) {
    if (valid) {
      gtk_button_set_label (GTK_BUTTON (u_pane->priv->button), _("Update"));
      add_banner (u_pane, TRUE);
    } else if (invalid) {
      gtk_button_set_label (GTK_BUTTON (u_pane->priv->button), _("Log in"));
      add_banner (u_pane, FALSE);
    }
  } else {
    gtk_button_set_label (GTK_BUTTON (u_pane->priv->button), _("Log in"));
  }
}

static void
got_dynamic_caps_cb (SwClientService  *service,
                     const char      **caps,
                     const GError     *error,
                     gpointer          user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);

  if (error) {
    g_message ("Cannot get dynamic caps: %s", error->message);
    return;
  }

  on_caps_changed (service, caps, pane);

  pane->priv->started = TRUE;
}

static void
got_static_caps_cb (SwClientService  *service,
                     const char      **caps,
                     const GError     *error,
                     gpointer          user_data)
{
  BishoPaneUsername *pane = BISHO_PANE_USERNAME (user_data);

  if (error) {
    g_message ("Cannot get static caps: %s", error->message);
    return;
  }

  if (sw_client_service_has_cap (caps, CAN_VERIFY_CREDENTIALS)) {
    pane->priv->can_verify = TRUE;
    g_signal_connect (pane->priv->service, "capabilities-changed", G_CALLBACK (on_caps_changed), pane);
    sw_client_service_get_dynamic_capabilities (pane->priv->service, got_dynamic_caps_cb, pane);
  }
}

static void
bisho_pane_username_init (BishoPaneUsername *self)
{
  GtkWidget *hbox, *vbox, *align, *vbox2, *image;

  self->priv = GET_PRIVATE (self);
  self->priv->gconf = gconf_client_get_default ();
  self->priv->started = FALSE;
  self->priv->can_verify = FALSE;

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

  self->priv->logout_button = gtk_button_new ();
  g_signal_connect (self->priv->logout_button, "clicked", G_CALLBACK (on_logout_clicked), self);
  image = gtk_image_new_from_stock (GTK_STOCK_CLEAR, GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image);
  gtk_container_add (GTK_CONTAINER (self->priv->logout_button), image);
  gtk_widget_show (self->priv->logout_button);
  gtk_box_pack_start (GTK_BOX (vbox2), self->priv->logout_button, FALSE, FALSE, 0);
}

static void
bisho_pane_username_constructed (GObject *object)
{
  BishoPaneUsername *u_pane = (BishoPaneUsername*)object;
  BishoPane *pane = (BishoPane *)u_pane;

  u_pane->priv->service = sw_client_get_service (pane->socialweb, pane->info->name);
  sw_client_service_get_static_capabilities (u_pane->priv->service, got_static_caps_cb, pane);
}

static void
bisho_pane_username_class_init (BishoPaneUsernameClass *klass)
{
  GObjectClass *o_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (BishoPaneUsernamePrivate));

  o_class->constructed = bisho_pane_username_constructed;
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
  g_signal_connect (entry, "changed", G_CALLBACK (on_entry_changed), pane);
  gtk_widget_show (entry);
  gtk_table_attach (GTK_TABLE (priv->table), entry,
                    1, 2, priv->rows, priv->rows + 1, GTK_FILL, GTK_FILL, 0, 0);

  gconf_key = g_strdup_printf ("/apps/libsocialweb/services/%s/%s", info->name, key);
  g_object_set_data_full (G_OBJECT (entry), DATA_GCONF_KEY, gconf_key, g_free);

  value = gconf_client_get_string (priv->gconf, gconf_key, NULL);
  /* Even though we may be setting the text to NULL that isn't a problem because
     it ensures that the changed handler is fired. */
  gtk_entry_set_text (GTK_ENTRY (entry), value);
  g_free (value);

  priv->rows++;
}
