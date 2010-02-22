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
#include <unique/unique.h>
#include <libsoup/soup.h>
#include "bisho-window.h"

enum {
  COMMAND_CALLBACK = 1
};

static void
handle_uri (BishoWindow *window, const char *s)
{
  SoupURI *uri = NULL;
  GHashTable *params = NULL;

  uri = soup_uri_new (s);
  if (strcmp (uri->scheme, "x-bisho") != 0) {
    soup_uri_free (uri);
    return;
  }

  if (uri->query)
    params = soup_form_decode (uri->query);
  else
    params = g_hash_table_new (NULL, NULL);

  bisho_window_callback (window, uri->path, params);

  g_hash_table_destroy (params);
  soup_uri_free (uri);
}

static UniqueResponse
unique_message_cb (UniqueApp *app,
                   UniqueCommand  command,
                   UniqueMessageData *message,
                   guint time_, gpointer user_data)
{
  GtkWindow *window = GTK_WINDOW (user_data);
  char **uris;

  switch (command) {
  case UNIQUE_ACTIVATE:
    gtk_window_set_screen (window, unique_message_data_get_screen (message));
    gtk_window_present_with_time (window, time_);
    break;
  case COMMAND_CALLBACK:
    gtk_window_set_screen (window, unique_message_data_get_screen (message));
    gtk_window_present_with_time (window, time_);

    uris = unique_message_data_get_uris (message);
    if (uris)
      handle_uri (BISHO_WINDOW (window), uris[0]);
    g_strfreev (uris);
    break;
  default:
    break;
  }

  return UNIQUE_RESPONSE_OK;
}

int
main (int argc, char **argv)
{
  UniqueApp *app;
  GtkWidget *window;

  g_thread_init (NULL);

  bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  gtk_init (&argc, &argv);

  /* TODO: use GOption to parse arguments */

  app = unique_app_new_with_commands ("com.intel.Bisho", NULL,
                                      "callback", COMMAND_CALLBACK,
                                      NULL);

  if (unique_app_is_running (app)) {
    UniqueResponse response;

    if (argc != 2) {
      response = unique_app_send_message (app, UNIQUE_ACTIVATE, NULL);
    } else {
      UniqueMessageData *msg;
      msg = unique_message_data_new ();
      unique_message_data_set_uris (msg, argv + 1);
      response = unique_app_send_message (app, COMMAND_CALLBACK, msg);
      unique_message_data_free (msg);
    }

    if (response == UNIQUE_RESPONSE_OK)
      goto done;
  }

  window = bisho_window_new ();

  unique_app_watch_window (app, GTK_WINDOW (window));

  g_signal_connect (app, "message-received", G_CALLBACK (unique_message_cb), window);

  g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

  gtk_widget_show (window);

  gtk_main ();

 done:
  g_object_unref (app);

  return 0;
}
