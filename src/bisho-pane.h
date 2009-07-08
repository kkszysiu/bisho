#ifndef __BISHO_PANE_H__
#define __BISHO_PANE_H__

#include <gtk/gtk.h>
#include "service-info.h"

G_BEGIN_DECLS

#define BISHO_TYPE_PANE (bisho_pane_get_type())
#define BISHO_PANE(obj)                                                 \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BISHO_TYPE_PANE, BishoPane))
#define BISHO_PANE_CLASS(klass)                                         \
  (G_TYPE_CHECK_CLASS_CAST ((klass), BISHO_TYPE_PANE, BishoPaneClass))
#define IS_BISHO_PANE(obj)                              \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BISHO_TYPE_PANE))
#define IS_BISHO_PANE_CLASS(klass)                      \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), BISHO_TYPE_PANE))
#define BISHO_PANE_GET_CLASS(obj)                                       \
   (G_TYPE_INSTANCE_GET_CLASS ((obj), BISHO_TYPE_PANE, BishoPaneClass))

typedef struct _BishoPane BishoPane;
typedef struct _BishoPaneClass BishoPaneClass;

struct _BishoPane {
  GtkTable parent;
  ServiceInfo *info;
};

struct _BishoPaneClass {
  GtkTableClass parent_class;
  void (*continue_auth) (BishoPane *pane, GHashTable *params);
};

GType bisho_pane_get_type (void) G_GNUC_CONST;

void bisho_pane_continue_auth (BishoPane *pane, GHashTable *params);

G_END_DECLS

#endif /* __BISHO_PANE_H__ */
