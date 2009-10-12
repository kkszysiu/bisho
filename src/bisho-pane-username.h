#ifndef __BISHO_PANE_USERNAME_H__
#define __BISHO_PANE_USERNAME_H__

#include <bisho-pane.h>


G_BEGIN_DECLS

#define BISHO_TYPE_PANE_USERNAME (bisho_pane_username_get_type())
#define BISHO_PANE_USERNAME(obj)                                        \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                BISHO_TYPE_PANE_USERNAME,               \
                                BishoPaneUsername))
#define BISHO_PANE_USERNAME_CLASS(klass)                                \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             BISHO_TYPE_PANE_USERNAME,                  \
                             BishoPaneUsernameClass))
#define BISHO_IS_PANE_USERNAME(obj)                                     \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                BISHO_TYPE_PANE_USERNAME))
#define BISHO_IS_PANE_USERNAME_CLASS(klass)                             \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             BISHO_TYPE_PANE_USERNAME))
#define BISHO_PANE_USERNAME_GET_CLASS(obj)                              \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               BISHO_TYPE_PANE_USERNAME,                \
                               BishoPaneUsernameClass))

typedef struct _BishoPaneUsernamePrivate BishoPaneUsernamePrivate;
typedef struct _BishoPaneUsername      BishoPaneUsername;
typedef struct _BishoPaneUsernameClass BishoPaneUsernameClass;

struct _BishoPaneUsername {
  BishoPane parent;
  BishoPaneUsernamePrivate *priv;
};

struct _BishoPaneUsernameClass {
  BishoPaneClass parent_class;
};

GType bisho_pane_username_get_type (void) G_GNUC_CONST;

GtkWidget *bisho_pane_username_new (ServiceInfo *info);

void bisho_pane_username_add_entry (BishoPaneUsername *pane, const char *label, const char *key);

G_END_DECLS

#endif /* __BISHO_PANE_USERNAME_H__ */
