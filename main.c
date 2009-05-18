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

#include <gtk/gtk.h>
#include "bisho-window.h"

int
main (int argc, char **argv)
{
  GtkWidget *window;

  g_thread_init (NULL);

  gtk_init (&argc, &argv);

  window = bisho_window_new ();

  g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
