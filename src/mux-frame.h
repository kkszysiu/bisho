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

#ifndef _MUX_FRAME
#define _MUX_FRAME

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MUX_TYPE_FRAME mux_frame_get_type()

#define MUX_FRAME(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUX_TYPE_FRAME, MuxFrame))

#define MUX_FRAME_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MUX_TYPE_FRAME, MuxFrameClass))

#define MUX_IS_FRAME(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUX_TYPE_FRAME))

#define MUX_IS_FRAME_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), MUX_TYPE_FRAME))

#define MUX_FRAME_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), MUX_TYPE_FRAME, MuxFrameClass))

typedef struct {
    GtkFrame parent;

    GdkColor border_color;
} MuxFrame;

typedef struct {
    GtkFrameClass parent_class;
} MuxFrameClass;

GType mux_frame_get_type (void);

GtkWidget* mux_frame_new (void);

G_END_DECLS

#endif
