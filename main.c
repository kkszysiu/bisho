#include <gtk/gtk.h>
#include <mojito-client/mojito-client.h>

static MojitoClient *client;

static GtkWidget *window, *master_box;

static GKeyFile *
get_keys_for_service (const char *name)
{
  char *filename, *path;
  GKeyFile *keys;

  g_assert (name);

  filename = g_strconcat (name, ".keys", NULL);
  path = g_build_filename ("mojito", "services", filename, NULL);
  g_free (filename);

  keys = g_key_file_new ();

  if (g_key_file_load_from_data_dirs (keys, path, NULL, G_KEY_FILE_NONE, NULL)) {
    g_free (path);
    return keys;
  } else {
    g_free (path);
    g_key_file_free (keys);
    return NULL;
  }
}

static gboolean
on_link_event (GtkTextTag  *tag,
               GObject     *object,
               GdkEvent    *event,
               GtkTextIter *iter,
               gpointer     user_data)
{
  if (event->type == GDK_BUTTON_PRESS && event->button.button == 1) {
    const char *url = user_data;
    g_debug ("clicked %s", url);
    return TRUE;
  }
  return FALSE;
}

static void
construct_ui (const char *service_name)
{
  GKeyFile *keys;
  GtkWidget *box, *label, *text;
  GtkTextBuffer *buffer;
  GtkTextIter end;
  char *s;

  g_assert (service_name);

  keys = get_keys_for_service (service_name);
  if (keys == NULL)
    return;

  box = gtk_vbox_new (FALSE, 4);

  s = g_key_file_get_string (keys, "MojitoService", "Name", NULL);
  if (s) {
    label = gtk_label_new (s);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 0);
    g_free (s);
  }

  text = gtk_text_view_new ();
  gtk_text_view_set_editable (GTK_TEXT_VIEW (text), FALSE);
  gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (text), FALSE);
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text), GTK_WRAP_WORD);
  /* TODO: something like this */
  gtk_widget_modify_base (text, GTK_STATE_NORMAL, &text->style->bg[GTK_STATE_NORMAL]);
  gtk_widget_show (text);
  gtk_box_pack_start (GTK_BOX (box), text, FALSE, FALSE, 0);
  buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (text));

  s = g_key_file_get_string (keys, "MojitoService", "Description", NULL);
  if (s) {
    gtk_text_buffer_get_end_iter (buffer, &end);
    gtk_text_buffer_insert (buffer, &end, s, -1);
    g_free (s);
  }

  s = g_key_file_get_string (keys, "MojitoService", "Link", NULL);
  if (s) {
    GtkTextTag *tag;

    gtk_text_buffer_get_end_iter (buffer, &end);

    tag = gtk_text_buffer_create_tag (buffer, NULL,
                                      "foreground", "blue",
                                      "underline", PANGO_UNDERLINE_SINGLE,
                                      NULL);
    g_signal_connect (tag, "event", G_CALLBACK (on_link_event), s);

    gtk_text_buffer_insert (buffer, &end, "  ", -1);
    gtk_text_buffer_insert_with_tags (buffer, &end,
                                      "Launch site for more information", -1,
                                      tag, NULL);
    gtk_text_buffer_insert (buffer, &end, ".", -1);
  }

  gtk_widget_show (box);
  gtk_container_add (GTK_CONTAINER (master_box), box);
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
  master_box = gtk_vbox_new (TRUE, 4);
  gtk_widget_show (master_box);
  gtk_container_add (GTK_CONTAINER (window), master_box);

  mojito_client_get_services (client, client_get_services_cb, NULL);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
