"""Differential execution of every real timeline, with and without prepared frames."""
from pathlib import Path
import sys,subprocess,struct
ROOT=Path(__file__).resolve().parents[1];SHARED=ROOT/'source'
sys.path.insert(0,str(Path(__file__).resolve().parent));sys.path.insert(0,str(ROOT/'source/engine/tools'))
from host_compiler import compiler
from bake_animation import fingerprint
folder=ROOT/'work/baked-animation-test';folder.mkdir(exist_ok=True)
valid=(ROOT/'out/3ds/samorost1/levels/Start.lbk').read_bytes()
(folder/'stale.lbk').write_bytes(valid[:8]+struct.pack('<I',0)+valid[12:])
(folder/'truncated.lbk').write_bytes(valid[:-1])
bad=bytearray(valid);bad[-1]^=1;(folder/'corrupt.lbk').write_bytes(bad)
# Recompute payload checksum to exercise semantic validation rather than checksum.
for name,change in [('bad-slot',lambda b,n:struct.pack_into('<I',b,32+n*24,64)),('nan',lambda b,n:struct.pack_into('<I',b,32+n*24+8,0x7fc00000)),('bad-clip',lambda b,n:struct.pack_into('<I',b,32,8192))]:
 bad=bytearray(valid);n=struct.unpack_from('<I',bad,16)[0];change(bad,n);struct.pack_into('<I',bad,24,fingerprint(bad[32:]));(folder/(name+'.lbk')).write_bytes(bad)
source=folder/'check.c';source.write_text(r'''#include "engine/adapters/sprite_timeline/timeline.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static SmVm fast,slow;
typedef struct {unsigned events,named,sounds,hash;} Stats;
static Stats fs,ss;static unsigned comparisons,fast_steps,slow_steps,frames;
static void event(SmVm *v,int id,const char *label,void *user){(void)v;Stats *s=user;s->events++;s->hash=s->hash*33+(unsigned)id;while(*label)s->hash=s->hash*33+(unsigned char)*label++;}
static void named(SmVm *v,int id,void *user){(void)v;Stats *s=user;s->named++;s->hash=s->hash*33+(unsigned)id;}
static void sound(SmVm *v,SmAssets *a,unsigned id,float l,float r,void *user){(void)v;(void)a;Stats *s=user;s->sounds++;unsigned x,y;memcpy(&x,&l,4);memcpy(&y,&r,4);s->hash=s->hash*33+id+x+y;}
static void compare(void){
 assert(fast.failed==slow.failed&&!fast.failed);assert(fast.random==slow.random&&fast.tick==slow.tick&&fast.count==slow.count);
 assert(!memcmp(&fs,&ss,sizeof fs));
 for(unsigned i=0;i<SM_NODES;i++){assert(fast.nodes[i].alive==slow.nodes[i].alive);if(fast.nodes[i].alive)assert(!memcmp(fast.nodes+i,slow.nodes+i,sizeof(SmNode)));}
 comparisons++;
}
static void init(SmAssets *common,SmAssets *scene,unsigned id){
 memset(&fs,0,sizeof fs);memset(&ss,0,sizeof ss);
 sm_vm_init(&fast,common,scene,event,named,sound,&fs);sm_vm_init(&slow,common,scene,event,named,sound,&ss);slow.baked_enabled=false;
 fast.root=sm_create(&fast,1,id,-1);slow.root=sm_create(&slow,1,id,-1);assert(fast.root>=0&&slow.root>=0);compare();
}
static void finish(void){fast_steps+=fast.baked_steps;slow_steps+=slow.interpreted_steps;sm_vm_clear(&fast);sm_vm_clear(&slow);}
int main(int argc,char **argv){
 assert(argc==3);char path[1024];SmAssets common,scene;snprintf(path,sizeof path,"%s/levels/Univerzal.spk",argv[1]);assert(sm_assets_load(&common,path,"Univerzal"));
 const char *levels[]={"Start","Lyzar","Rybar","Vcely","Strom","Vodarna","Strojovna"};
 for(unsigned level=0;level<7;level++){
  snprintf(path,sizeof path,"%s/levels/%s.spk",argv[1],levels[level]);assert(sm_assets_load(&scene,path,levels[level]));assert(scene.baked.frame_count>0);frames+=scene.baked.frame_count;
  for(unsigned i=0;i<scene.sprite_count;i++){
   SmSprite *sprite=scene.sprites+i;unsigned total=sprite->data[0]|(unsigned)sprite->data[1]<<8;init(&common,&scene,sprite->id);
   for(unsigned f=1;f<total;f++){sm_next(&fast,fast.root);sm_next(&slow,slow.root);compare();}
   for(unsigned f=total;f>1;f-=f>7?7:1){sm_seek(&fast,fast.root,f,true);sm_seek(&slow,slow.root,f,true);compare();}
   /* Exercise loop reuse and nested animation clocks. */
   for(unsigned t=0;t<8;t++){sm_tick(&fast,0.04f);sm_tick(&slow,0.04f);compare();}
   finish();
  }
  init(&common,&scene,scene.root);
  for(unsigned tick=0;tick<1500;tick++){
   if(tick%100==0)for(unsigned i=0;i<SM_NODES;i++)if(fast.nodes[i].alive&&fast.nodes[i].kind==0){fast.nodes[i].transform.a+=0.125f;slow.nodes[i].transform.a+=0.125f;fast.nodes[i].alpha=slow.nodes[i].alpha=0.375f;break;}
   sm_tick(&fast,0.04f);sm_tick(&slow,0.04f);compare();
  }
  finish();sm_assets_free(&scene);
 }
 assert(frames==14304&&fast_steps>10000);
 snprintf(path,sizeof path,"%s/levels/Start.spk",argv[1]);assert(sm_assets_load(&scene,path,"Start"));
 const char *bad[]={"missing","stale","truncated","corrupt","bad-slot","nan","bad-clip"};
 for(unsigned i=0;i<7;i++){snprintf(path,sizeof path,"%s/%s.lbk",argv[2],bad[i]);assert(!sm_baked_load(&scene,path));assert(!scene.baked.frame_count&&!scene.baked.bytes);for(unsigned j=0;j<scene.sprite_count;j++)assert(!scene.sprites[j].baked_frames);}
 init(&common,&scene,scene.root);
 for(unsigned t=0;t<200;t++){sm_tick(&fast,0.04f);sm_tick(&slow,0.04f);compare();}
 assert(!fast.baked_steps);finish();sm_assets_free(&scene);sm_assets_free(&common);
 printf("PASS %u prepared frames; %u VM comparisons, %u prepared steps vs %u interpreter steps; seeks, loops, nested ticks, external edits, events and fallback\n",frames,comparisons,fast_steps,slow_steps);
}
''')
cc,env,msvc=compiler();exe=folder/'check.exe';units=[source,SHARED/'engine/adapters/sprite_timeline/timeline.c',SHARED/'engine/animation/baked.c',SHARED/'engine/assets/file_cache.c']
args=[cc,'/nologo','/std:c11','/O2','/MT','/I'+str(SHARED),*map(str,units),'/Fe:'+str(exe)] if msvc else [cc,'-std=c11','-O2','-I'+str(SHARED),*map(str,units),'-lm','-o',str(exe)]
subprocess.run(args,cwd=folder,env=env,check=True);subprocess.run([str(exe),str(ROOT/'out/3ds/samorost1'),str(folder)],cwd=folder,check=True)
