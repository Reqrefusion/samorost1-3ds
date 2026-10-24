"""Verify the real renderer flushes before texture/offset/view changes."""
from pathlib import Path
import subprocess,sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from host_compiler import compiler

def block(source,name):
    start=source.index('static void '+name+'(');end=source.index('{',start)+1;depth=1
    while depth:depth+=(source[end]=='{')-(source[end]=='}');end+=1
    return source[start:end]

text=(ROOT/'source/canvas.c').read_text()
folder=ROOT/'work/batch-test';folder.mkdir(parents=True,exist_ok=True)
code=r'''
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
typedef struct {int id;} C3D_Tex;
typedef struct {int unused;} C3D_TexEnv;
static C3D_TexEnv dummy;
static unsigned used,batch_start,batch_count,draw_calls;
static C3D_Tex *batch_texture,*bound;
static float batch_offset[4];
static struct {C3D_Tex *texture;unsigned start,count;} commands[8];
#define GPU_TRIANGLES 0
static void C3D_DrawArrays(int mode,unsigned start,unsigned count){assert(mode==0);commands[draw_calls].texture=bound;commands[draw_calls].start=start;commands[draw_calls].count=count;}
static void C3D_TexBind(int unit,C3D_Tex *tex){assert(unit==0);bound=tex;}
#define C3D_TexEnvInit(...) ((void)0)
#define C3D_GetTexEnv(...) (&dummy)
#define C3D_TexEnvSrc(...) ((void)0)
#define C3D_TexEnvFunc(...) ((void)0)
#define C3D_TexEnvColor(...) ((void)0)
'''+block(text,'flush_batch')+'\n'+block(text,'environment')+r'''
int main(void){
 C3D_Tex a={1},b={2};float zero[4]={0},offset[4]={0.1f,0,0,0};
 environment(&a,zero);used+=6;batch_count+=6;
 environment(&a,zero);assert(draw_calls==0);used+=6;batch_count+=6;
 environment(&b,zero);assert(draw_calls==1&&commands[0].texture==&a&&commands[0].start==0&&commands[0].count==12);
 used+=3;batch_count+=3;environment(&b,offset);
 assert(draw_calls==2&&commands[1].texture==&b&&commands[1].start==12&&commands[1].count==3);
 used+=6;batch_count+=6;flush_batch();batch_texture=NULL;
 assert(draw_calls==3&&commands[2].start==15&&commands[2].count==6);
 environment(&a,zero);used+=6;batch_count+=6;flush_batch();
 assert(draw_calls==4&&commands[3].texture==&a&&commands[3].start==21&&commands[3].count==6);
 puts("PASS adjacent merge and ordered flush before texture, color-offset and view changes");
}
'''
source=folder/'test.c';source.write_text(code)
cc,env,msvc=compiler();exe=folder/'test.exe'
args=[cc,'/nologo','/std:c11','/O2','/MT',source,'/Fe:'+str(exe)] if msvc else [cc,'-std=c11','-O2',source,'-lm','-o',exe]
subprocess.run(list(map(str,args)),cwd=folder,env=env,check=True);subprocess.run([str(exe)],check=True)
