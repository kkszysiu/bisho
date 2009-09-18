#include "mux-banner.h"

struct _MuxBannerPrivate {
  GtkWidget *label;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), MUX_TYPE_BANNER, MuxBannerPrivate))
G_DEFINE_TYPE (MuxBanner, mux_banner, NBTK_GTK_TYPE_FRAME);

static void
mux_banner_class_init (MuxBannerClass *klass)
{
  g_type_class_add_private (klass, sizeof (MuxBannerPrivate));
}

static void
mux_banner_init (MuxBanner *self)
{
  self->priv = GET_PRIVATE (self);

  gtk_container_set_border_width (GTK_CONTAINER (self), 10);

  self->priv->label = gtk_label_new (NULL);
  gtk_widget_show (self->priv->label);
  gtk_container_add (GTK_CONTAINER (self), self->priv->label);
}

GtkWidget *
mux_banner_new (void)
{
  return g_object_new (MUX_TYPE_BANNER,
                       "border-width", 4,
                       NULL);
}

void
mux_banner_set_text (MuxBanner *banner, const char *text)
{
  gtk_label_set_text (GTK_LABEL (banner->priv->label), text);
}
