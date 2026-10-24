#include "audio.h"
#include "engine/platform/3ds/audio_device.h"
#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#define CHANNELS 8
#define BUFFERS 3
#define FRAMES 2048
typedef struct {FILE *file;ndspWaveBuf buffers[BUFFERS];unsigned remaining,total;bool loop,started;char name[128];} Voice;
static Voice voices[CHANNELS];static char directory[384];static bool ready;static unsigned next_effect=2,underruns;
static LumeAudioDeviceStatus device;static Result device_result;
static uint32_t le32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void stop(unsigned ch){Voice *v=voices+ch;ndspChnWaveBufClear(ch);if(v->file)fclose(v->file);v->file=NULL;v->started=false;v->name[0]=0;for(unsigned i=0;i<BUFFERS;i++)v->buffers[i].status=NDSP_WBUF_FREE;}
void sm_audio_reset(void){if(ready)for(unsigned i=0;i<CHANNELS;i++)stop(i);}
void sm_audio_close(void){if(!ready)return;sm_audio_reset();for(unsigned ch=0;ch<CHANNELS;ch++)for(unsigned i=0;i<BUFFERS;i++){linearFree((void*)voices[ch].buffers[i].data_vaddr);voices[ch].buffers[i].data_vaddr=NULL;}ndspExit();ready=false;}
bool sm_audio_init(const char *path){
 snprintf(directory,sizeof directory,"%s",path);device_result=lume_audio_device_init(&device);if(R_FAILED(device_result))return false;ready=true;ndspSetOutputMode(NDSP_OUTPUT_STEREO);
 for(unsigned ch=0;ch<CHANNELS;ch++){
  ndspChnReset(ch);ndspChnSetInterp(ch,NDSP_INTERP_LINEAR);ndspChnSetRate(ch,32000);ndspChnSetFormat(ch,NDSP_FORMAT_STEREO_PCM16);
  for(unsigned i=0;i<BUFFERS;i++){voices[ch].buffers[i].data_vaddr=linearAlloc(FRAMES*4);if(!voices[ch].buffers[i].data_vaddr){sm_audio_close();return false;}}
 }
 return true;
}
static void open_voice(unsigned ch,const char *bank,unsigned id,const char *name,float left,float right,bool loop){
 if(!ready)return;stop(ch);Voice *v=voices+ch;char path[512];snprintf(path,sizeof path,"%s/audio/%s/%u.pcm",directory,bank,id);v->file=fopen(path,"rb");if(!v->file)return;
 uint8_t h[16];if(fread(h,1,sizeof h,v->file)!=sizeof h||memcmp(h,"PCM1",4)||le32(h+4)!=32000||h[8]!=2||h[9]||h[10]!=16||h[11]||!le32(h+12)){stop(ch);return;}
 v->remaining=v->total=le32(h+12);v->loop=loop;snprintf(v->name,sizeof v->name,"%s",name?name:"");
 float mix[12]={0};mix[0]=fmaxf(0,fminf(1,left));mix[1]=fmaxf(0,fminf(1,right));ndspChnSetMix(ch,mix);
}
void sm_audio_event(SmVm *vm,SmAssets *a,unsigned id,float left,float right,void *user){(void)vm;(void)user;open_voice(next_effect,a->name,id,"",left,right,false);next_effect++;if(next_effect==CHANNELS)next_effect=2;}
void sm_audio_music(const char *name,float gain,bool halt){
 if(!ready)return;unsigned ch=strstr(name,"/Atmo")?0:1;
 if(halt){if(!strcmp(voices[ch].name,name))stop(ch);return;}
 if(voices[ch].file&&!strcmp(voices[ch].name,name))return;
 // Sound metadata is supplied by the current VM through the application.
 extern SmVm *sm_application_vm;
 SmVm *vm=sm_application_vm;if(!vm)return;const char *slash=strchr(name,'/');if(!slash)return;
 SmAssets *a=!strncmp(name,"Univerzal/",10)?vm->common:vm->scene;
 for(unsigned i=0;i<a->sound_count;i++)if(!strcmp(a->sounds[i].name,slash+1)||!strcmp(a->sounds[i].name,name)){open_voice(ch,a->name,a->sounds[i].id,name,gain,gain,true);return;}
}
void sm_audio_update(void){
 if(!ready)return;
 for(unsigned ch=0;ch<CHANNELS;ch++){Voice *v=voices+ch;if(!v->file)continue;unsigned queued=0;
  for(unsigned i=0;i<BUFFERS;i++)queued+=v->buffers[i].status==NDSP_WBUF_QUEUED||v->buffers[i].status==NDSP_WBUF_PLAYING;
  if(v->started&&!queued&&v->remaining)underruns++;
  for(unsigned i=0;i<BUFFERS;i++){
   ndspWaveBuf *b=v->buffers+i;if(b->status==NDSP_WBUF_QUEUED||b->status==NDSP_WBUF_PLAYING)continue;
   unsigned frames=0;while(frames<FRAMES){
    if(!v->remaining){if(!v->loop)break;if(fseek(v->file,16,SEEK_SET)){v->loop=false;break;}v->remaining=v->total;}
    unsigned n=FRAMES-frames;if(n>v->remaining)n=v->remaining;size_t read=fread((uint8_t*)(void*)b->data_vaddr+frames*4,4,n,v->file);frames+=(unsigned)read;v->remaining-=(unsigned)read;
    if(read!=n){v->remaining=0;v->loop=false;break;}
   }
   if(frames){b->nsamples=frames;b->looping=false;DSP_FlushDataCache(b->data_vaddr,frames*4);ndspChnWaveBufAdd(ch,b);v->started=true;queued++;}
  }
  if(!queued&&!v->remaining&&!v->loop)stop(ch);
 }
}
unsigned sm_audio_underruns(void){return underruns;}

unsigned sm_audio_active(void){unsigned count=0;for(unsigned i=0;i<CHANNELS;i++)count+=voices[i].file!=NULL;return count;}
unsigned long sm_audio_initial_result(void){return (unsigned long)device.initial_result;}
unsigned long sm_audio_final_result(void){return (unsigned long)device_result;}
bool sm_audio_used_hle(void){return device.emulator_retry;}
