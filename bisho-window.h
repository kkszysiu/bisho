#ifndef __BISHO_WINDOW_H__
#define __BISHO_WINDOW_H__

#include <gtk/gtk.h>
#include <mux/mux-window.h>

G_BEGIN_DECLS

#define BISHO_TYPE_WINDOW                                               \
   (bisho_window_get_type())
#define BISHO_WINDOW(obj)                                               \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                   \
                               BISHO_TYPE_WINDOW,                       \
                               BishoWindow))
#define BISHO_WINDOW_CLASS(klass)                                       \
  (G_TYPE_CHECK_CLASS_CAST ((klass),                                    \
                            BISHO_TYPE_WINDOW,                          \
                            BishoWindowClass))
#define IS_BISHO_WINDOW(obj)                                            \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                   \
                               BISHO_TYPE_WINDOW))
#define IS_BISHO_WINDOW_CLASS(klass)                                    \
  (G_TYPE_CHECK_CLASS_TYPE ((klass),                                    \
                            BISHO_TYPE_WINDOW))
#define BISHO_WINDOW_GET_CLASS(obj)                                     \
  (G_TYPE_INSTANCE_GET_CLASS ((obj),                                    \
                              BISHO_TYPE_WINDOW,                        \
                              BishoWindowClass))

typedef struct _BishoWindowPrivate BishoWindowPrivate;
typedef struct _BishoWindow      BishoWindow;
typedef struct _BishoWindowClass BishoWindowClass;

struct _BishoWindow {
  MuxWindow parent;
  BishoWindowPrivate *priv;
};

struct _BishoWindowClass {
  MuxWindowClass parent_class;
};

GType bisho_window_get_type (void) G_GNUC_CONST;

GtkWidget * bisho_window_new (void);

G_END_DECLS

#endif /* __BISHO_WINDOW_H__ */
