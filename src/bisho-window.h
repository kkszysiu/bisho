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

#ifndef __BISHO_WINDOW_H__
#define __BISHO_WINDOW_H__

#include <gtk/gtk.h>
#include <libsocialweb-client/sw-client.h>
#include "service-info.h"

G_BEGIN_DECLS

#define BISHO_TYPE_WINDOW                                               \
   (bisho_window_get_type())
#define BISHO_WINDOW(obj)                                               \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                   \
                               BISHO_TYPE_WINDOW,                       \
                               BishoWindow))
#define BISHO_WINDOW_CLASS(klass)                                       \
  (G_TYPE_CHECK_CLASS_CAST ((klass),                                    \
                            BISHO_TYPE_WINDOW,                          \
                            BishoWindowClass))
#define BISHO_IS_WINDOW(obj)                                            \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                   \
                               BISHO_TYPE_WINDOW))
#define BISHO_IS_WINDOW_CLASS(klass)                                    \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                                    \
                            BISHO_TYPE_WINDOW))
#define BISHO_WINDOW_GET_CLASS(obj)                                     \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                                    \
                              BISHO_TYPE_WINDOW,                        \
                              BishoWindowClass))

typedef struct _BishoWindowPrivate BishoWindowPrivate;
typedef struct _BishoWindow      BishoWindow;
typedef struct _BishoWindowClass BishoWindowClass;

struct _BishoWindow {
  GtkWindow parent;
  BishoWindowPrivate *priv;
};

struct _BishoWindowClass {
  GtkWindowClass parent_class;
};

GType bisho_window_get_type (void) G_GNUC_CONST;

GtkWidget * bisho_window_new (void);

void bisho_window_callback (BishoWindow *window, const char *id, GHashTable *params);

SwClient * bisho_window_get_socialweb (BishoWindow *window);

G_END_DECLS

#endif /* __BISHO_WINDOW_H__ */
