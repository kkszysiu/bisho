#include <gtk/gtk.h>
#include <mojito-client/mojito-client.h>
#include <mux/mux-expanding-item.h>
#include "bisho-window.h"
#include "bisho-utils.h"
#include "mojito-keyfob-callout-ginterface.h"
#include "service-info.h"
#include "entry.h"
#include "mojito-keyfob-bindings.h"

struct _BishoWindowPrivate {
  MojitoClient *client;
  GtkWidget *master_box;
};

#define GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), BISHO_TYPE_WINDOW, BishoWindowPrivate))

static void callout_iface_init (gpointer g_iface, gpointer iface_data);
G_DEFINE_TYPE_WITH_CODE (BishoWindow, bisho_window, GTK_TYPE_WINDOW,
                         G_IMPLEMENT_INTERFACE (MOJITO_TYPE_KEYFOB_CALLOUT_IFACE, callout_iface_init));

static gboolean
on_link_event (GtkTextTag  *tag,
               GObject     *object,
               GdkEvent    *event,
               GtkTextIter *iter,
               gpointer     user_data)
{
  if (event->type == GDK_BUTTON_PRESS && event->button.button == 1) {
    GtkWidget *widget = GTK_WIDGET (object);
    ServiceInfo *info = user_data;

    gtk_show_uri (gtk_widget_get_screen (widget), info->link,
                  event->button.time, NULL);

    return TRUE;
  }
  return FALSE;
}

/* A small hack to change the colours that the text view renders with. Not
   guaranteed to work but it works with many themes. */
static void
hack_style (GtkWidget *widget, GtkStyle *old_style, gpointer user_data)
{
  g_signal_handlers_block_by_func (widget, hack_style, user_data);
  gtk_widget_modify_base (widget, GTK_STATE_NORMAL, &widget->style->bg[GTK_STATE_NORMAL]);
  g_signal_handlers_unblock_by_func (widget, hack_style, user_data);
}

static void
construct_ui (BishoWindow *window, const char *service_name)
{
  ServiceInfo *info;
  GtkWidget *expander, *label, *text;
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
  mux_expanding_item_set_label (m, info->display_name);

  box = mux_expanding_item_get_content_box (m);
  gtk_container_set_border_width (GTK_CONTAINER (box), 8);
  gtk_box_set_spacing (box, 8);

  text = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (text), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text), GTK_WRAP_WORD);
  g_signal_connect (text, "style-set", G_CALLBACK (hack_style), NULL);
  gtk_widget_show (text);
  gtk_box_pack_start (box, text, FALSE, FALSE, 0);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));

  if (info->description) {
    gtk_text_buffer_get_end_iter (buffer, &end);
    gtk_text_buffer_insert (buffer, &end, info->description, -1);
  }

  if (info->link) {
    GtkTextTag *tag;

    gtk_text_buffer_get_end_iter (buffer, &end);

    tag = gtk_text_buffer_create_tag (buffer, NULL,
                                      "foreground", "blue",
                                      "underline", PANGO_UNDERLINE_SINGLE,
                                      NULL);
    g_signal_connect (tag, "event", G_CALLBACK (on_link_event), info);

    gtk_text_buffer_insert (buffer, &end, "  ", -1);
    gtk_text_buffer_insert_with_tags (buffer, &end,
                                      "Launch site for more information", -1,
                                      tag, NULL);
    gtk_text_buffer_insert (buffer, &end, ".", -1);
  }

  switch (info->auth) {
  case AUTH_USERNAME:
    {
      GtkWidget *table, *entry;

      table = gtk_table_new (1, 2, FALSE);

      label = gtk_label_new ("Username:");
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

      label = gtk_label_new ("Username:");
      gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

      entry = new_entry_from_gconf (info, "user");
      gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 2, 0, 1);

      label = gtk_label_new ("Password:");
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
      GtkWidget *l;
      l = gtk_label_new ("TODO: OAuth");
      gtk_widget_show (l);
      gtk_box_pack_start (GTK_BOX (box), l, FALSE, FALSE, 0);
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
callout_iface_init (gpointer g_iface, gpointer iface_data)
{
  //MojitoKeyfobCalloutIfaceClass *klass = (MojitoKeyfobCalloutIfaceClass*)g_iface;
  //mojito_keyfob_callout_iface_implement_need_authentication (klass, need_authentication);
}

static void
bisho_window_class_init (BishoWindowClass *klass)
{
  g_type_class_add_private (klass, sizeof (BishoWindowPrivate));
}

static void
bisho_window_init (BishoWindow *self)
{
  self->priv = GET_PRIVATE (self);

  gtk_window_set_title (GTK_WINDOW (self), "Web Services Settings");

  self->priv->master_box = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (self->priv->master_box), 8);
  gtk_widget_show (self->priv->master_box);
  gtk_container_add (GTK_CONTAINER (self), self->priv->master_box);

  self->priv->client = mojito_client_new ();
  /* TODO move to a separate populate() function? */
  mojito_client_get_services (self->priv->client, client_get_services_cb, self);
}

GtkWidget *
bisho_window_new (void)
{
  return g_object_new (BISHO_TYPE_WINDOW, NULL);
}
