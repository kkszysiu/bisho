#include <gtk/gtk.h>
#include <mojito-client/mojito-client.h>

#include "service-info.h"
#include "entry.h"

static MojitoClient *client;

static GtkWidget *window, *master_box;

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

static GtkWidget *
make_expander_header (ServiceInfo *info)
{
  GtkWidget *box, *image, *label;
  g_assert (info);

  box = gtk_hbox_new (FALSE, 4);

  if (info->icon) {
    image = gtk_image_new_from_file (info->icon);
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);
  }

  label = gtk_label_new (info->display_name);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 0);

  gtk_widget_show_all (box);

  return box;
}

static void
construct_ui (const char *service_name)
{
  ServiceInfo *info;
  GtkWidget *expander, *box, *label, *text;
  GtkTextBuffer *buffer;
  GtkTextIter end;
  char *s;

  g_assert (service_name);

  info= get_info_for_service (service_name);
  if (info == NULL)
    return;

  expander = gtk_expander_new (NULL);
  gtk_expander_set_label_widget (GTK_EXPANDER (expander),
                                 make_expander_header (info));

  box = gtk_vbox_new (FALSE, 4);

  text = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (text), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text), GTK_WRAP_WORD);
  /* TODO: something like this
     gtk_widget_modify_base (text, GTK_STATE_NORMAL, &text->style->bg[GTK_STATE_NORMAL]);
  */
  gtk_widget_show (text);
  gtk_box_pack_start (GTK_BOX (box), text, FALSE, FALSE, 0);
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
  case AUTH_INVALID:
    /* Should never see this, so ignore it */
    break;
  }

  /* TODO: i18n problem here, 'a Advogato account' should be 'an Advogato'. Drop
     this label? */
  s = g_strdup_printf (
                       "<small>"
                       "You'll need a %s account and an Internet connection"
                       "to use this web service."
                       "</small>",
                       info->display_name);
  label = gtk_label_new (NULL);
  gtk_label_set_markup (GTK_LABEL (label), s);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_widget_show (label);
  gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);
  g_free (s);

  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (expander), box);
  gtk_widget_show (expander);
  gtk_container_add (GTK_CONTAINER (master_box), expander);
}

static void
client_get_services_cb (MojitoClient *client,
                        const GList        *services,
                        gpointer      userdata)
{
  const GList *l;

  for (l = services; l; l = l->next) {
    construct_ui (l->data);
  }
}

int
main (int argc, char **argv)
{
  gtk_init (&argc, &argv);

  client = mojito_client_new ();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Web Services Settings");
  g_signal_connect (window, "delete-event", gtk_main_quit, NULL);

  master_box = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (master_box), 8);
  gtk_widget_show (master_box);
  gtk_container_add (GTK_CONTAINER (window), master_box);

  mojito_client_get_services (client, client_get_services_cb, NULL);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
