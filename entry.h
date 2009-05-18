#include <gtk/gtk.h>
#include "service-info.h"
#include "bisho-window.h"

GtkWidget * new_entry_from_gconf (BishoWindow *window, ServiceInfo *info, const char *key);
