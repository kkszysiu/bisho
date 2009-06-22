/*
* libmux - GTK+ Moblin User Experience widgets
 * Copyright (C) 2009 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

/*
 * TODO: GTK+ in git has support for hyperlinks in labels which makes this
 * entire class redundant.  When we have that version of GTK+, drop this.
 */

#include <gtk/gtk.h>
#include "mux-banner.h"

G_DEFINE_TYPE (MuxBanner, mux_banner, GTK_TYPE_TEXT_VIEW);

static void
mux_banner_class_init (MuxBannerClass *klass)
{
}

static void
mux_banner_init (MuxBanner *self)
{
  GtkTextView *text = GTK_TEXT_VIEW (self);

  g_object_set (text,
                "editable", FALSE,
                "cursor-visible", FALSE,
                "wrap-mode", GTK_WRAP_WORD,
                "left-margin", 6,
                "right-margin", 6,
                "pixels-above-lines", 6,
                "pixels-below-lines", 6,
                NULL);

  /* TODO: find some cunning way of disabling text selection */
}

GtkWidget *
mux_banner_new (void)
{
  return g_object_new (MUX_TYPE_BANNER, NULL);
}

GtkTextBuffer *
mux_banner_get_buffer (MuxBanner *banner)
{
  g_return_val_if_fail (MUX_IS_BANNER (banner), NULL);

  return gtk_text_view_get_buffer (GTK_TEXT_VIEW (banner));
}

static gboolean
on_link_tag_event (GtkTextTag  *tag,
               GObject     *object,
               GdkEvent    *event,
               GtkTextIter *iter,
               gpointer     user_data)
{
  if (event->type == GDK_BUTTON_PRESS && event->button.button == 1) {
    GtkWidget *widget = GTK_WIDGET (object);
    const char *url = user_data;

    gtk_show_uri (gtk_widget_get_screen (widget), url,
                  event->button.time, NULL);

    return TRUE;
  }
  return FALSE;
}

GtkTextTag *
mux_banner_create_link_tag (MuxBanner *banner, const char *url)
{
  GtkTextTag *tag;

  /* TODO: make a subclass of GtkTextTag for cleanliness and so that we don't
     leak the URL */

  g_return_val_if_fail (MUX_IS_BANNER (banner), NULL);

  tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer (GTK_TEXT_VIEW (banner)),
                              NULL,
                              "foreground", "#009bce",
                              "underline", PANGO_UNDERLINE_SINGLE,
                              "scale", PANGO_SCALE_SMALL,
                              NULL);

  if (url)
    g_signal_connect (tag, "event", G_CALLBACK (on_link_tag_event), g_strdup (url));

  return tag;
}

void
mux_banner_set_text (MuxBanner *banner, const char *text)
{
  g_return_if_fail (MUX_IS_BANNER (banner));

  gtk_text_buffer_set_text (mux_banner_get_buffer (banner), text, -1);
}
