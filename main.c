#include <gtk/gtk.h>
#include <gconf/gconf-client.h>
#include <mojito-client/mojito-client.h>

static MojitoClient *client;

static GtkWidget *window, *master_box;

static GConfClient *gconf;

typedef enum {
  AUTH_INVALID = 0,
  AUTH_USERNAME,
  AUTH_USERNAME_PASSWORD
} ServiceAuthType;

typedef struct {
  char *name;
  char *display_name;
  char *description;
  char *link;
  ServiceAuthType auth;
  GdkPixbuf *icon;
} ServiceInfo;

#define GROUP "MojitoService"

static ServiceAuthType
authtype_from_string (const char *s)
{
  if (s == NULL) {
    return AUTH_INVALID;
  } else if (g_ascii_strcasecmp (s, "username") == 0) {
    return AUTH_USERNAME;
  } else if (g_ascii_strcasecmp (s, "password") == 0) {
    return AUTH_USERNAME_PASSWORD;
  } else {
    return AUTH_INVALID;
  }
}

static ServiceInfo *
get_info_for_service (const char *name)
{
  char *filename, *path, *real_path, *image_path, *authstring;
  GKeyFile *keys;
  ServiceInfo *info;
  ServiceAuthType auth;

  g_assert (name);

  filename = g_strconcat (name, ".keys", NULL);
  path = g_build_filename ("mojito", "services", filename, NULL);
  g_free (filename);

  keys = g_key_file_new ();

  if (!g_key_file_load_from_data_dirs (keys, path, &real_path, G_KEY_FILE_NONE, NULL)) {
    g_free (path);
    g_key_file_free (keys);
    return NULL;
  }
  g_free (path);

  /* Sanity check for required keys */
  if (!g_key_file_has_key (keys, GROUP, "Name", NULL) ||
      !g_key_file_has_key (keys, GROUP, "AuthType", NULL)) {
    g_key_file_free (keys);
    return NULL;
  }

  authstring = g_key_file_get_string (keys, GROUP, "AuthType", NULL);
  auth = authtype_from_string (authstring);
  g_free (authstring);

  if (auth == AUTH_INVALID) {
    g_key_file_free (keys);
    return NULL;
  }

  info = g_slice_new0 (ServiceInfo);
  info->name = g_strdup (name);
  info->display_name = g_key_file_get_string (keys, GROUP, "Name", NULL);
  info->description = g_key_file_get_string (keys, GROUP, "Description", NULL);
  info->link = g_key_file_get_string (keys, GROUP, "Link", NULL);
  info->auth = auth;

  g_key_file_free (keys);

  path = g_path_get_dirname (real_path);
  g_free (real_path);
  filename = g_strconcat (name, ".png", NULL);
  image_path = g_build_filename (path, filename, NULL);
  g_free (filename);

  info->icon = gdk_pixbuf_new_from_file (image_path, NULL);
  g_free (image_path);

  return info;
}

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
    image = gtk_image_new_from_pixbuf (info->icon);
    gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);
  }

  label = gtk_label_new (info->display_name);
  gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
  gtk_box_pack_start (GTK_BOX (box), label, TRUE, TRUE, 0);

  gtk_widget_show_all (box);

  return box;
}

static void
set_gconf_key (ServiceInfo *info, const char *key_suffix, const char *value)
{
  char *key;

  key = g_strdup_printf ("/apps/mojito/services/%s/%s",
                         info->name, key_suffix);

  /* TODO: block gconf notify when we have one */
  gconf_client_set_string (gconf, key, value, NULL);

  g_free (key);
}

static gboolean
on_user_entry_left (GtkWidget *widget, GdkEventFocus *event, gpointer user_data)
{
  ServiceInfo *info = user_data;

  set_gconf_key (info, "user", gtk_entry_get_text (GTK_ENTRY (widget)));

  return FALSE;
}

static char *
get_gconf_key (ServiceInfo *info, const char *key_suffix)
{
  char *key, *value;
  GError *error = NULL;

  key = g_strdup_printf ("/apps/mojito/services/%s/%s",
                         info->name, key_suffix);

  value = gconf_client_get_string (gconf, key, &error);
  if (error) {
    g_message ("Cannot get key %s: %s", key, error->message);
    g_error_free (error);
  }

  g_free (key);

  return value;
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

      table = gtk_table_new (2, 2, FALSE);

      label = gtk_label_new ("Username:");
      gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);

      entry = gtk_entry_new ();
      gtk_entry_set_text (GTK_ENTRY (entry), get_gconf_key (info, "user"));
      /* TODO: connect to gconf notify */
      g_signal_connect (entry, "focus-out-event", G_CALLBACK (on_user_entry_left), info);
      gtk_table_attach_defaults (GTK_TABLE (table), entry, 1, 2, 0, 1);

      gtk_widget_show_all (table);
      gtk_box_pack_start (GTK_BOX (box), table, FALSE, FALSE, 0);
    }
    break;
  case AUTH_USERNAME_PASSWORD:
    {
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

  gconf = gconf_client_get_default ();

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  master_box = gtk_vbox_new (FALSE, 8);
  gtk_container_set_border_width (GTK_CONTAINER (master_box), 8);
  gtk_widget_show (master_box);
  gtk_container_add (GTK_CONTAINER (window), master_box);

  mojito_client_get_services (client, client_get_services_cb, NULL);

  gtk_widget_show (window);

  gtk_main ();

  return 0;
}
