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
#include "mux-label.h"

G_DEFINE_TYPE (MuxLabel, mux_label, GTK_TYPE_TEXT_VIEW);

static void
mux_label_style_set (GtkWidget *widget, GtkStyle *previous)
{
  GTK_WIDGET_CLASS (mux_label_parent_class)->style_set (widget, previous);

  widget->style->base[GTK_STATE_NORMAL] = widget->style->bg[GTK_STATE_NORMAL];
  widget->style->text[GTK_STATE_NORMAL] = widget->style->fg[GTK_STATE_NORMAL];
}

static void
mux_label_class_init (MuxLabelClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->style_set = mux_label_style_set;
}

static void
mux_label_init (MuxLabel *self)
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
mux_label_new (void)
{
  return g_object_new (MUX_TYPE_LABEL, NULL);
}

GtkTextBuffer *
mux_label_get_buffer (MuxLabel *label)
{
  g_return_val_if_fail (MUX_IS_LABEL (label), NULL);

  return gtk_text_view_get_buffer (GTK_TEXT_VIEW (label));
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
mux_label_create_link_tag (MuxLabel *label, const char *url)
{
  GtkTextTag *tag;

  /* TODO: make a subclass of GtkTextTag for cleanliness and so that we don't
     leak the URL */

  g_return_val_if_fail (MUX_IS_LABEL (label), NULL);

  tag = gtk_text_buffer_create_tag (gtk_text_view_get_buffer (GTK_TEXT_VIEW (label)),
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
mux_label_set_text (MuxLabel *label, const char *text)
{
  g_return_if_fail (MUX_IS_LABEL (label));

  gtk_text_buffer_set_text (mux_label_get_buffer (label), text, -1);
}
