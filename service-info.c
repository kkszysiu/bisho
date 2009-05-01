#include <glib.h>
#include "service-info.h"

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
  } else if (g_ascii_strcasecmp (s, "oauth") == 0) {
    return AUTH_OAUTH;
  } else {
    return AUTH_INVALID;
  }
}

ServiceInfo *
get_info_for_service (const char *name)
{
  char *filename, *path, *real_path, *authstring;
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

  /* TODO: this should be specified in the key file or something */
  path = g_path_get_dirname (real_path);
  g_free (real_path);
  filename = g_strconcat (name, ".png", NULL);
  info->icon = g_build_filename (path, filename, NULL);
  g_free (filename);

  if (!g_file_test (info->icon, G_FILE_TEST_EXISTS)) {
    g_free (info->icon);
    info->icon = NULL;
  }

  return info;
}

