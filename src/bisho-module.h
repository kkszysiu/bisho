#ifndef __BISHO_MODULE_H__
#define __BISHO_MODULE_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define BISHO_TYPE_MODULE         (bisho_module_get_type ())
#define BISHO_MODULE(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), BISHO_TYPE_MODULE, BishoModule))
#define BISHO_MODULE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), BISHO_TYPE_MODULE, BishoModuleClass))
#define BISHO_IS_MODULE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), BISHO_TYPE_MODULE))
#define BISHO_IS_MODULE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), BISHO_TYPE_MODULE))
#define BISHO_MODULE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), BISHO_TYPE_MODULE, BishoModuleClass))

typedef struct _BishoModule BishoModule;
typedef struct _BishoModuleClass BishoModuleClass;

struct _BishoModule {
  GTypeModule parent_instance;

  char *filename;
  GModule *library;

  void (*load) (BishoModule *module);
};

struct _BishoModuleClass {
  GTypeModuleClass  parent_class;
};

GType bisho_module_get_type (void) G_GNUC_CONST;

BishoModule * bisho_module_new (const gchar *filename);

void bisho_module_load (BishoModule *module);

G_END_DECLS

#endif /* __BISHO_MODULE_H-_ */
