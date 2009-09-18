#ifndef __MUX_BANNER_H__
#define __MUX_BANNER_H__

#include <nbtk/nbtk-gtk.h>

G_BEGIN_DECLS

#define MUX_TYPE_BANNER (mux_banner_get_type())
#define MUX_BANNER(obj)                                                 \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), MUX_TYPE_BANNER, MuxBanner))
#define MUX_BANNER_CLASS(klass)                                         \
  (G_TYPE_CHECK_CLASS_CAST ((klass), MUX_TYPE_BANNER, MuxBannerClass))
#define IS_MUX_BANNER(obj)                                              \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), MUX_TYPE_BANNER))
#define IS_MUX_BANNER_CLASS(klass)                                      \
   (G_TYPE_CHECK_CLASS_TYPE ((klass), MUX_TYPE_BANNER))
#define MUX_BANNER_GET_CLASS(obj)                                       \
   (G_TYPE_INSTANCE_GET_CLASS ((obj), MUX_TYPE_BANNER, MuxBannerClass))

typedef struct _MuxBannerPrivate MuxBannerPrivate;
typedef struct _MuxBanner      MuxBanner;
typedef struct _MuxBannerClass MuxBannerClass;

struct _MuxBanner {
  NbtkGtkFrame parent;
  MuxBannerPrivate *priv;
};

struct _MuxBannerClass {
  NbtkGtkFrameClass parent_class;
};

GType mux_banner_get_type (void) G_GNUC_CONST;

GtkWidget * mux_banner_new (void);

void mux_banner_set_text (MuxBanner *banner, const char *text);

G_END_DECLS

#endif /* __MUX_BANNER_H__ */
