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

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include "mux-expanding-item.h"
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

char *
bisho_utils_encode_tokens (const char *token, const char *secret)
{
  char *encoded_token, *encoded_secret;
  char *string;

  g_assert (token);
  g_assert (secret);

  encoded_token = g_base64_encode ((guchar*)token, strlen (token));
  encoded_secret = g_base64_encode ((guchar*)secret, strlen (secret));

  string = g_strconcat (encoded_token, " ", encoded_secret, NULL);

  g_free (encoded_token);
  g_free (encoded_secret);

  return string;
}
