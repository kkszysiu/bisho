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

#ifndef __MUX_EXPANDER_H__
#define __MUX_EXPANDER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MUX_TYPE_EXPANDER                                               \
   (mux_expander_get_type())
#define MUX_EXPANDER(obj)                                               \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                MUX_TYPE_EXPANDER,                      \
                                MuxExpander))
#define MUX_EXPANDER_CLASS(klass)                                       \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             MUX_TYPE_EXPANDER,                         \
                             MuxExpanderClass))
#define MUX_IS_EXPANDER(obj)                                            \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                MUX_TYPE_EXPANDER))
#define MUX_IS_EXPANDER_CLASS(klass)                                    \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             MUX_TYPE_EXPANDER))
#define MUX_EXPANDER_GET_CLASS(obj)                                     \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               MUX_TYPE_EXPANDER,                       \
                               MuxExpanderClass))

typedef struct _MuxExpanderPrivate MuxExpanderPrivate;
typedef struct _MuxExpander      MuxExpander;
typedef struct _MuxExpanderClass MuxExpanderClass;

struct _MuxExpander
{
    GtkWidget parent;
    MuxExpanderPrivate *priv;
};

struct _MuxExpanderClass
{
    GtkWidgetClass parent_class;
};

GType mux_expander_get_type (void) G_GNUC_CONST;

GtkWidget *mux_expander_new (void);

void mux_expander_set_state (MuxExpander *expander, GtkExpanderStyle state);

GtkExpanderStyle mux_expander_get_state (MuxExpander *expander);

G_END_DECLS

#endif /* __MUX_EXPANDER_H__ */
