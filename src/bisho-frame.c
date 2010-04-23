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
#include <gtk/gtk.h>
#include <libsocialweb-client/sw-client.h>
#include "mux-expanding-item.h"
#include "bisho-module.h"
#include "bisho-frame.h"
#include "bisho-utils.h"
#include "service-info.h"
#include "bisho-pane-username.h"

struct _BishoFramePrivate {
  SwClient *client;
  GtkWidget *master_box;
  /* Hash of auth type to pane gtypes */
  GHashTable *types;
  /* Hash of string (identifier) to pane widget */
  GHashTable *panes;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_FRAME, BishoFramePrivate))

G_DEFINE_TYPE (BishoFrame, bisho_frame, GTK_TYPE_VBOX);

static void
construct_ui (BishoFrame *frame, const char *service_name)
{
  ServiceInfo *info;
  GtkWidget *expander, *pane;
  GtkBox *box;
  MuxExpandingItem *m;

  g_assert (frame);
  g_assert (service_name);

  info = get_info_for_service (service_name);
  if (info == NULL)
    return;

  expander = mux_expanding_item_new ();
  m = MUX_EXPANDING_ITEM (expander);

  bisho_utils_make_exclusive_expander (m);
  if (info->icon) {
    mux_expanding_item_set_icon_from_file (m, info->icon);
  } else {
    mux_expanding_item_set_label (m, info->display_name);
  }

  box = mux_expanding_item_get_content_box (m);
  gtk_container_set_border_width (GTK_CONTAINER (box), 8);
  gtk_box_set_spacing (box, 8);

  if (g_strcmp0 (info->auth_type, "username") == 0) {
    pane = bisho_pane_username_new (info);
    bisho_pane_username_add_entry
      (BISHO_PANE_USERNAME (pane), _("Username:"), "user", TRUE);
    gtk_widget_show (pane);
    gtk_box_pack_start (GTK_BOX (box), pane, FALSE, FALSE, 0);
  } else if (g_strcmp0 (info->auth_type, "password") == 0) {
    pane = bisho_pane_username_new (info);
    bisho_pane_username_add_entry
      (BISHO_PANE_USERNAME (pane), _("Username:"), "user", TRUE);
    bisho_pane_username_add_entry
      (BISHO_PANE_USERNAME (pane), _("Password:"), "password", FALSE);
    gtk_widget_show (pane);
    gtk_box_pack_start (GTK_BOX (box), pane, FALSE, FALSE, 0);
  } else {
    gpointer pane_type;

    pane_type = g_hash_table_lookup (frame->priv->types, info->auth_type);
    if (pane_type) {
      pane = g_object_new (GPOINTER_TO_INT (pane_type),
                           "socialweb", frame->priv->client,
                           "service", info,
                           NULL);
      gtk_widget_show (pane);
      gtk_box_pack_start (GTK_BOX (box), pane, FALSE, FALSE, 0);
      g_hash_table_insert (frame->priv->panes, info->name, pane);
    }
  }

  gtk_widget_show_all (expander);
  gtk_box_pack_start (GTK_BOX (frame), expander, FALSE, FALSE, 0);
}

static void
client_get_services_cb (SwClient *client,
                        const GList        *services,
                        gpointer      userdata)
{
  BishoFrame *frame = BISHO_FRAME (userdata);
  const GList *l;

  for (l = services; l; l = l->next) {
    construct_ui (frame, l->data);
  }
}

static void
find_panes (BishoFrame *frame)
{
  GType *types;
  guint i, count = 0;

  /* Explicitly register the internal panes */
  g_type_class_peek (BISHO_TYPE_PANE_USERNAME);

  types = g_type_children (BISHO_TYPE_PANE, &count);

  for (i = 0; i < count; i++) {
    GObjectClass *klass;
    const char *auth_type;

    klass = g_type_class_ref (types[i]);

    auth_type = bisho_pane_get_auth_type (BISHO_PANE_CLASS (klass));
    if (auth_type) {
      g_hash_table_insert (frame->priv->types,
                           (gpointer)auth_type,
                           GINT_TO_POINTER (types[i]));
    }

    g_type_class_unref (klass);
  }

  g_free (types);
}

static gpointer
load_modules (gpointer foo)
{
  GError *error = NULL;
  const char *name;
  GDir *dir;

  dir = g_dir_open (PKGLIBDIR, 0, &error);

  if (!dir) {
    if (error->domain != G_FILE_ERROR || error->code != G_FILE_ERROR_NOENT)
      g_printerr ("Cannot open module directory: %s\n", error->message);
    g_error_free (error);
    return NULL;
  }

  while ((name = g_dir_read_name (dir))) {
    if (g_str_has_suffix (name, ".so")) {
      BishoModule *module;
      char *path;

      path = g_build_filename (PKGLIBDIR, name, NULL);
      module = bisho_module_new (path);

      if (!g_type_module_use (G_TYPE_MODULE (module))) {
        g_printerr ("Cannot load module %s\n", path);
        g_object_unref (module);
        g_free (path);
        continue;
      }

      g_free (path);

      g_type_module_unuse (G_TYPE_MODULE (module));
    }
  }

  g_dir_close (dir);

  return NULL;
}

static void
bisho_frame_class_init (BishoFrameClass *klass)
{
  static GOnce once = G_ONCE_INIT;

  g_once (&once, load_modules, NULL);

  g_type_class_add_private (klass, sizeof (BishoFramePrivate));
}

static void
bisho_frame_init (BishoFrame *self)
{
  GtkWidget *label;

  self->priv = GET_PRIVATE (self);

  g_object_set (self,
                "homogeneous", FALSE,
                "spacing", 8,
                "border-width", 8,
                NULL);

  /* Please don't translate 'myzone' */
  label = gtk_label_new (_("Set up your social networks to see new updates in myzone and the status panel."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (self), label, FALSE, FALSE, 0);

  self->priv->panes = g_hash_table_new (g_str_hash, g_str_equal);

  self->priv->types = g_hash_table_new (g_str_hash, g_str_equal);
  find_panes (self);

  self->priv->client = sw_client_new ();
}

GtkWidget *
bisho_frame_new (void)
{
  return g_object_new (BISHO_TYPE_FRAME, NULL);
}

void
bisho_frame_populate (BishoFrame *frame)
{
  g_return_if_fail (BISHO_IS_FRAME (frame));

  sw_client_get_services (frame->priv->client, client_get_services_cb, frame);
}

void
bisho_frame_callback (BishoFrame *frame, const char *id, GHashTable *params)
{
  BishoPane *pane;

  pane = g_hash_table_lookup (frame->priv->panes, id);
  if (pane)
    bisho_pane_continue_auth (pane, params);
}

SwClient *
bisho_frame_get_socialweb (BishoFrame *frame)
{
  return frame->priv->client;
}
