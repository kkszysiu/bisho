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

#ifndef __BISHO_UTILS_H__
#define __BISHO_UTILS_H__

#include <gtk/gtk.h>
#include "mux-expanding-item.h"

G_BEGIN_DECLS

void bisho_utils_make_exclusive_expander (MuxExpandingItem *item);

char * bisho_utils_encode_tokens (const char *token, const char *secret);

G_END_DECLS

#endif /* __BISHO_UTILS_H__ */
