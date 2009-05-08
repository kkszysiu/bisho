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
