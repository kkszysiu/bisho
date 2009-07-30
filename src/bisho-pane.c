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

#include <config.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "bisho-pane.h"

G_DEFINE_ABSTRACT_TYPE (BishoPane, bisho_pane, GTK_TYPE_TABLE);

static void
bisho_pane_class_init (BishoPaneClass *klass)
{
}

static void
bisho_pane_init (BishoPane *self)
{
}

void
bisho_pane_continue_auth (BishoPane *pane, GHashTable *params)
{
  BishoPaneClass *pane_class = BISHO_PANE_GET_CLASS (pane);

  if (pane_class->continue_auth)
    pane_class->continue_auth (pane, params);
}


GtkWidget *
bisho_pane_make_disclaimer_label (ServiceInfo *info)
{
  char *s;
  GtkWidget *label;

  s = g_strdup_printf (_("You'll need an account with %s and an Internet connection to use this web service."),
                       info->display_name);
  label = gtk_label_new (s);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  g_free (s);

  return label;
}

