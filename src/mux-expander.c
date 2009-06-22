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

#include "mux-expander.h"

#define DEFAULT_SIZE 20

struct _MuxExpanderPrivate {
  int size;
  GtkExpanderStyle state;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MUX_TYPE_EXPANDER, MuxExpanderPrivate))
G_DEFINE_TYPE (MuxExpander, mux_expander, GTK_TYPE_WIDGET);

static void
mux_expander_style_set (GtkWidget *widget, GtkStyle *previous_style)
{
  MuxExpander *expander = MUX_EXPANDER (widget);

  GTK_WIDGET_CLASS (mux_expander_parent_class)->style_set (widget, previous_style);

  gtk_widget_style_get (widget,
			"expander-size", &expander->priv->size,
			NULL);

  gtk_widget_queue_resize (widget);
}

static void
mux_expander_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  MuxExpander *expander = MUX_EXPANDER (widget);

  requisition->width = requisition->height = expander->priv->size;
}

static gboolean
mux_expander_expose_event (GtkWidget *widget, GdkEventExpose *event)
{
  MuxExpander *expander = MUX_EXPANDER (widget);

  gtk_paint_expander (widget->style, widget->window,
                      GTK_WIDGET_STATE (widget),
                      &event->area,
                      widget,
                      NULL,
                      widget->allocation.x + widget->allocation.width / 2,
                      widget->allocation.y + widget->allocation.height / 2,
                      expander->priv->state);

  return FALSE;
}

static void
mux_expander_class_init (MuxExpanderClass *klass)
{
    GtkWidgetClass *w_class = (GtkWidgetClass *)klass;

    w_class->size_request = mux_expander_size_request;
    w_class->style_set = mux_expander_style_set;
    w_class->expose_event = mux_expander_expose_event;

    g_type_class_add_private (klass, sizeof (MuxExpanderPrivate));

    gtk_widget_class_install_style_property (w_class, g_param_spec_int
                                             ("expander-size", "Expander Size", "Size of the expander arrow",
                                              0, G_MAXINT, DEFAULT_SIZE,
                                              G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
mux_expander_init (MuxExpander *expander)
{
  expander->priv = GET_PRIVATE (expander);

  GTK_WIDGET_SET_FLAGS (expander, GTK_NO_WINDOW);

  expander->priv->size = DEFAULT_SIZE;
  expander->priv->state = GTK_EXPANDER_COLLAPSED;
}

GtkWidget *
mux_expander_new (void)
{
  return g_object_new (MUX_TYPE_EXPANDER, NULL);
}

void
mux_expander_set_state (MuxExpander *expander, GtkExpanderStyle state)
{
  expander->priv->state = state;

  gtk_widget_queue_draw (GTK_WIDGET (expander));
}

GtkExpanderStyle
mux_expander_get_state (MuxExpander *expander)
{
  return expander->priv->state;
}
