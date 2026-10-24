"""Exercise real curved rope meshes at both screen scales, rotations and zooms."""
from pathlib import Path
import sys,subprocess
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from host_compiler import compiler
folder=ROOT/'work/stroke-test';folder.mkdir(exist_ok=True)
source=folder/'stroke-test.c'
source.write_text(r'''#include "engine/adapters/sprite_timeline/timeline.h"
#include "engine/adapters/sprite_timeline/stroke.h"
#include <stdio.h>
#include <assert.h>
int main(int argc,char **argv){
 assert(argc==2);unsigned total=0,curved=0,strom=0,strom_curved=0,lyzar=0,samples=0;
 const char *names[]={"Start","Lyzar","Rybar","Vcely","Strom","Vodarna","Strojovna"};
 for(unsigned level=0;level<7;level++){
  char path[1024];snprintf(path,sizeof path,"%s/levels/%s.spk",argv[1],names[level]);SmAssets assets;
  assert(sm_assets_load(&assets,path,names[level]));unsigned count=0;
  for(unsigned i=0;i<assets.shape_count;i++){
   SmShape *s=assets.shapes+i;if(!s->stroke_pairs)continue;count++;total++;if(s->triangles>2){curved++;if(level==4)strom_curved++;}
   SmMesh *mesh=assets.mesh+s->buffer;unsigned previous_end=0;
   for(unsigned r=0;r<s->stroke_count;r++){
   SmStrokeRun *run=s->strokes+r;assert(run->triangle>=previous_end&&run->triangle+run->triangles<=s->triangles);previous_end=run->triangle+run->triangles;
   for(unsigned pair=0;pair<run->pairs;pair++)for(unsigned zoom=0;zoom<3;zoom++)for(unsigned eye=0;eye<3;eye++){
    const float *a=mesh->vertices+(run->first+pair*2)*6,*b=a+6;
    float scale=zoom==0?0.16f:zoom==1?0.32f:0.8f;
    float angle=0.37f;SmMatrix m={scale*cosf(angle),scale*sinf(angle),-scale*sinf(angle),scale*cosf(angle),(float)eye*3,11};
    float ax,ay,bx,by;sm_stroke_pair(m,a,b,&ax,&ay,&bx,&by);
    float width=hypotf(bx-ax,by-ay),original=hypotf(b[0]-a[0],b[1]-a[1])*scale;
    assert(fabsf(width-fmaxf(1.35f,original))<0.002f);
    float cx=(a[0]+b[0])*0.5f,cy=(a[1]+b[1])*0.5f;
    assert(fabsf((ax+bx)*0.5f-(m.a*cx+m.c*cy+m.x))<0.002f);
    assert(fabsf((ay+by)*0.5f-(m.b*cx+m.d*cy+m.y))<0.002f);samples++;
   }
  }
   }
  if(level==1)lyzar=count;if(level==4)strom=count;printf("%s: %u stroke meshes\n",names[level],count);sm_assets_free(&assets);
 }
 assert(strom>=270&&strom_curved>=168&&lyzar>=100&&samples>10000);
 printf("PASS %u stroke meshes (%u curved), %u rotated/zoomed/eye samples; minimum width and center preserved\n",total,curved,samples);return 0;
}
''')
cc,env,msvc=compiler();exe=folder/'stroke-test.exe'
units=[source,ROOT/'source/engine/adapters/sprite_timeline/timeline.c',ROOT/'source/engine/animation/baked.c',ROOT/'source/engine/assets/file_cache.c']
args=[cc,'/nologo','/O2','/MT','/std:c11','/I'+str(ROOT/'source'),'/I'+str(ROOT/'source'),*map(str,units),'/Fe:'+str(exe)] if msvc else [cc,'-std=c11','-O2','-I'+str(ROOT/'source'),'-I'+str(ROOT/'source'),*map(str,units),'-lm','-o',str(exe)]
subprocess.run(args,cwd=folder,env=env,check=True)
subprocess.run([str(exe),str(ROOT/'out/3ds/samorost1')],cwd=folder,check=True)
