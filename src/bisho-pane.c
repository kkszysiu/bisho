#include "bisho-pane.h"

G_DEFINE_ABSTRACT_TYPE (BishoPane, bisho_pane, GTK_TYPE_TABLE);

static void
bisho_pane_class_init (BishoPaneClass *klass)
{
}

static void
bisho_pane_init (BishoPane *self)
{
}

void
bisho_pane_continue_auth (BishoPane *pane, GHashTable *params)
{
  BishoPaneClass *pane_class = BISHO_PANE_GET_CLASS (pane);

  if (pane_class->continue_auth)
    pane_class->continue_auth (pane, params);
}

