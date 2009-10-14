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

#ifndef __MUX_LABEL_H__
#define __MUX_LABEL_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MUX_TYPE_LABEL                                                 \
   (mux_label_get_type())
#define MUX_LABEL(obj)                                                 \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                MUX_TYPE_LABEL,                        \
                                MuxLabel))
#define MUX_LABEL_CLASS(klass)                                         \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             MUX_TYPE_LABEL,                           \
                             MuxLabelClass))
#define MUX_IS_LABEL(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                MUX_TYPE_LABEL))
#define MUX_IS_LABEL_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             MUX_TYPE_LABEL))
#define MUX_LABEL_GET_CLASS(obj)                                       \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               MUX_TYPE_LABEL,                         \
                               MuxLabelClass))

typedef struct _MuxLabel      MuxLabel;
typedef struct _MuxLabelClass MuxLabelClass;

struct _MuxLabel {
  GtkTextView parent;
};

struct _MuxLabelClass {
  GtkTextViewClass parent_class;
};

GType mux_label_get_type (void) G_GNUC_CONST;

GtkWidget *mux_label_new (void);

GtkTextBuffer * mux_label_get_buffer (MuxLabel *label);

GtkTextTag * mux_label_create_link_tag (MuxLabel *label, const char *url);

void mux_label_set_text (MuxLabel *label, const char *text);

void mux_label_set_markup (MuxLabel *label, const char *markup);

G_END_DECLS

#endif /* __MUX_LABEL_H__ */
