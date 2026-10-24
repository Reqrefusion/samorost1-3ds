from pathlib import Path
import sys,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
LUME=ROOT
sys.path.insert(0,str(Path(__file__).resolve().parent));from host_compiler import compiler
code=r'''
#include "engine/adapters/sprite_timeline/timeline.h"
#include <assert.h>
#include <stdio.h>
static SmAssets common,scene;static SmVm vm;
static unsigned labels;
static void event(SmVm *v,int id,const char *s,void *ctx){(void)v;(void)id;(void)ctx;labels++;if(labels<5)printf("label %s\n",s);}
int main(int argc,char **argv){
 setbuf(stdout,NULL);assert(argc==2);char path[1024];snprintf(path,sizeof path,"%s/levels/Univerzal.spk",argv[1]);assert(sm_assets_load(&common,path,"Univerzal"));
 for(unsigned i=0;i<common.sprite_count;i++){sm_vm_init(&vm,&common,&common,NULL,NULL,NULL,NULL);int object=sm_create(&vm,1,common.sprites[i].id,-1);assert(object>=0&&!vm.failed);}puts("PASS common timeline definitions");
 const char *names[]={"Start","Lyzar","Rybar","Vcely","Strom","Vodarna","Strojovna"};
 for(unsigned l=0;l<7;l++){
  snprintf(path,sizeof path,"%s/levels/%s.spk",argv[1],names[l]);assert(sm_assets_load(&scene,path,names[l]));
  sm_vm_init(&vm,&common,&scene,event,NULL,NULL,NULL);vm.root=sm_create(&vm,1,scene.root,-1);
  for(unsigned f=0;f<1500&&!vm.failed;f++)sm_tick(&vm,0.04f);
  if(vm.failed){printf("FAIL %s: %s\n",names[l],vm.error);return 3;}
  unsigned active=0;for(unsigned n=0;n<SM_NODES;n++)active+=vm.nodes[n].alive;
  printf("PASS %s: 60 seconds, %u active nodes\n",names[l],active);
  unsigned tested_frames=0;
  for(unsigned si=0;si<scene.sprite_count;si++){
   sm_vm_init(&vm,&common,&scene,NULL,NULL,NULL,NULL);vm.root=sm_container(&vm);SmSprite *sprite=scene.sprites+si;int object=sm_create(&vm,1,sprite->id,vm.root);assert(object>=0);sm_attach(&vm,object,vm.root,0);
   unsigned total=(unsigned)sprite->data[0]|(unsigned)sprite->data[1]<<8;
   for(unsigned frame=1;frame<total&&!vm.failed;frame++)sm_next(&vm,object);
   if(vm.failed){printf("FAIL exhaustive %s sprite %u: %s\n",names[l],sprite->id,vm.error);return 4;}
   tested_frames+=total;
  }
  printf("PASS %s: all %u timeline definitions, %u authored frames replayed\n",names[l],scene.sprite_count,tested_frames);sm_vm_clear(&vm);sm_assets_free(&scene);
 }
 sm_assets_free(&common);return 0;
}
'''
cc,env,msvc=compiler();folder=ROOT/'work/native-test';folder.mkdir(parents=True,exist_ok=True);source=folder/'test.c';source.write_text(code);exe=folder/'test.exe'
units=[source,ROOT/'source/engine/adapters/sprite_timeline/timeline.c',ROOT/'source/engine/animation/baked.c',ROOT/'source/engine/assets/file_cache.c',ROOT/'source/engine/core/jobs_state.c']
args=[cc,'/nologo','/std:c11','/utf-8','/O2','/MT','/I'+str(ROOT/'source'),'/I'+str(ROOT/'source'),*map(str,units),'/Fe:'+str(exe)]
if not msvc:args=[cc,'-std=c11','-O2','-I'+str(ROOT/'source'),'-I'+str(ROOT/'source'),*map(str,units),'-lm','-o',str(exe)]
r=subprocess.run(args,cwd=folder,env=env,capture_output=True,text=True);print(r.stdout+r.stderr);r.check_returncode();r=subprocess.run([str(exe),str(ROOT/'out/3ds/samorost1')],cwd=folder,capture_output=True,text=True);print(r.stdout+r.stderr);r.check_returncode()