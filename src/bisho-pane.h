#ifndef __BISHO_PANE_H__
#define __BISHO_PANE_H__

#include <gtk/gtk.h>
#include "service-info.h"
#include <mojito-client/mojito-client.h>

G_BEGIN_DECLS

#define BISHO_TYPE_PANE (bisho_pane_get_type())
#define BISHO_PANE(obj)                                                 \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), BISHO_TYPE_PANE, BishoPane))
#define BISHO_PANE_CLASS(klass)                                         \
  (G_TYPE_CHECK_CLASS_CAST ((klass), BISHO_TYPE_PANE, BishoPaneClass))
#define BISHO_IS_PANE(obj)                              \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), BISHO_TYPE_PANE))
#define BISHO_IS_PANE_CLASS(klass)                      \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), BISHO_TYPE_PANE))
#define BISHO_PANE_GET_CLASS(obj)                                       \
   (G_TYPE_INSTANCE_GET_CLASS ((obj), BISHO_TYPE_PANE, BishoPaneClass))

typedef struct _BishoPane BishoPane;
typedef struct _BishoPaneClass BishoPaneClass;

struct _BishoPane {
  GtkVBox parent;
  MojitoClient *mojito;
  ServiceInfo *info;
  GtkWidget *description;
  GtkWidget *banner;
  GtkWidget *user_box;
  GtkWidget *user_icon;
  GtkWidget *user_name;
  GtkWidget *content;
  GtkWidget *disclaimer;
};

struct _BishoPaneClass {
  GtkVBoxClass parent_class;
  void (*continue_auth) (BishoPane *pane, GHashTable *params);
};

GType bisho_pane_get_type (void) G_GNUC_CONST;

void bisho_pane_continue_auth (BishoPane *pane, GHashTable *params);

void bisho_pane_set_banner (BishoPane *pane, const char *message);

void bisho_pane_set_banner_error (BishoPane *pane, const GError *error);

void bisho_pane_set_user (BishoPane *pane, const char *icon, const char *username);

void bisho_pane_follow_connected (BishoPane *pane, GtkWidget *widget);

G_END_DECLS

#endif /* __BISHO_PANE_H__ */
