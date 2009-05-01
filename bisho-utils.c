#include <gtk/gtk.h>
#include <mux/mux-expanding-item.h>
#include "bisho-utils.h"

static GList *expander_list = NULL;

static void
expanded_cb (GObject *object, GParamSpec *param_spec, gpointer user_data)
{
  MuxExpandingItem *just_expanded = MUX_EXPANDING_ITEM (object);
  GList *l;

  if (mux_expanding_item_get_active (just_expanded)) {
    for (l = expander_list; l; l = l->next) {
      MuxExpandingItem *expander = l->data;
      if (expander != just_expanded && mux_expanding_item_get_active (expander))
        mux_expanding_item_set_active (expander, FALSE);
    }
  }
}

void
bisho_utils_make_exclusive_expander (MuxExpandingItem *item)
{
  g_return_if_fail (MUX_IS_EXPANDING_ITEM (item));

  expander_list = g_list_prepend (expander_list, item);

  g_signal_connect (item, "notify::expanded", G_CALLBACK (expanded_cb), NULL);
}
