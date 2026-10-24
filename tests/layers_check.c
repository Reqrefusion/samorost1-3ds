#include "layers.h"
#include "stereo.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
static unsigned rocket_samples,bird_samples,checked_nodes,checked_anchors;
static float rocket_low=10,rocket_high=-10,bird_low=10,bird_high=-10;
static void descendants(const SmVm *v,int id,SmLayerState anchor){
 const SmNode *n=v->nodes+id;SmLayerState current=sm_layer_state(v,id);
 assert(current.group==anchor.group&&fabsf(current.plane-anchor.plane)<0.00001f&&current.relief==anchor.relief);
 for(unsigned i=0;i<n->children_count;i++)descendants(v,n->children[i],anchor);
}
void sm_test_layers(SmVm *v){
 for(int i=0;i<SM_NODES;i++)if(v->nodes[i].alive){
  SmNode *n=v->nodes+i;SmLayerState current=sm_layer_state(v,i);checked_nodes++;
  assert(isfinite(current.plane)&&current.plane>=-0.65f&&current.plane<=0.80f&&current.relief>=0&&current.relief<=1);
  if(n->parent>=0){SmLayerState parent=sm_layer_state(v,n->parent);
   if(parent.group!=SM_LAYER_NONE&&n->kind!=3)assert(current.group==parent.group&&current.plane==parent.plane&&current.relief==parent.relief);
   if(v->nodes[n->parent].parent==v->root){assert(current.group!=SM_LAYER_NONE);checked_anchors++;}
  }
  if(n->kind==1&&n->assets&&((!strcmp(n->assets->name,"Start")&&n->id==615)||(!strcmp(n->assets->name,"Rybar")&&n->id==776&&n->children_count))){
   SmLayerState anchor=sm_layer_state(v,i);assert(anchor.relief==0&&anchor.plane>=-0.55f&&anchor.plane<=0.8f);descendants(v,i,anchor);
   unsigned original_count=0;int parent=n->parent;if(parent>=0){original_count=v->nodes[parent].children_count;v->nodes[parent].children_count=SM_CHILDREN;}
   SmLayerState unaffected=sm_layer_state(v,i);if(parent>=0)v->nodes[parent].children_count=original_count;
   assert(anchor.plane==unaffected.plane&&anchor.group==unaffected.group);
   if(anchor.group==SM_LAYER_ROCKET){rocket_samples++;if(anchor.plane<rocket_low)rocket_low=anchor.plane;if(anchor.plane>rocket_high)rocket_high=anchor.plane;}
   else {bird_samples++;if(anchor.plane<bird_low)bird_low=anchor.plane;if(anchor.plane>bird_high)bird_high=anchor.plane;}
  }
 }
}
static void pair(const SmVm *v,const char *a,const char *b){
 int x=sm_find((SmVm*)v,a),y=sm_find((SmVm*)v,b);if(x<0||y<0)return;
 SmLayerState sa=sm_layer_state(v,x),sb=sm_layer_state(v,y);assert(sa.plane==sb.plane);
}
void sm_test_contacts(SmVm *v){
 if(!strcmp(v->scene->name,"Lyzar")){pair(v,"Kurak","Tabak");pair(v,"Kurak","Kour");}
 if(!strcmp(v->scene->name,"Rybar")){
  int bird=sm_find(v,"Sup"),hero=sm_find(v,"Lyzar"),bone=sm_find(v,"Kostricka");
  if(bird>=0&&hero>=0&&v->nodes[bird].frame==4){int jump=sm_child(v,hero,0);if(!v->nodes[hero].visible||(jump>=0&&v->nodes[hero].frame==4&&v->nodes[jump].frame>=35))pair(v,"Sup","Lyzar");}
  if(bird>=0&&bone>=0&&v->nodes[bird].frame==3)pair(v,"Sup","Kostricka");
 }
 if(!strcmp(v->scene->name,"Vcely")){pair(v,"Strom","Sosak");pair(v,"Strom","KrytkaSosakStrom");pair(v,"Gondola","Skret");}
 if(!strcmp(v->scene->name,"Strom")){pair(v,"Gondola","Vetev");pair(v,"Gondola","Lano");pair(v,"Gondola","SovaPozice");pair(v,"Gramec","Veverka");pair(v,"Veverka","Dutina");}
 if(!strcmp(v->scene->name,"Vodarna")){pair(v,"Mravenec","Skret");pair(v,"Rozvadec","Knob1");}
 if(!strcmp(v->scene->name,"Strojovna")){pair(v,"Kolo","KoloStin");pair(v,"Rafika1","BtnPlus");pair(v,"Rafika2","BtnMinus");}
}
void sm_test_layers_finish(void){
 printf("PASS all-scene inheritance: %u node samples, %u assigned root branches; contact assemblies stay aligned\n",checked_nodes,checked_anchors);
 assert(checked_nodes>100000&&checked_anchors>10000);
 assert(rocket_samples>100&&bird_samples>100&&rocket_high-rocket_low>0.2f&&bird_high-bird_low>0.2f);
 SmMatrix tiny=sm_identity();tiny.a=tiny.d=0.001f;assert(sm_layer_relief_visibility(tiny,1555,1081)==0);
 printf("PASS connected flight groups: rocket %u samples %.2f..%.2f, bird %u samples %.2f..%.2f; child-count invariant and tiny-relief suppression\n",rocket_samples,rocket_low,rocket_high,bird_samples,bird_low,bird_high);
}
