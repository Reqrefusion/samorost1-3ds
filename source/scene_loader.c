#include "scene_loader.h"
#include "depth.h"
#include "engine/render/texture_filter.h"
#include <lz4.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __3DS__
#include <3ds.h>
#include "engine/platform/3ds/audio_worker.h"
static uint64_t milliseconds(void){return osGetTime();}
#else
#include <time.h>
static uint64_t milliseconds(void){return (uint64_t)clock()*1000/CLOCKS_PER_SEC;}
#endif
static unsigned read32(const uint8_t *p){return (unsigned)p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
static bool read_all(FILE *file,uint8_t *out,unsigned size){
 while(size){unsigned n=size>65536?65536:size;if(fread(out,1,n,file)!=n)return false;out+=n;size-=n;
#ifdef __3DS__
 svcSleepThread(0);
#endif
 }
 return true;
}
void sm_texture_pixels_free(SmTexturePixels *pixels){for(unsigned i=0;i<SM_TEXTURES;i++){free(pixels[i].data);memset(pixels+i,0,sizeof *pixels);}}
bool sm_texture_pixels_load(SmAssets *assets,const char *directory,SmTexturePixels *pixels){
 memset(pixels,0,sizeof(*pixels)*SM_TEXTURES);
 for(unsigned i=0;i<assets->texture_count;i++){
  char path[512];snprintf(path,sizeof path,"%s/textures/%s/%s.stx",directory,assets->name,assets->textures[i].name);FILE *f=fopen(path,"rb");if(!f)goto fail;
  uint8_t h[20];if(fread(h,1,20,f)!=20||memcmp(h,"STX1",4)){fclose(f);goto fail;}
  unsigned w=read32(h+4),height=read32(h+8),raw=read32(h+12),packed=read32(h+16);
  if(w!=assets->textures[i].w||height!=assets->textures[i].h||w<8||height<8||w>1024||height>1024||(w&(w-1))||(height&(height-1))||raw!=w*height*4||!packed||packed>raw+1024){fclose(f);goto fail;}
  unsigned levels=sm_mip_levels(w,height),total=sm_mip_size(raw,levels);
  uint8_t *input=malloc(packed);pixels[i].data=malloc(total);
  if(!pixels[i].data&&levels){levels=0;total=raw;pixels[i].data=malloc(total);}if(!input||!pixels[i].data){free(input);fclose(f);goto fail;}
  bool ok=read_all(f,input,packed)&&fgetc(f)==EOF;fclose(f);
  int decoded=ok?LZ4_decompress_safe((char*)input,(char*)pixels[i].data,(int)packed,(int)raw):-1;free(input);if(decoded!=(int)raw)goto fail;
  sm_mip_generate(pixels[i].data,w,height,levels);
  pixels[i].width=w;pixels[i].height=height;pixels[i].size=total;pixels[i].mip_levels=levels;
 }
 return true;
fail:sm_texture_pixels_free(pixels);return false;
}
void sm_scene_dispose(SmSceneData *data){sm_texture_pixels_free(data->pixels);sm_assets_free(&data->assets);memset(data,0,sizeof *data);}
bool sm_scene_prepare(SmSceneData *data,const char *directory,const char *name){
 memset(data,0,sizeof *data);uint64_t start=milliseconds();char path[512];snprintf(path,sizeof path,"%s/levels/%s.spk",directory,name);
 if(!sm_assets_load(&data->assets,path,name))return false;
 sm_depth_load(&data->assets,directory);
 if(!data->assets.depth_count)data->builtin_relief=sm_depth_builtin(&data->assets);
 data->assets_ms=milliseconds()-start;start=milliseconds();
 if(!sm_texture_pixels_load(&data->assets,directory,data->pixels)){sm_scene_dispose(data);return false;}
 data->textures_ms=milliseconds()-start;return true;
}
#ifdef __3DS__
static SmSceneData staged;static Thread worker;static LightLock mutex;
static char stage_directory[384],stage_name[32];static int status;
static void entry(void *unused){(void)unused;bool ok=sm_scene_prepare(&staged,stage_directory,stage_name);LightLock_Lock(&mutex);status=ok?1:-1;LightLock_Unlock(&mutex);}
void sm_scene_loader_init(const char *directory){LightLock_Init(&mutex);snprintf(stage_directory,sizeof stage_directory,"%s",directory);}
bool sm_scene_loader_start(const char *name){
 if(worker)return false;snprintf(stage_name,sizeof stage_name,"%s",name);status=0;int core;
 worker=lume_music_worker(entry,&core);
 if(!worker)worker=threadCreate(entry,NULL,64*1024,0x30,-2,false);
 return worker!=NULL;
}
int sm_scene_loader_poll(void){LightLock_Lock(&mutex);int result=status;LightLock_Unlock(&mutex);return result;}
SmSceneData *sm_scene_loader_take(void){if(!worker||sm_scene_loader_poll()!=1)return NULL;threadJoin(worker,U64_MAX);threadFree(worker);worker=NULL;return &staged;}
void sm_scene_loader_release(void){sm_scene_dispose(&staged);}
void sm_scene_loader_shutdown(void){if(worker){threadJoin(worker,U64_MAX);threadFree(worker);worker=NULL;}sm_scene_dispose(&staged);}
#endif
