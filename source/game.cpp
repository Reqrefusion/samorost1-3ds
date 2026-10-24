#include "game.h"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <initializer_list>
#include <string>

namespace {
using String=std::string;
constexpr unsigned H(const char *s,unsigned h=2166136261u){return *s?H(s+1,(h^unsigned(*s))*16777619u):h;}
unsigned hash(const String &s){return H(s.c_str());}
struct Sensor {bool alive=false,enabled=true;int parent=-1;unsigned generation=0;String name;float x=0,y=0,w=0,h=0,rotation=0;};
static SmVm *vm;
static Sensor sensors[128];
static SmMusic music;
static unsigned level;
static int requested=-1;
static float transition_delay,clock_seconds,credits_deadline,entry_deadline;
static float space_phase,space_speed,space_from,space_target,space_elapsed,space_duration;
void space_motion(float target,float duration){space_from=space_speed;space_target=target;space_elapsed=0;space_duration=duration;if(duration<=0)space_speed=target;}
static bool bWasMoving,zoomEnabled;
static int captured_sensor=-1;static unsigned captured_generation;
static float _focusY=0,_focusZoom=0;
static constexpr float gameWidth=1900,gameHeight=1500;

class Movie {
public:
 int id=-1;unsigned generation=0;
 Movie()=default;
 Movie(int value):id(value),generation(value>=0&&value<SM_NODES?vm->nodes[value].generation:0){}
 explicit operator bool() const{return id>=SM_NODES?id<SM_NODES+128&&sensors[id-SM_NODES].alive:id>=0&&vm&&vm->nodes[id].alive&&vm->nodes[id].generation==generation;}
 SmNode *node()const{return id<SM_NODES&&bool(*this)?vm->nodes+id:nullptr;}
 unsigned currentFrame()const{return node()?node()->frame:0;}
 bool isPlaying()const{return node()&&node()->playing;}
 Movie mParent()const{return id>=SM_NODES&&bool(*this)?Movie(sensors[id-SM_NODES].parent):node()?Movie(node()->parent):Movie();}
 String mName()const{return id>=SM_NODES&&bool(*this)?sensors[id-SM_NODES].name:node()?node()->name:"";}
 String name()const{return mName();}
 Movie getChildAt(unsigned i)const{return node()?Movie(sm_child(vm,id,i)):Movie();}
 Movie getChildByName(const String &s)const{return node()?Movie(sm_find_child(vm,id,s.c_str())):Movie();}
 unsigned indexOf(Movie child)const{if(node())for(unsigned i=0;i<node()->children_count;i++)if(node()->children[i]==child.id)return i;return 0;}
 void addChildAt(Movie child,unsigned index)const{if(node()&&child.node())sm_attach(vm,child.id,id,index);}
 void gotoAndStop(unsigned f)const{if(node())sm_seek(vm,id,f,false);}
 void gotoAndPlay(unsigned f)const{if(node())sm_seek(vm,id,f,true);}
 void gotoAndPlay(const char *label)const{if(node())sm_seek_label(vm,id,label,true);}
 void play()const{if(node())sm_play(vm,id);}
 void stop()const{if(node())sm_stop(vm,id);}
 void nextFrame()const{if(node())sm_next(vm,id);}
 bool visible()const{return node()&&node()->visible;}
 bool visible(bool value)const{if(node())node()->visible=value;return value;}
 bool enabled()const{return id>=SM_NODES&&bool(*this)&&sensors[id-SM_NODES].enabled;}
 bool enabled(bool value)const{if(id>=SM_NODES&&bool(*this))sensors[id-SM_NODES].enabled=value;return value;}
 float alpha()const{return node()?node()->alpha:0;}
 float alpha(float value)const{if(node())node()->alpha=value;return value;}
 void fps(unsigned value)const{if(node())node()->fps=std::max(1u,value);}
 void removeFromParent()const{if(id>=SM_NODES&&bool(*this))sensors[id-SM_NODES].alive=false;else if(node())sm_remove(vm,id);}
 void setTouchable(bool)const{}
 void speedUp(float,float,float)const;
};
using TimelineMovie=Movie;using TimelineSprite=Movie;using DisplayObject=Movie;using DisplayObjectContainer=Movie;using ISensor=Movie;
struct TouchList {unsigned length;};
struct TouchEvent {Movie currentTarget;bool began;TouchList getTouches(void*,const char *phase)const{return {unsigned(began==(!std::strcmp(phase,"began")))};}};
struct Scheduled {Movie movie;float due=0,restore_at=0;unsigned restore_fps=0;};
static Scheduled scheduled[32];
struct Fade {Movie movie;float start=0,end=0,from=0,to=0;};
static Fade fades[8];
void fade(Movie movie,float target,float duration){for(auto &f:fades)if(!f.end||f.movie.id==movie.id){f={movie,clock_seconds,clock_seconds+duration,movie.alpha(),target};return;}}
void Movie::speedUp(float target,float duration,float settle)const{if(!node())return;for(auto &job:scheduled)if(!job.due&&!job.restore_at){job.movie=*this;job.restore_fps=unsigned(settle);job.restore_at=clock_seconds+duration;fps(unsigned(target));break;}}
void delayPlay(Movie movie,float milliseconds){for(auto &job:scheduled)if(!job.due&&!job.restore_at){job.movie=movie;job.due=clock_seconds+milliseconds/1000;break;}}
void enableSensor(const String &name,bool enabled){for(auto &sensor:sensors)if(sensor.alive&&sensor.name==name)sensor.enabled=enabled;}
void enableSensors(std::initializer_list<const char*> names){for(auto name:names)enableSensor(name,true);}
void disableSensors(std::initializer_list<const char*> names){for(auto name:names)enableSensor(name,false);}
void removeQuadSensor(const String &name){for(auto &sensor:sensors)if(sensor.name==name)sensor.alive=false;}
Sensor &addQuadSensor(Movie parent,const String &name,float x,float y,float w,float h){
 for(auto &sensor:sensors)if(sensor.alive&&sensor.name==name&&sensor.parent==parent.id&&sensor.generation==parent.generation){sensor.x=x;sensor.y=y;sensor.w=w;sensor.h=h;sensor.enabled=true;return sensor;}
 for(auto &sensor:sensors)if(!sensor.alive){sensor={true,true,parent.id,parent.generation,name,x,y,w,h,0};return sensor;}
 vm->failed=true;std::snprintf(vm->error,sizeof vm->error,"Sensor capacity exceeded");return sensors[0];
}
void focusTo(float,float y,float zoom,bool=true,bool=false,bool=false){_focusY=y;_focusZoom=zoom;}
void weekFocusTo(float x,float y,float z,float=1){focusTo(x,y,z);}
void setFocusAC(float){}
struct GlobalFacade {
 void enableSensor(const String &n){::enableSensor(n,true);}
 void disableSensor(const String &n){::enableSensor(n,false);}
 void disableAllSensors(){for(auto &sensor:sensors)sensor.enabled=false;}
 void removeSensorByName(const String &n){removeQuadSensor(n);}
 void zatmeni(bool active,float delay,const char *next=nullptr,const char * =nullptr,bool=false){
  if(!active||!next)return;const char *names[]={"Start","Lyzar","Rybar","Vcely","Strom","Vodarna","Strojovna"};
  for(unsigned i=0;i<7;i++)if(!std::strcmp(next,names[i])){requested=int(i);transition_delay=delay;break;}
 }
 void playSound(const String &name,float gain=1){
  auto slash=name.find('/');String bank=slash==String::npos?vm->scene->name:name.substr(0,slash);String key=slash==String::npos?name:name.substr(slash+1);
  SmAssets *a=bank=="Univerzal"?vm->common:vm->scene;if(bank!=a->name)return;
  for(unsigned i=0;i<a->sound_count;i++)if(key==a->sounds[i].name||name==a->sounds[i].name){if(vm->sound)vm->sound(vm,a,a->sounds[i].id,gain,gain,vm->user);return;}
 }
 void playAtmoLoop(const String &name,float gain=1,float=0){if(music)music(name.c_str(),gain,false);}
 void stopAtmoLoop(const String &name,float=0){if(music)music(name.c_str(),0,true);}
 void volumeAtmoLoop(const String &name,float gain,float=0){if(music)music(name.c_str(),gain,gain<=0);}
 void stopLastLongChanels(float){}
} mGlobal,Global;

// Scene rules, actor bindings and sensors are generated from the owned game.
// The generator rejects unhandled decompiler constructs rather than guessing.
#include "rules.inc"

void named(Movie object){switch(level){case 0:if(object.name()=="Kopule")sprKopule=object;break;case 2:Rybar_onAddNamedObject(object);break;case 3:Vcely_onAddNamedObject(object);break;case 4:Strom_onAddNamedObject(object);break;case 5:Vodarna_onAddNamedObject(object);break;}}
void event(Movie object,const String &label){
 if(level==0){
  switch(hash(label)){
   case H("Hvezdy1"):space_motion(-0.00032f,1);break;
   case H("Hvezdy1Rev"):space_motion(0.00016f,1);break;
   case H("Hvezdy1End"):space_motion(0,3);break;
   case H("Hvezdy2"):space_motion(-0.00032f,0);break;
   case H("Hvezdy3"):space_motion(0.00004f,0);break;
   case H("Hvezdy4"):space_motion(0,0);break;
   case H("Koukal"):_layerScene.play();break;
   case H("Kopule"):sprKopule.visible(false);break;
   case H("Pristani"):mGlobal.zatmeni(true,0.5f,"Lyzar");break;
   case H("KONEC"):{qTma=Movie(sm_rectangle(vm,1900,1500));if(qTma.node()){qTma.node()->color[0]=qTma.node()->color[1]=qTma.node()->color[2]=0;qTma.alpha(0);sm_attach(vm,qTma.id,vm->root,SM_CHILDREN);fade(qTma,0.7f,5);}credits_deadline=clock_seconds+20;int id=-1;for(unsigned i=0;i<SM_IDS;i++)if(vm->scene->names[i]&&!std::strcmp(vm->scene->names[i],"EndCredits")){id=sm_create(vm,1,i,-1);break;}if(id>=0){sprCredits=Movie(id);sprCredits.alpha(0);sm_attach(vm,id,vm->root,SM_CHILDREN);fade(sprCredits,1,6);}break;}
   case H("END"):mGlobal.zatmeni(true,6.4f,"Start");break;
   default:break;
  }
  return;
 }
 switch(level){case 1:Lyzar_onAnimLabel(object,label);break;case 2:Rybar_onAnimLabel(object,label);break;case 3:Vcely_onAnimLabel(object,label);break;case 4:Strom_onAnimLabel(object,label);break;case 5:Vodarna_onAnimLabel(object,label);break;case 6:Strojovna_onAnimLabel(object,label);break;}
}
void touch(Movie sensor,bool began,bool moving){bWasMoving=moving;TouchEvent e{sensor,began};switch(level){case 0:Start_onTouchSensor(e);break;case 1:Lyzar_onTouchSensor(e);break;case 2:Rybar_onTouchSensor(e);break;case 3:Vcely_onTouchSensor(e);break;case 4:Strom_onTouchSensor(e);break;case 5:Vodarna_onTouchSensor(e);break;case 6:Strojovna_onTouchSensor(e);break;}}
}

