"""Run the real native scene rules against all converted scene packages."""
from pathlib import Path
import subprocess,sys,re,json
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from host_compiler import compiler
folder=ROOT/'work/game-test';folder.mkdir(parents=True,exist_ok=True)
source=folder/'test.cpp'
names=set()
names.update(re.findall(r'addQuadSensor\([^,]+,"([^"]+)"',(ROOT/'source/rules.inc').read_text(encoding='utf-8')))
names.update('Mlok'+str(i) for i in range(1,7));names.add('Snek')
expected_maps=sum(__import__('struct').unpack_from('<I',p.read_bytes(),12)[0] for p in (ROOT/'maps/depth').glob('*.smd'))
code=r'''
#include "game.h"
#include "depth.h"
#include "stereo.h"
#include <cassert>
#include <cstdio>
#include <cstring>
#include <initializer_list>
#include <cstdlib>
#include <cmath>
static SmAssets common,scene;static SmVm vm;
extern "C" void sm_test_view(void);
extern "C" void sm_test_layers(SmVm*);
extern "C" void sm_test_contacts(SmVm*);
extern "C" void sm_test_layers_finish(void);
static unsigned labels,transitions,depth_images,cinematic_frames;
static void event(SmVm *v,int id,const char *name,void *user){labels++;sm_game_event(v,id,name,user);}
static void tick(unsigned frames){for(unsigned i=0;i<frames&&!vm.failed;i++){sm_game_update(0.04f);sm_tick(&vm,0.04f);if(sm_game_cinematic())cinematic_frames++;sm_test_layers(&vm);sm_test_contacts(&vm);}if(vm.failed){printf("FAIL: %s\n",vm.error);fflush(stdout);assert(false);}}
static void click(const char *name){sm_game_click_named(name,true);sm_game_click_named(name,false);}
static int actor(const char *name){int id=sm_find(&vm,name);assert(id>=0);return id;}
int main(int argc,char **argv){
 setbuf(stdout,nullptr);sm_test_view();assert(argc==3);char path[1024];snprintf(path,sizeof path,"%s/levels/Univerzal.spk",argv[1]);assert(sm_assets_load(&common,path,"Univerzal"));
 const char *levels[]={"Start","Lyzar","Rybar","Vcely","Strom","Vodarna","Strojovna"};
 const char *sensors[]={SENSOR_NAMES};
 for(unsigned level=0;level<7;level++){
  snprintf(path,sizeof path,"%s/levels/%s.spk",argv[1],levels[level]);assert(sm_assets_load(&scene,path,levels[level]));
  if(level==0){assert(!sm_depth_load(&scene,argv[2]));assert(scene.depth_count==0);}
  assert(sm_depth_load(&scene,argv[1]));unsigned sd_count=scene.depth_count;unsigned sd_hash=0;
  for(unsigned i=0;i<scene.image_count;i++)if(scene.images[i].relief){for(unsigned j=0;j<81;j++)sd_hash=sd_hash*33u+(uint8_t)scene.images[i].relief[j];free(scene.images[i].relief);scene.images[i].relief=nullptr;}
  scene.depth_count=0;assert(sm_depth_builtin(&scene));assert(scene.depth_count==sd_count);unsigned embedded_hash=0;
  for(unsigned i=0;i<scene.image_count;i++)if(scene.images[i].relief)for(unsigned j=0;j<81;j++){int8_t sample=scene.images[i].relief[j];embedded_hash=embedded_hash*33u+(uint8_t)sample;
   float l=sm_stereo_surface(0,1,0,sample),r=sm_stereo_surface(1,1,0,sample);
   assert(fabsf(l+r)<0.00001f);assert(sm_stereo_surface(0,0,0.5f,sample)==0&&sm_stereo_surface(1,0,0.5f,sample)==0&&sm_stereo_surface(2,1,0.5f,sample)==0);
   assert((sample>=0&&l>=0&&r<=0)||(sample<0&&l<0&&r>0));
  }
  assert(sd_hash==embedded_hash);depth_images+=scene.depth_count;
  for(unsigned i=0;i<scene.image_count;i++)if(scene.images[i].relief){const int8_t *d=scene.images[i].relief;for(unsigned j=0;j<9;j++)assert(d[j]==0&&d[72+j]==0&&d[j*9]==0&&d[j*9+8]==0);}
  sm_vm_init(&vm,&common,&scene,event,sm_game_named,nullptr,nullptr);sm_game_init(&vm,level,false,nullptr);assert(!vm.failed);
  if(level==0){click("Domek");tick(4000);assert(sm_game_transition()==1);printf("PASS intro reaches Lyzar\n");}
  if(level==1){
   assert(!sm_game_click_named("Klic",false));assert(!sm_game_click_named("Lyzar",false));
   for(unsigned puff=0;puff<3;puff++){assert(sm_game_click_named("RukaDymka",false));tick(500);}
   assert(sm_game_click_named("Klic",false));tick(500);
   int child=sm_child(&vm,actor("Kurak"),0);unsigned generation=vm.nodes[child].generation;sm_seek(&vm,actor("Kurak"),sm_frame(&vm,actor("Kurak")),false);assert(sm_child(&vm,actor("Kurak"),0)==child&&vm.nodes[child].generation==generation);
   printf("PASS smoker/key gating and same-frame child preservation\n");
  }
  if(level==6){int room=sm_child(&vm,vm.root,0),planet=sm_child(&vm,vm.root,1);assert(vm.nodes[room].transform.y==-100);assert(vm.nodes[planet].transform.x==950&&vm.nodes[planet].transform.y==750);assert(!sm_game_cinematic());assert(!sm_game_click_named("Plus",false));click("Vypinac");tick(100);assert(sm_game_click_named("Plus",false));printf("PASS engine-room light gates controls\n");}
  
  if(level==1){click("Sberac1");tick(100);click("Cedulka");tick(100);click("Cedulka");tick(100);click("Lano1");tick(500);click("Cudl");tick(500);click("Lyzar");tick(1500);}
  if(level==3){
   for(const char *name:{"Strom0","Strom1","Strom2","Strom3"}){click(name);tick(250);}
   click("DiraSosak");tick(500);
   for(unsigned i=0;i<10000&&sm_game_transition()<0;i++){if(i%100==0)for(const char *name:{"Dira","Vypinac","Skret","DoGondoly","Gondola"})click(name);if(i%5==0)click("Kytka");tick(1);}
  }
  if(level==4){
   for(unsigned i=0;i<3000;i++){for(const char *name:{"Cerv1","Cerv2","Cerv3","Cerv4"})click(name);tick(1);}
   click("Dutina");tick(300);click("Veverka");tick(500);click("Veverka");tick(500);click("Veverka");tick(500);
   for(unsigned i=0;i<5000&&sm_game_transition()<0;i++){if(i%25==0)for(const char *name:{"Sova0","Sova1","Sova2","Sova3","Gondola"})click(name);tick(1);}
  }
  if(level==5){
   click("Klika1");
   for(unsigned i=0;i<10000&&sm_game_transition()<0;i++){
    if(i%50==0){for(const char *name:{"GondolaTlac","Gondola","Skret"})click(name);if(sm_frame(&vm,actor("Mravec"))>1)click("Schody1");}
    if(i%100==0&&sm_frame(&vm,actor("Rozvadec"))==1)click("Rozvadec");
    for(unsigned k=3;k<=6;k++){char name[16];snprintf(name,sizeof name,"Knob%u",k);int id=sm_find(&vm,name);if(id>=0&&sm_frame(&vm,id)==1)click(name);}
    if(sm_frame(&vm,actor("Dvere"))>1)click("Dvere");tick(1);
   }
  }
  
  
  unsigned seed=0x32ac+level;
  for(unsigned i=0;i<12000;i++){
   if(i%15==0){seed=seed*1664525u+1013904223u;click(sensors[seed%(sizeof sensors/sizeof sensors[0])]);}
   tick(1);if(sm_game_transition()>=0){transitions++;break;}
  }
  assert(sm_game_transition()==int((level+1)%7)); unsigned active=0;for(auto &node:vm.nodes)active+=node.alive;
  
  printf("PASS %s mixed interaction: %u active nodes, %u sensors\n",levels[level],active,sm_game_sensor_count());sm_vm_clear(&vm);sm_assets_free(&scene);
 }
 assert(depth_images==40&&cinematic_frames>0);puts("PASS 40 embedded/SD relief maps match; eye symmetry, near/far sign, zero slider and mono bottom verified");
 printf("PASS %u animation callbacks; %u reachable scene transitions\n",labels,transitions);
 snprintf(path,sizeof path,"%s/levels/Start.spk",argv[1]);assert(sm_assets_load(&scene,path,"Start"));sm_vm_init(&vm,&common,&scene,event,sm_game_named,nullptr,nullptr);sm_game_init(&vm,0,true,nullptr);tick(10000);assert(sm_game_transition()==0);puts("PASS outro/credits returns to Start");sm_assets_free(&scene);sm_assets_free(&common);sm_test_layers_finish();
}
'''.replace('depth_images==40',f'depth_images=={expected_maps}').replace('PASS 40 relief maps',f'PASS {expected_maps} relief maps').replace('SENSOR_NAMES',','.join('"'+s+'"' for s in sorted(names)))
source.write_text(code,encoding='utf-8')
cc,env,msvc=compiler()

