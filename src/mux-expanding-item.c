/*
* libmux - GTK+ Moblin User Experience widgets
 * Copyright (C) 2009 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <gtk/gtk.h>
#include "mux-expanding-item.h"
#include "mux-expander.h"

enum {
  PROP_0,
  PROP_EXPANDED
};

struct _MuxExpandingItemPrivate {
  gboolean active;
  GtkWidget *arrow;
  GtkWidget *icon;
  GtkWidget *label;
  GtkWidget *button_box;
  GtkWidget *content_box;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MUX_TYPE_EXPANDING_ITEM, MuxExpandingItemPrivate))
G_DEFINE_TYPE (MuxExpandingItem, mux_expanding_item, GTK_TYPE_FRAME);

static void
mux_expanding_item_set_property (GObject      *object,
			   guint         prop_id,
			   const GValue *value,
			   GParamSpec   *pspec)
{
  MuxExpandingItem *item = MUX_EXPANDING_ITEM (object);

  switch (prop_id) {
    case PROP_EXPANDED:
      mux_expanding_item_set_active (item, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
mux_expanding_item_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
  MuxExpandingItem *item = MUX_EXPANDING_ITEM (object);

  switch (prop_id) {
    case PROP_EXPANDED:
      g_value_set_boolean (value, item->priv->active);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
mux_expanding_item_class_init (MuxExpandingItemClass *klass)
{
  GObjectClass *o_class = (GObjectClass*)klass;

  g_type_class_add_private (klass, sizeof (MuxExpandingItemPrivate));

  o_class->set_property = mux_expanding_item_set_property;
  o_class->get_property = mux_expanding_item_get_property;

  g_object_class_install_property (o_class,
				   PROP_EXPANDED,
				   g_param_spec_boolean ("expanded", "Expanded", NULL,
							 FALSE,
							 G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_CONSTRUCT));
}

static gboolean
header_pressed (GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
  /* TODO: set pressed flag and then check in button-release etc etc */

  if (event->type == GDK_BUTTON_PRESS && event->button == 1) {
    MuxExpandingItem *item = MUX_EXPANDING_ITEM (user_data);

    mux_expanding_item_set_active (item, !item->priv->active);
  }

  return FALSE;
}

static void
mux_expanding_item_init (MuxExpandingItem *self)
{
  MuxExpandingItemPrivate *priv = GET_PRIVATE (self);
  GtkWidget *box, *label_box, *event_box;
  PangoAttrList *attrs;

  self->priv = priv;

  gtk_container_set_border_width (GTK_CONTAINER (self), 4);

  box = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (self), box);

  event_box = gtk_event_box_new ();
  gtk_event_box_set_visible_window (GTK_EVENT_BOX (event_box), FALSE);
  gtk_widget_add_events (event_box, GDK_BUTTON_PRESS_MASK);
  g_signal_connect (event_box, "button-press-event", G_CALLBACK (header_pressed), self);
  gtk_box_pack_start (GTK_BOX (box), event_box, FALSE, FALSE, 0);

  label_box = gtk_hbox_new (FALSE, 4);
  gtk_container_set_border_width (GTK_CONTAINER (label_box), 4);
  gtk_container_add (GTK_CONTAINER (event_box), label_box);

  priv->arrow = mux_expander_new ();
  gtk_box_pack_start (GTK_BOX (label_box), priv->arrow, FALSE, FALSE, 0);

  priv->icon = gtk_image_new ();
  gtk_box_pack_start (GTK_BOX (label_box), priv->icon, FALSE, FALSE, 0);

  priv->label = gtk_label_new (NULL);
  attrs = pango_attr_list_new ();
  pango_attr_list_insert (attrs, pango_attr_weight_new (PANGO_WEIGHT_BOLD));
  pango_attr_list_insert (attrs, pango_attr_scale_new (PANGO_SCALE_LARGE));
  gtk_label_set_attributes (GTK_LABEL (priv->label), attrs);
  gtk_misc_set_alignment (GTK_MISC (priv->label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (label_box), priv->label, TRUE, TRUE, 0);

  priv->button_box = gtk_hbox_new (FALSE, 0);
  gtk_widget_set_no_show_all (priv->button_box, TRUE);
  gtk_box_pack_end (GTK_BOX (label_box), priv->button_box, FALSE, FALSE, 0);

  priv->content_box = gtk_vbox_new (FALSE, 0);
  gtk_widget_set_no_show_all (priv->content_box, TRUE);
  gtk_box_pack_start (GTK_BOX (box), priv->content_box, FALSE, FALSE, 0);
}

GtkWidget *
mux_expanding_item_new (void)
{
  return g_object_new (MUX_TYPE_EXPANDING_ITEM, NULL);
}

void
mux_expanding_item_set_label (MuxExpandingItem *item, const char *label)
{
  MuxExpandingItemPrivate *priv;

  priv = GET_PRIVATE (item);

  gtk_label_set_text (GTK_LABEL (priv->label), label);
}

void
mux_expanding_item_set_icon_from_name (MuxExpandingItem *item, const char *icon_name)
{
  MuxExpandingItemPrivate *priv;

  priv = GET_PRIVATE (item);

  gtk_image_set_from_icon_name (GTK_IMAGE (priv->icon), icon_name, GTK_ICON_SIZE_LARGE_TOOLBAR);
}

void
mux_expanding_item_set_icon_from_file (MuxExpandingItem *item, const char *filename)
{
  MuxExpandingItemPrivate *priv;

  priv = GET_PRIVATE (item);

  gtk_image_set_from_file (GTK_IMAGE (priv->icon), filename);
}

GtkBox *
mux_expanding_item_get_button_box (MuxExpandingItem *item)
{
  return GTK_BOX (GET_PRIVATE (item)->button_box);
}

GtkBox *
mux_expanding_item_get_content_box (MuxExpandingItem *item)
{
  return GTK_BOX (GET_PRIVATE (item)->content_box);
}

void
mux_expanding_item_set_active (MuxExpandingItem *item, gboolean active)
{
  MuxExpandingItemPrivate *priv;

  priv = GET_PRIVATE (item);

  if (priv->active == active)
    return;

  priv->active = active;

  if (active) {
    mux_expander_set_state (MUX_EXPANDER (priv->arrow), GTK_EXPANDER_EXPANDED);
    gtk_widget_show (priv->button_box);
    gtk_widget_show (priv->content_box);
  } else {
    mux_expander_set_state (MUX_EXPANDER (priv->arrow), GTK_EXPANDER_COLLAPSED);
    gtk_widget_hide (priv->button_box);
    gtk_widget_hide (priv->content_box);
  }

  g_object_notify (G_OBJECT (item), "expanded");
}

gboolean
mux_expanding_item_get_active (MuxExpandingItem *item)
{
  return GET_PRIVATE (item)->active;
}