void sm_game_init(SmVm *value,unsigned index,bool outro,SmMusic callback){
 vm=value;level=index;music=callback;requested=-1;transition_delay=credits_deadline=clock_seconds=0;entry_deadline=0.6f;_focusY=_focusZoom=0;
 space_phase=space_speed=space_from=space_target=space_elapsed=space_duration=0;if(outro)space_motion(0.000002f,0);
 captured_sensor=-1;for(auto &sensor:sensors)sensor=Sensor();for(auto &job:scheduled)job=Scheduled();for(auto &f:fades)f=Fade();reset_fields();
 vm->root=sm_container(vm);unsigned root=vm->scene->root;
 if(outro)for(unsigned i=0;i<SM_IDS;i++)if(vm->scene->names[i]&&!std::strcmp(vm->scene->names[i],"SceneOutro")){root=i;break;}
 _layerScene=Movie(sm_create(vm,1,root,vm->root));sm_attach(vm,_layerScene.id,vm->root,0);bind_scene();
 if(level==6){_layerScene.node()->transform.y=-100;qTma=Movie(sm_rectangle(vm,1900,1500));if(qTma.node()){qTma.node()->color[0]=qTma.node()->color[1]=qTma.node()->color[2]=0;qTma.node()->transform.y=100;qTma.alpha(0.92f);sm_attach(vm,qTma.id,_layerScene.id,_layerScene.indexOf(animDioda));}for(unsigned i=0;i<SM_IDS;i++)if(vm->scene->names[i]&&!std::strcmp(vm->scene->names[i],"Planetka")){_LayerPlaneta=Movie(sm_create(vm,1,i,vm->root));if(_LayerPlaneta.node()){_LayerPlaneta.node()->transform.x=950;_LayerPlaneta.node()->transform.y=750;}sm_attach(vm,_LayerPlaneta.id,vm->root,1);_LayerPlaneta.visible(false);break;}}
 if(outro)_layerScene.play();
}
void sm_game_event(SmVm*,int node,const char *label,void*){event(Movie(node),label);}
void sm_game_named(SmVm*,int node,void*){named(Movie(node));}
float sm_game_space_phase(void){return space_phase;}
void sm_game_update(float dt){clock_seconds+=dt;
 if(space_duration>0){space_elapsed+=dt;float t=std::min(1.0f,space_elapsed/space_duration);t=t*t*(3-2*t);space_speed=space_from+(space_target-space_from)*t;}
 space_phase+=space_speed*dt*50000;if(entry_deadline&&clock_seconds>=entry_deadline){entry_deadline=0;event(Movie(),"onZatmeni");}transition_delay=std::max(0.0f,transition_delay-dt);
 for(auto &sensor:sensors)if(sensor.alive&&(sensor.parent<0||!vm->nodes[sensor.parent].alive||vm->nodes[sensor.parent].generation!=sensor.generation))sensor.alive=false;
 for(auto &job:scheduled){if(job.due&&clock_seconds>=job.due){job.movie.play();job.due=0;}if(job.restore_at&&clock_seconds>=job.restore_at){job.movie.fps(job.restore_fps);job.restore_at=0;}}
 for(auto &f:fades)if(f.end){float t=std::min(1.0f,(clock_seconds-f.start)/(f.end-f.start));f.movie.alpha(f.from+(f.to-f.from)*t);if(t>=1)f.end=0;}
 if(credits_deadline&&clock_seconds>=credits_deadline){credits_deadline=0;event(Movie(),"END");}
}
bool sm_game_cinematic(void){return level==6&&_LayerPlaneta.visible();}
int sm_game_pending_transition(void){return requested;}
int sm_game_transition(void){return requested>=0&&transition_delay<=0?requested:-1;}
SmMatrix sm_game_camera(unsigned width,float zoom,float panx,float pany){SmMatrix m=sm_identity();m.a=m.d=0.16f*zoom;m.x=width*0.5f-950*m.a+panx;m.y=120-750*m.d+pany;return m;}
unsigned sm_game_sensor_count(void){unsigned count=0;for(auto &sensor:sensors)count+=sensor.alive&&sensor.enabled;return count;}
bool sm_game_click_named(const char *name,bool began){for(unsigned i=0;i<128;i++)if(sensors[i].alive&&sensors[i].enabled&&sensors[i].name==name){touch(Movie(SM_NODES+int(i)),began,false);return true;}return false;}
void sm_game_touch(float x,float y,bool began,bool moving){
 // Coordinates are authored scene coordinates obtained from the bottom camera.
 // Reverse sensor order follows display-list hit testing; BG is only fallback.
 if(!began){if(captured_sensor>=0){auto &s=sensors[captured_sensor];if(s.alive&&s.generation==captured_generation)touch(Movie(SM_NODES+captured_sensor),false,moving);}captured_sensor=-1;return;}
 if(x<0||x>=1900||y<0||y>=1500){captured_sensor=-1;return;}
 for(int i=127;i>=0;i--){auto &s=sensors[i];if(!s.alive||!s.enabled||s.parent<0||!vm->nodes[s.parent].alive||vm->nodes[s.parent].generation!=s.generation)continue;
  bool visible=true;for(int p=s.parent;p>=0;p=vm->nodes[p].parent)visible&=vm->nodes[p].visible;if(!visible)continue;
  float px=x,py=y;if(!sm_inverse_point(sm_world(vm,s.parent),&px,&py))continue;px-=s.x;py-=s.y;
  float c=std::cos(s.rotation),sn=std::sin(s.rotation),rx=c*px+sn*py,ry=-sn*px+c*py;
  if(rx>=0&&ry>=0&&rx<s.w&&ry<s.h){captured_sensor=i;captured_generation=s.generation;touch(Movie(SM_NODES+i),began,moving);return;}
 }
}
