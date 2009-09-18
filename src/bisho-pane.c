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
#include <gtk/gtk.h>
#include <nbtk/nbtk-gtk.h>
#include "bisho-pane.h"
#include "mux-banner.h"
#include "mux-label.h"

G_DEFINE_ABSTRACT_TYPE (BishoPane, bisho_pane, GTK_TYPE_VBOX);

enum {
  PROP_0,
  PROP_SERVICE
};

static void
bisho_pane_get_property (GObject *object, guint property_id,
                         GValue *value, GParamSpec *pspec)
{
  BishoPane *pane = BISHO_PANE (object);

  switch (property_id) {
  case PROP_SERVICE:
    g_value_set_pointer (value, pane->info);
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
bisho_pane_set_property (GObject *object, guint property_id,
                         const GValue *value, GParamSpec *pspec)
{
  BishoPane *pane = BISHO_PANE (object);

  switch (property_id) {
  case PROP_SERVICE:
    {
      GtkTextBuffer *buffer;
      GtkTextIter end;
      char *s;

      pane->info = g_value_get_pointer (value);
      buffer = mux_label_get_buffer (MUX_LABEL (pane->description));

      if (pane->info->description) {
        gtk_text_buffer_get_end_iter (buffer, &end);
        gtk_text_buffer_insert (buffer, &end, pane->info->description, -1);
      }

      if (pane->info->link) {
        GtkTextTag *tag;

        gtk_text_buffer_get_end_iter (buffer, &end);

        tag = mux_label_create_link_tag (MUX_LABEL (pane->description), pane->info->link);

        gtk_text_buffer_insert (buffer, &end, "  ", -1);
        gtk_text_buffer_insert_with_tags (buffer, &end,
                                          _("Launch site for more information."), -1,
                                          tag, NULL);
      }

      s = g_strdup_printf (_("You'll need an account with %s and an Internet connection to use this web service."),
                           pane->info->display_name);
      gtk_label_set_text (GTK_LABEL (pane->disclaimer), s);
      g_free (s);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
bisho_pane_class_init (BishoPaneClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    object_class->get_property = bisho_pane_get_property;
    object_class->set_property = bisho_pane_set_property;

    pspec = g_param_spec_pointer ("service", "service", "service",
                                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_SERVICE, pspec);
}

static void
bisho_pane_init (BishoPane *pane)
{
  pane->description = mux_label_new ();
  gtk_widget_show (pane->description);
  gtk_box_pack_start (GTK_BOX (pane), pane->description, FALSE, FALSE, 0);

  pane->banner = mux_banner_new ();
  gtk_widget_show (pane->banner);
  gtk_box_pack_start (GTK_BOX (pane), pane->banner, FALSE, FALSE, 0);

  pane->content = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (pane->content);
  gtk_box_pack_start (GTK_BOX (pane), pane->content, TRUE, TRUE, 0);

  pane->disclaimer = gtk_label_new (NULL);
  gtk_widget_show (pane->disclaimer);
  gtk_misc_set_alignment (GTK_MISC (pane->disclaimer), 0.0, 0.5);
  gtk_label_set_line_wrap (GTK_LABEL (pane->disclaimer), TRUE);
  gtk_box_pack_start (GTK_BOX (pane), pane->disclaimer, FALSE, FALSE, 0);
}

void
bisho_pane_continue_auth (BishoPane *pane, GHashTable *params)
{
  BishoPaneClass *pane_class = BISHO_PANE_GET_CLASS (pane);

  if (pane_class->continue_auth)
    pane_class->continue_auth (pane, params);
}


GtkWidget *
bisho_pane_make_disclaimer_label (ServiceInfo *info)
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

void
bisho_pane_set_banner (BishoPane *pane, const char *message)
{
  mux_banner_set_text (MUX_BANNER (pane->banner), message);
  gtk_widget_show (pane->banner);
}
