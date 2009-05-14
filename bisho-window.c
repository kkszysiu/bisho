#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <mojito-client/mojito-client.h>
#include <mux/mux-expanding-item.h>
#include <mux/mux-banner.h>
#include <mux/mux-window.h>
#include "bisho-window.h"
#include "bisho-utils.h"
#include "service-info.h"
#include "entry.h"
#include "bisho-oauth-pane.h"

struct _BishoWindowPrivate {
  MojitoClient *client;
  GtkWidget *master_box;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_WINDOW, BishoWindowPrivate))

G_DEFINE_TYPE (BishoWindow, bisho_window, MUX_TYPE_WINDOW);

static void
construct_ui (BishoWindow *window, const char *service_name)
{
  ServiceInfo *info;
  GtkWidget *expander, *label, *banner;
  GtkBox *box;
  MuxExpandingItem *m;
  GtkTextBuffer *buffer;
  GtkTextIter end;

  g_assert (window);
  g_assert (service_name);

  info = get_info_for_service (service_name);
  if (info == NULL)
    return;

  expander = mux_expanding_item_new ();
  m = MUX_EXPANDING_ITEM (expander);

  bisho_utils_make_exclusive_expander (m);
  mux_expanding_item_set_icon_from_file (m, info->icon);
  /* TODO: only set the label if the icon doesn't have the name in already. This
     will require a new field in the keys and ServiceInfo */
#if 0
  mux_expanding_item_set_label (m, info->display_name);
#endif

  box = mux_expanding_item_get_content_box (m);
  gtk_container_set_border_width (GTK_CONTAINER (box), 8);
  gtk_box_set_spacing (box, 8);

  banner = mux_banner_new ();
  gtk_widget_show (banner);
  gtk_box_pack_start (box, banner, FALSE, FALSE, 0);
  buffer = mux_banner_get_buffer (MUX_BANNER (banner));

  if (info->description) {
    gtk_text_buffer_get_end_iter (buffer, &end);
    gtk_text_buffer_insert (buffer, &end, info->description, -1);
  }

  if (info->link) {
    GtkTextTag *tag;

    gtk_text_buffer_get_end_iter (buffer, &end);

    tag = mux_banner_create_link_tag (MUX_BANNER (banner), info->link);

    gtk_text_buffer_insert (buffer, &end, "  ", -1);
    gtk_text_buffer_insert_with_tags (buffer, &end,
                                      _("Launch site for more information"), -1,
                                      tag, NULL);
    gtk_text_buffer_insert (buffer, &end, ".", -1);
  }

  switch (info->auth) {
  case AUTH_USERNAME:
    {
      GtkWidget *table, *entry;

      table = gtk_table_new (1, 2, FALSE);

      label = gtk_label_new (_("Username:"));
      gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

      entry = new_entry_from_gconf (info, "user");
      gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 2, 0, 1);

      gtk_widget_show_all (table);
      gtk_box_pack_start (GTK_BOX (box), table, FALSE, FALSE, 0);
    }
    break;
  case AUTH_USERNAME_PASSWORD:
    {
      GtkWidget *table, *entry;

      table = gtk_table_new (2, 2, FALSE);

      label = gtk_label_new (_("Username:"));
      gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

      entry = new_entry_from_gconf (info, "user");
      gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 2, 0, 1);

      label = gtk_label_new (_("Password:"));
      gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 0, 0);

      entry = new_entry_from_gconf (info, "password");
      gtk_entry_set_visibility (GTK_ENTRY (entry), FALSE);
      gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 2, 1, 2);

      gtk_widget_show_all (table);
      gtk_box_pack_start (GTK_BOX (box), table, FALSE, FALSE, 0);
    }
    break;
  case AUTH_OAUTH:
    {
      GtkWidget *pane;
      pane = bisho_oauth_pane_new (info);
      gtk_widget_show (pane);
      gtk_box_pack_start (GTK_BOX (box), pane, FALSE, FALSE, 0);
    }
    break;
  case AUTH_INVALID:
    /* Should never see this, so ignore it */
    break;
  }

  gtk_widget_show_all (expander);
  gtk_box_pack_start (GTK_BOX (window->priv->master_box), expander, FALSE, FALSE, 0);
}

static void
client_get_services_cb (MojitoClient *client,
                        const GList        *services,
                        gpointer      userdata)
{
  BishoWindow *window = BISHO_WINDOW (userdata);
  const GList *l;

  for (l = services; l; l = l->next) {
    construct_ui (window, l->data);
  }
}

static void
bisho_window_class_init (BishoWindowClass *klass)
{
  g_type_class_add_private (klass, sizeof (BishoWindowPrivate));
}

static void
bisho_window_init (BishoWindow *self)
{
  GtkWidget *label;

  self->priv = GET_PRIVATE (self);

  gtk_window_set_title (GTK_WINDOW (self), _("Web Services Settings"));

  self->priv->master_box = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (self->priv->master_box), 8);
  gtk_widget_show (self->priv->master_box);
  gtk_container_add (GTK_CONTAINER (self), self->priv->master_box);

  label = gtk_label_new (_("Set up your social networks to see new updates in m_zone and the status panel."));
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (self->priv->master_box), label, FALSE, FALSE, 0);

  self->priv->client = mojito_client_new ();
  /* TODO move to a separate populate() function? */
  mojito_client_get_services (self->priv->client, client_get_services_cb, self);
}

GtkWidget *
bisho_window_new (void)
{
  return g_object_new (BISHO_TYPE_WINDOW, NULL);
}
