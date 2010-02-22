/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
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

#include "config.h"

#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <glib/gi18n-lib.h>
#include <gconf/gconf-client.h>

#include "bisho-cc-panel.h"
#include "bisho-cc-page.h"

#define BISHO_CC_PANEL_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), BISHO_TYPE_CC_PANEL, BishoCcPanelPrivate))

struct BishoCcPanelPrivate
{
        GtkWidget *notebook;
        CcPage    *page;
};

static void     bisho_cc_panel_class_init     (BishoCcPanelClass *klass);
static void     bisho_cc_panel_init           (BishoCcPanel      *panel);

G_DEFINE_DYNAMIC_TYPE (BishoCcPanel, bisho_cc_panel, CC_TYPE_PANEL)

static void
setup_panel (BishoCcPanel *panel)
{
        GtkWidget *label;
        char      *display_name;

        panel->priv->notebook = gtk_notebook_new ();
        gtk_container_add (GTK_CONTAINER (panel), panel->priv->notebook);
        gtk_widget_show (panel->priv->notebook);

        panel->priv->page = bisho_cc_page_new ();
        g_object_get (panel->priv->page,
                      "display-name", &display_name,
                      NULL);
        label = gtk_label_new (display_name);
        g_free (display_name);
        gtk_notebook_append_page (GTK_NOTEBOOK (panel->priv->notebook),
                                  GTK_WIDGET (panel->priv->page),
                                  label);
        gtk_widget_show (GTK_WIDGET (panel->priv->page));

        gtk_notebook_set_show_tabs (GTK_NOTEBOOK (panel->priv->notebook), FALSE);

        g_object_set (panel,
                      "current-page", panel->priv->page,
                      NULL);
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

        setup_panel (panel);

        return G_OBJECT (panel);
}

static void
bisho_cc_panel_class_init (BishoCcPanelClass *klass)
{
        GObjectClass *object_class = G_OBJECT_CLASS (klass);

        object_class->constructor = bisho_cc_panel_constructor;

        g_type_class_add_private (klass, sizeof (BishoCcPanelPrivate));
}

static void
bisho_cc_panel_class_finalize (BishoCcPanelClass *klass)
{
}

static void
bisho_cc_panel_init (BishoCcPanel *panel)
{
        panel->priv = BISHO_CC_PANEL_GET_PRIVATE (panel);
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
