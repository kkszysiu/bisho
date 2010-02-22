/* -*- Mode: C; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2010 Intel Corp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */

#ifndef __BISHO_CC_PAGE_H__
#define __BISHO_CC_PAGE_H__

#include <gtk/gtk.h>
#include <libgnome-control-center-extension/cc-page.h>

G_BEGIN_DECLS

#define BISHO_TYPE_CC_PAGE         (bisho_cc_page_get_type ())
#define BISHO_CC_PAGE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), BISHO_TYPE_CC_PAGE, BishoCcPage))
#define BISHO_CC_PAGE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), BISHO_TYPE_CC_PAGE, BishoCcPageClass))
#define BISHO_IS_CC_PAGE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), BISHO_TYPE_CC_PAGE))
#define BISHO_IS_CC_PAGE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), BISHO_TYPE_CC_PAGE))
#define BISHO_CC_PAGE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), BISHO_TYPE_CC_PAGE, BishoCcPageClass))

typedef struct _BishoCcPagePrivate BishoCcPagePrivate;

typedef struct
{
        CcPage              parent;
        BishoCcPagePrivate *priv;
} BishoCcPage;

typedef struct
{
        CcPageClass parent_class;
} BishoCcPageClass;

GType bisho_cc_page_get_type (void);
CcPage * bisho_cc_page_new (void);

G_END_DECLS

#endif /* __BISHO_CC_PAGE_H__ */