units=[ROOT/'tests/layers_check.c',ROOT/'source/layers.c',ROOT/'tests/view_check.c',ROOT/'source/engine/adapters/sprite_timeline/timeline.c',ROOT/'source/engine/animation/baked.c',ROOT/'source/depth.c',ROOT/'source/engine/assets/file_cache.c',ROOT/'source/engine/core/jobs_state.c']
objects=[]
def run(args):
    r=subprocess.run([str(a) for a in args],cwd=folder,env=env,capture_output=True,text=True);print(r.stdout+r.stderr);r.check_returncode()
for i,unit in enumerate(units):
    obj=folder/f'{i}.obj';run([cc,'/nologo','/c','/O2','/MT','/std:c11','/I'+str(ROOT/'source'),'/I'+str(ROOT/'source'),unit,'/Fo:'+str(obj)] if msvc else [cc,'-std=c11','-O2','-c','-I'+str(ROOT/'source'),'-I'+str(ROOT/'source'),unit,'-o',obj]);objects.append(obj)
exe=folder/'game-test.exe'
run([cc,'/nologo','/std:c++17','/utf-8','/O2','/MT','/EHsc','/I'+str(ROOT/'source'),'/I'+str(ROOT/'source'),source,ROOT/'source/game.cpp',*objects,'/Fe:'+str(exe)] if msvc else [__import__('shutil').which('c++') or 'g++','-std=c++17','-O2','-I'+str(ROOT/'source'),'-I'+str(ROOT/'source'),source,ROOT/'source/game.cpp',*objects,'-lm','-o',exe])
bad=folder/'bad-depth/depth';bad.mkdir(parents=True,exist_ok=True);(bad/'Start.smd').write_bytes(b'SMD1'+b'\0'*4)
run([exe,ROOT/'out/3ds/samorost1',bad.parent])
