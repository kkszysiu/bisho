#ifndef __BISHO_PANE_OAUTH_H__
#define __BISHO_PANE_OAUTH_H__

#include "bisho-pane.h"
#include "service-info.h"

G_BEGIN_DECLS

#define BISHO_TYPE_PANE_OAUTH (bisho_pane_oauth_get_type())
#define BISHO_PANE_OAUTH(obj)                                           \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                BISHO_TYPE_PANE_OAUTH,                  \
                                BishoPaneOauth))
#define BISHO_PANE_OAUTH_CLASS(klass)                                   \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             BISHO_TYPE_PANE_OAUTH,                     \
                             BishoPaneOauthClass))
#define BISHO_IS_PANE_OAUTH(obj)                                        \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                BISHO_TYPE_PANE_OAUTH))
#define BISHO_IS_PANE_OAUTH_CLASS(klass)                                \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             BISHO_TYPE_PANE_OAUTH))
#define BISHO_PANE_OAUTH_GET_CLASS(obj)                                 \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               BISHO_TYPE_PANE_OAUTH,                   \
                               BishoPaneOauthClass))

typedef struct _BishoPaneOauthPrivate BishoPaneOauthPrivate;
typedef struct _BishoPaneOauth      BishoPaneOauth;
typedef struct _BishoPaneOauthClass BishoPaneOauthClass;

struct _BishoPaneOauth {
  BishoPane parent;
  BishoPaneOauthPrivate *priv;
};

struct _BishoPaneOauthClass {
  BishoPaneClass parent_class;
};

GType bisho_pane_oauth_get_type (void) G_GNUC_CONST;

GtkWidget * bisho_pane_oauth_new (ServiceInfo *info);

G_END_DECLS

#endif /* __BISHO_PANE_OAUTH_H__ */
