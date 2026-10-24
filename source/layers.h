#ifndef SAMOROST_LAYERS_H
#define SAMOROST_LAYERS_H
#include "engine/adapters/sprite_timeline/timeline.h"
typedef enum {SM_LAYER_NONE,SM_LAYER_PLANET,SM_LAYER_DISTANT_PLANET,SM_LAYER_ROCKET,SM_LAYER_BIRD,SM_LAYER_PORTRAIT,SM_LAYER_EFFECTS,SM_LAYER_ACTOR,SM_LAYER_CONTACT,SM_LAYER_SCENERY,SM_LAYER_SWARM,SM_LAYER_MACHINE,SM_LAYER_LIFT,SM_LAYER_LIGHT,SM_LAYER_OVERLAY} SmLayerGroup;
typedef struct {float plane,relief;unsigned group;} SmLayerState;
#ifdef __cplusplus
extern "C" {
#endif
SmLayerState sm_layer_resolve(const SmVm*,int,SmLayerState);
SmLayerState sm_layer_state(const SmVm*,int);
float sm_layer_relief_visibility(SmMatrix,unsigned,unsigned);
#ifdef __cplusplus
}
#endif
#endif
