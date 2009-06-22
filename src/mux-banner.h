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

#ifndef __MUX_BANNER_H__
#define __MUX_BANNER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define MUX_TYPE_BANNER                                                 \
   (mux_banner_get_type())
#define MUX_BANNER(obj)                                                 \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                MUX_TYPE_BANNER,                        \
                                MuxBanner))
#define MUX_BANNER_CLASS(klass)                                         \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             MUX_TYPE_BANNER,                           \
                             MuxBannerClass))
#define MUX_IS_BANNER(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                MUX_TYPE_BANNER))
#define MUX_IS_BANNER_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             MUX_TYPE_BANNER))
#define MUX_BANNER_GET_CLASS(obj)                                       \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               MUX_TYPE_BANNER,                         \
                               MuxBannerClass))

typedef struct _MuxBanner      MuxBanner;
typedef struct _MuxBannerClass MuxBannerClass;

struct _MuxBanner {
  GtkTextView parent;
};

struct _MuxBannerClass {
  GtkTextViewClass parent_class;
};

GType mux_banner_get_type (void) G_GNUC_CONST;

GtkWidget *mux_banner_new (void);

GtkTextBuffer * mux_banner_get_buffer (MuxBanner *banner);

GtkTextTag * mux_banner_create_link_tag (MuxBanner *banner, const char *url);

void mux_banner_set_text (MuxBanner *banner, const char *text);

G_END_DECLS

#endif /* __MUX_BANNER_H__ */
