#ifndef SAMOROST_SCENE_LOADER_H
#define SAMOROST_SCENE_LOADER_H
#include "engine/adapters/sprite_timeline/timeline.h"
typedef struct {unsigned width,height,size,mip_levels;uint8_t *data;} SmTexturePixels;
typedef struct {
 SmAssets assets;SmTexturePixels pixels[SM_TEXTURES];
 uint64_t assets_ms,textures_ms;bool builtin_relief;
} SmSceneData;
bool sm_texture_pixels_load(SmAssets*,const char*,SmTexturePixels*);
void sm_texture_pixels_free(SmTexturePixels*);
bool sm_scene_prepare(SmSceneData*,const char*,const char*);
void sm_scene_dispose(SmSceneData*);
#ifdef __3DS__
void sm_scene_loader_init(const char*);
bool sm_scene_loader_start(const char*);
/* 0 loading, 1 ready, -1 failed. No partial data is exposed. */
int sm_scene_loader_poll(void);
SmSceneData *sm_scene_loader_take(void);
void sm_scene_loader_release(void);
void sm_scene_loader_shutdown(void);
#endif
#endif
