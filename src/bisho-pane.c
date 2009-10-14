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
#include "mux-label.h"

#if ! HAVE_DECL_GTK_INFO_BAR_NEW
#include "gtkinfobar.h"
#endif

G_DEFINE_ABSTRACT_TYPE (BishoPane, bisho_pane, GTK_TYPE_VBOX);

enum {
  PROP_0,
  PROP_SERVICE,
  PROP_MOJITO
};

static void
bisho_pane_get_property (GObject *object, guint property_id,
                         GValue *value, GParamSpec *pspec)
{
  BishoPane *pane = BISHO_PANE (object);

  switch (property_id) {
  case PROP_SERVICE:
    g_value_set_pointer (value, pane->info);
    break;
  case PROP_MOJITO:
    g_value_set_object (value, pane->mojito);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
bisho_pane_set_property (GObject *object, guint property_id,
                         const GValue *value, GParamSpec *pspec)
{
  BishoPane *pane = BISHO_PANE (object);

  switch (property_id) {
  case PROP_SERVICE:
    {
      GtkTextBuffer *buffer;
      GtkTextIter end;
      char *s;

      pane->info = g_value_get_pointer (value);
      buffer = mux_label_get_buffer (MUX_LABEL (pane->description));

      if (pane->info->description) {
        gtk_text_buffer_get_end_iter (buffer, &end);
        gtk_text_buffer_insert (buffer, &end, pane->info->description, -1);
      }

      if (pane->info->link) {
        GtkTextTag *tag;

        gtk_text_buffer_get_end_iter (buffer, &end);

        tag = mux_label_create_link_tag (MUX_LABEL (pane->description), pane->info->link);

        gtk_text_buffer_insert (buffer, &end, "  ", -1);
        gtk_text_buffer_insert_with_tags (buffer, &end,
                                          _("Launch site for more information."), -1,
                                          tag, NULL);
      }

      s = g_strdup_printf (_("<small>You'll need an account with %s and an Internet connection to use this web service.</small>"),
                           pane->info->display_name);
      mux_label_set_markup (MUX_LABEL (pane->disclaimer), s);
      g_free (s);
    }
    break;
  case PROP_MOJITO:
    pane->mojito = g_value_dup_object (value);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
  }
}

static void
bisho_pane_class_init (BishoPaneClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);
    GParamSpec *pspec;

    object_class->get_property = bisho_pane_get_property;
    object_class->set_property = bisho_pane_set_property;

    pspec = g_param_spec_pointer ("service", "service", "service",
                                  G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_SERVICE, pspec);

    pspec = g_param_spec_object ("mojito", "mojito", "mojito",
                                 MOJITO_TYPE_CLIENT,
                                 G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY);
    g_object_class_install_property (object_class, PROP_MOJITO, pspec);
}

static void
bisho_pane_init (BishoPane *pane)
{
  GtkWidget *align, *banner_content;

  gtk_box_set_spacing (GTK_BOX (pane), 8);

  pane->description = mux_label_new ();
  gtk_widget_show (pane->description);
  gtk_box_pack_start (GTK_BOX (pane), pane->description, FALSE, FALSE, 0);

  align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_widget_show (align);
  pane->banner = gtk_info_bar_new ();
  gtk_container_add (GTK_CONTAINER (align), pane->banner);
  gtk_box_pack_start (GTK_BOX (pane), align, FALSE, FALSE, 0);

  pane->banner_label = gtk_label_new (NULL);
  gtk_label_set_line_wrap (GTK_LABEL (pane->banner_label), TRUE);
  gtk_widget_show (pane->banner_label);
  banner_content = gtk_info_bar_get_content_area (GTK_INFO_BAR (pane->banner));
  gtk_container_add (GTK_CONTAINER (banner_content), pane->banner_label);

  align = gtk_alignment_new (0.5, 0.5, 0.0, 0.0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (align), 0, 0, 32, 0);
  gtk_widget_show (align);
  pane->user_box = gtk_hbox_new (FALSE, 0);
  pane->user_icon = gtk_image_new ();
  gtk_widget_show (pane->user_icon);
  gtk_box_pack_start (GTK_BOX (pane->user_box), pane->user_icon, FALSE, FALSE, 8);
  pane->user_name = gtk_label_new (NULL);
  gtk_box_pack_start (GTK_BOX (pane->user_box), pane->user_name, TRUE, TRUE, 8);
  gtk_container_add (GTK_CONTAINER (align), pane->user_box);
  gtk_box_pack_start (GTK_BOX (pane), align, FALSE, FALSE, 0);

  pane->content = gtk_alignment_new (0.0, 0.0, 0.0, 0.0);
  gtk_alignment_set_padding (GTK_ALIGNMENT (pane->content), 0, 0, 64, 64);
  gtk_widget_show (pane->content);
  gtk_box_pack_start (GTK_BOX (pane), pane->content, TRUE, TRUE, 0);

  pane->disclaimer = mux_label_new ();
  gtk_widget_show (pane->disclaimer);
  gtk_box_pack_start (GTK_BOX (pane), pane->disclaimer, FALSE, TRUE, 0);
}

void
bisho_pane_continue_auth (BishoPane *pane, GHashTable *params)
{
  BishoPaneClass *pane_class = BISHO_PANE_GET_CLASS (pane);

  if (pane_class->continue_auth)
    pane_class->continue_auth (pane, params);
}

void
bisho_pane_set_banner (BishoPane *pane, const char *message)
{
  if (message) {
    gtk_info_bar_set_message_type (GTK_INFO_BAR (pane->banner), GTK_MESSAGE_INFO);
    gtk_label_set_text (GTK_LABEL (pane->banner_label), message);
    gtk_widget_show (pane->banner);
  } else {
    gtk_widget_hide (pane->banner);
  }
}

void
bisho_pane_set_banner_error (BishoPane *pane, const GError *error)
{
  char *s;

  if (error) {
    s = g_strdup_printf (_("Sorry, we can't log in to %s. %s"),
                         pane->info->display_name,
                         error->message);
  } else {
    s = g_strdup_printf (_("Sorry, we can't log in to %s"),
                         pane->info->display_name);
  }

  gtk_info_bar_set_message_type (GTK_INFO_BAR (pane->banner), GTK_MESSAGE_WARNING);
  gtk_label_set_text (GTK_LABEL (pane->banner_label), s);
  gtk_widget_show (pane->banner);

  g_free (s);
}

void
bisho_pane_set_user (BishoPane *pane, const char *icon, const char *username)
{
  if (icon == NULL && username == NULL) {
    gtk_widget_hide (pane->user_box);
    return;
  }

  gtk_widget_show (pane->user_box);

  if (icon) {
    gtk_image_set_from_file (GTK_IMAGE (pane->user_icon), icon);
  } else {
    gtk_image_set_from_icon_name (GTK_IMAGE (pane->user_icon), "stock_person", GTK_ICON_SIZE_DIALOG);
  }

  if (username) {
    gtk_label_set_text (GTK_LABEL (pane->user_name), username);
    gtk_widget_show (pane->user_name);
  } else {
    gtk_widget_hide (pane->user_name);
  }
}

static void
on_online_changed (MojitoClient *client, gboolean online, gpointer user_data)
{
  GtkWidget *widget = GTK_WIDGET (user_data);

  gtk_widget_set_sensitive (widget, online);
}

void
bisho_pane_follow_connected (BishoPane *pane, GtkWidget *widget)
{
  g_return_if_fail (BISHO_IS_PANE (pane));
  g_return_if_fail (GTK_IS_WIDGET (widget));

  g_signal_connect (pane->mojito, "online-changed", G_CALLBACK (on_online_changed), widget);

  mojito_client_is_online (pane->mojito, on_online_changed, widget);
}
