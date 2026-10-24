#ifndef SAMOROST_DEPTH_H
#define SAMOROST_DEPTH_H
#include "engine/adapters/sprite_timeline/timeline.h"
#define SM_DEPTH_GRID 9
#ifdef __cplusplus
extern "C" {
#endif
bool sm_depth_load(SmAssets*,const char*);
bool sm_depth_builtin(SmAssets*);
#ifdef __cplusplus
}
#endif
#endif
