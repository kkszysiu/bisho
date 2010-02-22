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

#include <config.h>
#include <glib/gi18n-lib.h>
#include <gtk/gtk.h>
#include "bisho-cc-page.h"
#include "bisho-frame.h"

struct _BishoCcPagePrivate {
  int dummy;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_CC_PAGE, BishoCcPagePrivate))

G_DEFINE_TYPE (BishoCcPage, bisho_cc_page, CC_TYPE_PAGE);

static void
bisho_cc_page_class_init (BishoCcPageClass *klass)
{
  g_type_class_add_private (klass, sizeof (BishoCcPagePrivate));
}

static void
bisho_cc_page_init (BishoCcPage *self)
{
  GtkWidget *frame;

  self->priv = GET_PRIVATE (self);

  /* TODO: do this on first activate, because it involves disk IO */
  frame = bisho_frame_new ();
  gtk_widget_show (frame);
  gtk_container_add (GTK_CONTAINER (self), frame);
}

CcPage *
bisho_cc_page_new (void)
{
  return g_object_new (BISHO_TYPE_CC_PAGE,
                       "display-name", _("My Web Accounts"),
                       "id", "webservices",
                       NULL);
}
