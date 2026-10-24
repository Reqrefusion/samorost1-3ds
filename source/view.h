#ifndef SAMOROST_VIEW_H
#define SAMOROST_VIEW_H
#define LUME_VIEWPORT_TOP 0
#define LUME_VIEWPORT_BOTTOM 240
#include "engine/render/viewport.h"
#include "game.h"

/* Apply the shared Lume bottom viewport after the authored scene fit.
   The stereo overview never receives this crop. */
static inline SmMatrix sm_bottom_view_matrix(const LumeViewport *view) {
  float zoom = view->scale / 1024.0f;
  SmMatrix crop = sm_identity();
  crop.a = crop.d = zoom;
  crop.x = 160.0f - (view->cx / 1024.0f) * zoom;
  crop.y = 120.0f - (view->cy / 1024.0f) * zoom;
  return sm_multiply(crop, sm_game_camera(320, 1, 0, 0));
}
#endif
