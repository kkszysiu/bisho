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
 * Whilst GTK+ 2.18 has hyperlink support in GtkLabel it won't wrap like we want
 * it too...
 */

#include <config.h>
#include <string.h>
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

/*
 * This bad boy was taken from https://bugzilla.gnome.org/show_bug.cgi?id=59390
 */
static void
gtk_text_buffer_real_insert_markup (GtkTextBuffer *buffer,
                                    GtkTextIter   *textiter,
                                    const gchar   *markup,
                                    GtkTextTag    *extratag)
{
  PangoAttrIterator  *paiter;
  PangoAttrList      *attrlist;
  GtkTextMark        *mark;
  GError             *error = NULL;
  gchar              *text;

  g_return_if_fail (GTK_IS_TEXT_BUFFER (buffer));
  g_return_if_fail (textiter != NULL);
  g_return_if_fail (markup != NULL);
  g_return_if_fail (gtk_text_iter_get_buffer (textiter) == buffer);

  if (*markup == '\000')
    return;

  if (!pango_parse_markup(markup, -1, 0, &attrlist, &text, NULL, &error))
    {
      g_warning("Invalid markup string: %s", error->message);
      g_error_free(error);
      return;
    }

  if (attrlist == NULL)
    {
      gtk_text_buffer_insert(buffer, textiter, text, -1);
      g_free(text);
      return;
    }

  /* create mark with right gravity */
  mark = gtk_text_buffer_create_mark(buffer, NULL, textiter, FALSE);

  paiter = pango_attr_list_get_iterator(attrlist);

  do
    {
      PangoAttribute *attr;
      GtkTextTag     *tag;
      gint            start, end;

      pango_attr_iterator_range(paiter, &start, &end);

      if (end == G_MAXINT)  /* last chunk */
        end = start-1; /* resulting in -1 to be passed to _insert */

      tag = gtk_text_tag_new(NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_LANGUAGE)))
        g_object_set(tag, "language", pango_language_to_string(((PangoAttrLanguage*)attr)->value), NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_FAMILY)))
        g_object_set(tag, "family", ((PangoAttrString*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_STYLE)))
        g_object_set(tag, "style", ((PangoAttrInt*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_WEIGHT)))
        g_object_set(tag, "weight", ((PangoAttrInt*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_VARIANT)))
        g_object_set(tag, "variant", ((PangoAttrInt*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_STRETCH)))
        g_object_set(tag, "stretch", ((PangoAttrInt*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_SIZE)))
        g_object_set(tag, "size", ((PangoAttrInt*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_FONT_DESC)))
        g_object_set(tag, "font-desc", ((PangoAttrFontDesc*)attr)->desc, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_FOREGROUND)))
        {
          GdkColor col = { 0,
                           ((PangoAttrColor*)attr)->color.red,
                           ((PangoAttrColor*)attr)->color.green,
                           ((PangoAttrColor*)attr)->color.blue
                         };

          g_object_set(tag, "foreground-gdk", &col, NULL);
        }

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_BACKGROUND)))
        {
          GdkColor col = { 0,
                           ((PangoAttrColor*)attr)->color.red,
                           ((PangoAttrColor*)attr)->color.green,
                           ((PangoAttrColor*)attr)->color.blue
                         };

          g_object_set(tag, "background-gdk", &col, NULL);
        }

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_UNDERLINE)))
        g_object_set(tag, "underline", ((PangoAttrInt*)attr)->value, NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_STRIKETHROUGH)))
        g_object_set(tag, "strikethrough", (gboolean)(((PangoAttrInt*)attr)->value != 0), NULL);

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_RISE)))
        g_object_set(tag, "rise", ((PangoAttrInt*)attr)->value, NULL);

      /* PANGO_ATTR_SHAPE cannot be defined via markup text */

      if ((attr = pango_attr_iterator_get(paiter, PANGO_ATTR_SCALE)))
        g_object_set(tag, "scale", ((PangoAttrFloat*)attr)->value, NULL);

      gtk_text_tag_table_add(gtk_text_buffer_get_tag_table(buffer), tag);

      if (extratag)
        {
          gtk_text_buffer_insert_with_tags(buffer, textiter, text+start, end - start, tag, extratag, NULL);
        }
      else
        {
          gtk_text_buffer_insert_with_tags(buffer, textiter, text+start, end - start, tag, NULL);
        }

      /* mark had right gravity, so it should be
       *  at the end of the inserted text now */
      gtk_text_buffer_get_iter_at_mark(buffer, textiter, mark);
    }
  while (pango_attr_iterator_next(paiter));

  gtk_text_buffer_delete_mark(buffer, mark);
  pango_attr_iterator_destroy(paiter);
  pango_attr_list_unref(attrlist);
  g_free(text);
}

void
mux_label_set_markup (MuxLabel *label, const char *markup)
{
  GtkTextBuffer *buffer;
  GtkTextIter start, end;

  g_return_if_fail (MUX_IS_LABEL (label));
  g_return_if_fail (markup != NULL);

  buffer = mux_label_get_buffer (label);

  gtk_text_buffer_get_bounds (buffer, &start, &end);

  gtk_text_buffer_delete (buffer, &start, &end);

  gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
  gtk_text_buffer_real_insert_markup (buffer, &start, markup, NULL);
}
