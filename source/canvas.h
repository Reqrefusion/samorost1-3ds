#ifndef SAMOROST_CANVAS_H
#define SAMOROST_CANVAS_H
#include "engine/adapters/sprite_timeline/timeline.h"
#include "scene_loader.h"
#include <3ds.h>
#include <citro3d.h>
bool sm_canvas_init(void);
void sm_canvas_close(void);
bool sm_canvas_load(SmAssets*,const char*,bool);
bool sm_canvas_upload(SmAssets*,SmTexturePixels*,bool);
void sm_canvas_unload(bool);
bool sm_canvas_begin(void);
void sm_canvas_draw(SmVm*,unsigned,SmMatrix,float);
void sm_canvas_rect(float,float,float,float,uint32_t);
void sm_canvas_end(void);
unsigned sm_canvas_vertices(void);
unsigned sm_canvas_draw_calls(void);
#endif