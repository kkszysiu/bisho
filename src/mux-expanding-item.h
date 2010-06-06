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

#ifndef __MUX_EXPANDING_ITEM_H__
#define __MUX_EXPANDING_ITEM_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MUX_TYPE_EXPANDING_ITEM                                         \
   (mux_expanding_item_get_type())
#define MUX_EXPANDING_ITEM(obj)                                         \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                MUX_TYPE_EXPANDING_ITEM,                \
                                MuxExpandingItem))
#define MUX_EXPANDING_ITEM_CLASS(klass)                                 \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             MUX_TYPE_EXPANDING_ITEM,                   \
                             MuxExpandingItemClass))
#define MUX_IS_EXPANDING_ITEM(obj)                                      \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                MUX_TYPE_EXPANDING_ITEM))
#define MUX_IS_EXPANDING_ITEM_CLASS(klass)                              \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             MUX_TYPE_EXPANDING_ITEM))
#define MUX_EXPANDING_ITEM_GET_CLASS(obj)                               \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               MUX_TYPE_EXPANDING_ITEM,                 \
                               MuxExpandingItemClass))

typedef struct _MuxExpandingItemPrivate MuxExpandingItemPrivate;
typedef struct _MuxExpandingItem      MuxExpandingItem;
typedef struct _MuxExpandingItemClass MuxExpandingItemClass;

struct _MuxExpandingItem {
  GtkFrame parent;
  MuxExpandingItemPrivate *priv;
};

struct _MuxExpandingItemClass {
  GtkFrameClass parent_class;
};

GType mux_expanding_item_get_type (void) G_GNUC_CONST;

GtkWidget *mux_expanding_item_new (void);

void mux_expanding_item_set_label (MuxExpandingItem *item, const char *label);

void mux_expanding_item_set_icon_from_name (MuxExpandingItem *item, const char *icon_name);

void mux_expanding_item_set_icon_from_file (MuxExpandingItem *item, const char *filename);

GtkBox * mux_expanding_item_get_button_box (MuxExpandingItem *item);

GtkBox * mux_expanding_item_get_content_box (MuxExpandingItem *item);

void mux_expanding_item_set_active (MuxExpandingItem *item, gboolean active);

gboolean mux_expanding_item_get_active (MuxExpandingItem *item);

G_END_DECLS

#endif /* __MUX_EXPANDING_ITEM_H__ */
