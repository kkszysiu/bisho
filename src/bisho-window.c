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
#include "bisho-window.h"
#include "bisho-frame.h"

G_DEFINE_TYPE (BishoWindow, bisho_window, GTK_TYPE_WINDOW);

static void
bisho_window_class_init (BishoWindowClass *klass)
{
}

static void
bisho_window_init (BishoWindow *self)
{
  GdkScreen *screen;
  GtkWidget *box, *toolbar, *icon, *quit;
  GtkToolItem *sep;

  gtk_window_set_title (GTK_WINDOW (self), _("My Web Accounts"));
  gtk_window_set_icon_name (GTK_WINDOW (self), "bisho");
  gtk_window_set_decorated (GTK_WINDOW (self), FALSE);

  screen = gtk_widget_get_screen (GTK_WIDGET (self));
  gtk_window_set_default_size (GTK_WINDOW (self),
                               gdk_screen_get_width (screen),
                               gdk_screen_get_height (screen));

  box = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (self), box);

  toolbar = gtk_toolbar_new ();
  gtk_widget_set_name (toolbar, "MoblinToolbar");
  gtk_widget_show (toolbar);
  gtk_box_pack_start (GTK_BOX (box), toolbar, FALSE, FALSE, 0);

  sep = gtk_separator_tool_item_new ();
  gtk_separator_tool_item_set_draw (GTK_SEPARATOR_TOOL_ITEM (sep), FALSE);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (sep), TRUE);
  gtk_widget_show (GTK_WIDGET (sep));
  gtk_toolbar_insert ((GtkToolbar *)toolbar, sep, 0);

  icon = gtk_image_new_from_icon_name (GTK_STOCK_CLOSE, GTK_ICON_SIZE_BUTTON);
  quit = (GtkWidget *)gtk_tool_button_new (icon, NULL);
  gtk_widget_set_tooltip_text (quit, _("Quit"));
  g_signal_connect (quit, "clicked", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (quit);
  gtk_toolbar_insert ((GtkToolbar *)toolbar, (GtkToolItem *)quit, -1);

  self->frame = bisho_frame_new ();
  bisho_frame_populate (BISHO_FRAME (self->frame));
  gtk_widget_show (self->frame);
  gtk_box_pack_start (GTK_BOX (box), self->frame, TRUE, TRUE, 0);
}

GtkWidget *
bisho_window_new (void)
{
  return g_object_new (BISHO_TYPE_WINDOW, NULL);
}
