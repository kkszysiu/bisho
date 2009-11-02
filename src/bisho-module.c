#include <config.h>
#include <gmodule.h>
#include "bisho-module.h"

G_DEFINE_TYPE (BishoModule, bisho_module, G_TYPE_TYPE_MODULE);

static gboolean
bisho_module_load_module (GTypeModule *gmodule)
{
  BishoModule *module = BISHO_MODULE (gmodule);

  if (!module->filename) {
    g_warning ("Module path is NULL");
    return FALSE;
  }

  module->library = g_module_open (module->filename, 0);

  if (!module->library) {
    g_printerr ("Cannot load module: %s\n", g_module_error ());
    return FALSE;
  }

  /* Sanity check the module */
  if (!g_module_symbol (module->library, "bisho_module_load", (gpointer *) &module->load)) {
    g_printerr ("Module entrypoint missing: %s\n", g_module_error ());
    g_module_close (module->library);

    return FALSE;
  }

  module->load (module);

  return TRUE;
}

static void
bisho_module_unload_module (GTypeModule *gmodule)
{
}

static void
bisho_module_finalize (GObject *object)
{
  BishoModule *module = BISHO_MODULE (object);

  g_free (module->filename);

  G_OBJECT_CLASS (bisho_module_parent_class)->finalize (object);
}

static void
bisho_module_class_init (BishoModuleClass *class)
{
  GObjectClass *o_class = G_OBJECT_CLASS (class);
  GTypeModuleClass *type_class = G_TYPE_MODULE_CLASS (class);

  o_class->finalize = bisho_module_finalize;

  type_class->load = bisho_module_load_module;
  type_class->unload = bisho_module_unload_module;
}

static void
bisho_module_init (BishoModule *module)
{
  module->filename = NULL;
  module->library = NULL;
  module->load = NULL;
}

BishoModule *
bisho_module_new (const gchar *filename)
{
  BishoModule *module;

  g_return_val_if_fail (filename, NULL);

  module = g_object_new (BISHO_TYPE_MODULE, NULL);
  module->filename = g_strdup (filename);

  return module;
}
