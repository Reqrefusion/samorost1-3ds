#include "layers.h"
#include <math.h>
#include <string.h>
#include "layer_profiles.inc"
static float clamp(float x,float low,float high){return x<low?low:x>high?high:x;}
static float scale(SmMatrix m){return sqrtf((m.a*m.a+m.b*m.b+m.c*m.c+m.d*m.d)*0.5f);}
static SmLayerState group(unsigned id,float plane,float relief){SmLayerState s={plane,relief,id};return s;}
static float planet_plane(const SmVm *v,int parent){
 if(parent>=0&&parent<SM_NODES){const SmNode *p=v->nodes+parent;
  for(unsigned i=0;i<p->children_count;i++){const SmNode *n=v->nodes+p->children[i];if(n->alive&&n->kind==1&&n->id==610)return clamp(-0.20f+0.35f*log2f(fmaxf(0.05f,scale(n->transform))),-0.55f,0.25f);}
 }
 return -0.20f;
}
static float blend(float a,float b,float t){return t<=0?a:t>=1?b:a+(b-a)*t;}
static float ease(float t){t=clamp(t,0,1);return t*t*(3-2*t);}
static const SmNode *sibling(const SmVm *v,const SmNode *n,unsigned id){
 if(n->parent<0)return NULL;const SmNode *p=v->nodes+n->parent;
 for(unsigned i=0;i<p->children_count;i++){const SmNode *child=v->nodes+p->children[i];if(child->alive&&child->kind==1&&child->id==id)return child;}
 return NULL;
}
static float bird_plane(const SmVm *v,const SmNode *n){
 const SmNode *flight=n->children_count?v->nodes+n->children[0]:NULL;
 if(!flight)return 0.10f;
 if(n->frame==2)return -0.10f+0.20f*ease((float)flight->frame/246);
 if(n->frame==4)return 0.10f+0.55f*ease(((float)flight->frame-1)/147);
 return 0.10f;
}
static bool scene_root(const char *level,unsigned id){
 for(unsigned i=0;i<sizeof scene_layers/sizeof scene_layers[0];i++)if(scene_layers[i].root==id&&!strcmp(scene_layers[i].level,level))return true;
 return false;
}
static SmLayerState profile(const SmVm *v,const SmNode *n,SmLayerState inherited){
 const char *level=n->assets->name;
 for(unsigned s=0;s<sizeof scene_layers/sizeof scene_layers[0];s++){
  if(strcmp(scene_layers[s].level,level))continue;
  for(unsigned i=scene_layers[s].first;i<scene_layers[s].first+scene_layers[s].count;i++){
   if(layer_rules[i].kind>=0&&(unsigned)layer_rules[i].kind!=n->kind)continue;
   if(layer_rules[i].name[0]){if(strcmp(layer_rules[i].name,n->name))continue;}
   else if(layer_rules[i].id!=n->id)continue;
   if(layer_rules[i].root_only&&(n->parent<0||!scene_root(level,v->nodes[n->parent].id)))continue;
   return layer_rules[i].state;
  }
  if(n->kind==1&&scene_root(level,n->id)){inherited.plane=scene_layers[s].plane;inherited.relief=scene_layers[s].relief;return inherited;}
  if(n->parent>=0&&scene_root(level,v->nodes[n->parent].id))return group(SM_LAYER_SCENERY,inherited.plane,inherited.relief);
  break;
 }
 return inherited;
}
SmLayerState sm_layer_resolve(const SmVm *v,int id,SmLayerState inherited){
 if(id<0||id>=SM_NODES||!v->nodes[id].alive)return inherited;
 /* A body, its flame/wing poses and its passenger share one anchor. Nested
    replacement or insertion never creates another disparity plane. */
 const SmNode *n=v->nodes+id;
 if(n->kind==3)return group(SM_LAYER_OVERLAY,0,0);
 if(inherited.group!=SM_LAYER_NONE)return inherited;
 if(!n->assets)return inherited;
 if(!strcmp(n->assets->name,"Start")){
  if(n->kind==1&&n->id==615){
   float s=fmaxf(0.03f,scale(n->transform));
   return group(SM_LAYER_ROCKET,clamp(-0.35f+0.35f*log2f(s/0.25f),-0.45f,0.80f),0);
  }
  if(n->kind==1&&n->id==610)return group(SM_LAYER_PLANET,planet_plane(v,n->parent),0.5f);
  if(n->kind==1&&n->id==611)return group(SM_LAYER_DISTANT_PLANET,-0.62f,1);
  if(n->kind==1&&n->id==617)return group(SM_LAYER_PORTRAIT,0.22f,0);
  if(n->kind==1&&n->id==619)return group(SM_LAYER_DISTANT_PLANET,-0.65f,0);
  if(n->kind==1&&n->id==618)return group(SM_LAYER_EFFECTS,0.12f,0);
  if(n->kind==0){
   if(n->id==1394||n->id==1397||n->id==1404||n->id==1405)return group(SM_LAYER_PLANET,planet_plane(v,n->parent),0);
   if((n->id>=1310&&n->id<=1336))return group(SM_LAYER_EFFECTS,0.12f,0);
   if(n->id==1339||n->id==1341)return group(SM_LAYER_PORTRAIT,0.22f,0);
  }
 }
 if(!strcmp(n->assets->name,"Rybar")&&n->kind==1&&n->id==782){inherited.plane=-0.40f;inherited.relief=0.5f;return inherited;}
 if(!strcmp(n->assets->name,"Rybar")&&n->kind==1){
  if(n->id==776)return group(SM_LAYER_BIRD,bird_plane(v,n),0);
  const SmNode *bird=sibling(v,n,776);
  if(bird&&n->id==781&&n->frame==4&&bird->frame==4){
   const SmNode *jump=n->children_count?v->nodes+n->children[0]:NULL;
   float t=!n->visible?1:jump?ease(((float)jump->frame-10)/25):0;
   return group(SM_LAYER_ACTOR,blend(0.10f,bird_plane(v,bird),t),0);
  }
  if(bird&&n->id==690&&bird->frame>=2){
   const SmNode *flight=bird->children_count?v->nodes+bird->children[0]:NULL;
   float t=bird->frame>2?1:flight?ease(((float)flight->frame-200)/46):0;
   return group(SM_LAYER_ACTOR,blend(-0.30f,bird_plane(v,bird),t),0);
  }
 }
 /* Authored image fragments are often one connected photographic plate.
    Unprofiled branches stay coplanar; display-list indices are not distance. */
 return profile(v,n,inherited);
}
SmLayerState sm_layer_state(const SmVm *v,int id){
 int chain[64];unsigned size=0;while(id>=0&&id<SM_NODES&&v->nodes[id].alive&&size<64){chain[size++]=id;id=v->nodes[id].parent;}
 SmLayerState state={0,1,SM_LAYER_NONE};while(size)state=sm_layer_resolve(v,chain[--size],state);return state;
}
float sm_layer_relief_visibility(SmMatrix m,unsigned width,unsigned height){
 float projected=fmaxf(width*sqrtf(m.a*m.a+m.b*m.b),height*sqrtf(m.c*m.c+m.d*m.d));
 return clamp((projected-24)/96,0,1);
}
