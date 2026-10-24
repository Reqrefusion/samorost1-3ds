"""Compile the actual shared audio initializer against controlled service results."""
from pathlib import Path
import subprocess,sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from host_compiler import compiler
folder=ROOT/'work/audio-device-test';folder.mkdir(parents=True,exist_ok=True)
(folder/'3ds.h').write_text('''#include <stdint.h>
#include <stdbool.h>
typedef int32_t Result;typedef int64_t s64;
#define R_FAILED(x) ((x)<0)
#define R_SUCCEEDED(x) ((x)>=0)
Result ndspInit(void);
Result svcGetSystemInfo(s64*,unsigned,unsigned);
void ndspUseComponent(const void*,unsigned,unsigned,unsigned);
''')
source=folder/'test.c'
source.write_text('''#include "engine/platform/3ds/audio_device.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static Result first,second,query;static s64 model;static unsigned calls,queries,components;
Result ndspInit(void){return ++calls==1?first:second;}
Result svcGetSystemInfo(s64 *out,unsigned type,unsigned index){assert(type==0x20000&&index==0);queries++;*out=model;return query;}
void ndspUseComponent(const void *bytes,unsigned size,unsigned program,unsigned data){assert(bytes&&size>1&&program==255&&data==255);assert(strstr(bytes,"HLE audio interface"));components++;}
static void check(Result initial,Result retry,Result query_result,s64 id,bool expect_retry){
 first=initial;second=retry;query=query_result;model=id;calls=queries=components=0;
 LumeAudioDeviceStatus status;Result result=lume_audio_device_init(&status);
 assert(status.initial_result==initial&&status.emulator_retry==expect_retry);
 assert(result==(expect_retry?retry:initial));assert(calls==(expect_retry?2u:1u));assert(components==unsigned_unused);
 assert(queries==(initial<0?1u:0u));
}
int main(void){
 check(0,-1,0,1,false);check(-1,0,-1,1,false);check(-1,0,0,0,false);check(-1,0,0,3,false);
 check(-1,0,0,1,true);check(-1,0,0,2,true);check(-1,-2,0,1,true);
 puts("PASS normal DSP, physical-console guard, Azahar IDs 1/2, and retry failure");
}
'''.replace('unsigned_unused','(expect_retry?1u:0u)'))
cc,env,msvc=compiler();exe=folder/'test.exe'
args=[cc,'/nologo','/std:c11','/O2','/MT','/I'+str(folder),'/I'+str(ROOT/'source'),str(source),'/Fe:'+str(exe)] if msvc else [cc,'-std=c11','-O2','-I'+str(folder),'-I'+str(ROOT/'source'),str(source),'-o',str(exe)]
subprocess.run(args,cwd=folder,env=env,check=True)
subprocess.run([str(exe)],check=True)
