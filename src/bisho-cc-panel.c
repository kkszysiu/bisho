/*
 *
 * Copyright (C) 2010 Intel Corp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "bisho-cc-panel.h"
#include "bisho-frame.h"

struct _BishoCcPanelPrivate {
  GtkWidget *frame;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_CC_PANEL, BishoCcPanelPrivate))

G_DEFINE_DYNAMIC_TYPE (BishoCcPanel, bisho_cc_panel, CC_TYPE_PANEL);

static void
bisho_cc_panel_active_changed (CcPanel  *base_panel,
                              gboolean is_active)
{
  BishoCcPanel *panel = BISHO_CC_PANEL (base_panel);
  static gboolean populated = FALSE;

  if (is_active && !populated) {
    bisho_frame_populate (BISHO_FRAME (panel->priv->frame));
    populated = TRUE;
  }
}

static void
bisho_cc_panel_init (BishoCcPanel *self)
{
  self->priv = GET_PRIVATE (self);

  self->priv->frame = bisho_frame_new ();
  gtk_widget_show (self->priv->frame);
  gtk_container_add (GTK_CONTAINER (self), self->priv->frame);
}

static GObject *
bisho_cc_panel_constructor (GType                  type,
                            guint                  n_construct_properties,
                            GObjectConstructParam *construct_properties)
{
  BishoCcPanel *panel;

  panel = BISHO_CC_PANEL (G_OBJECT_CLASS (bisho_cc_panel_parent_class)->constructor
                          (type, n_construct_properties, construct_properties));

  g_object_set (panel,
                "display-name", _("My Web Accounts"),
                "id", "bisho.desktop",
                NULL);

  return G_OBJECT (panel);
}

static void
bisho_cc_panel_class_init (BishoCcPanelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  CcPanelClass *panel_class = CC_PANEL_CLASS (klass);

  object_class->constructor = bisho_cc_panel_constructor;

  panel_class->active_changed = bisho_cc_panel_active_changed;

  g_type_class_add_private (klass, sizeof (BishoCcPanelPrivate));
}

static void
bisho_cc_panel_class_finalize (BishoCcPanelClass *klass)
{
}

void
bisho_cc_panel_register (GIOModule *module)
{
        bisho_cc_panel_register_type (G_TYPE_MODULE (module));
        g_io_extension_point_implement (CC_PANEL_EXTENSION_POINT_NAME,
                                        BISHO_TYPE_CC_PANEL,
                                        "webservices",
                                        10);
}
