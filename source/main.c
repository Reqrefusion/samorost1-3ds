#include <3ds.h>
#include <stdio.h>
#include <sys/stat.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "engine/adapters/sprite_timeline/timeline.h"
#include "canvas.h"
#include "game.h"
#include "audio.h"
#include "view.h"
#include "depth.h"
u32 __stacksize__=128*1024;
static SmAssets common,scene;static SmVm vm;
SmVm *sm_application_vm=&vm;
static const char *levels[]={"Start","Lyzar","Rybar","Vcely","Strom","Vodarna","Strojovna"};
static const char *directory="sdmc:/3ds/samorost1";
static FILE *logfile;
static void message(const char *text){if(logfile){fprintf(logfile,"%s\n",text);fflush(logfile);}}
static SmSceneData boot_data;
static bool activate(unsigned index,bool outro,SmSceneData *data){
 u64 start=osGetTime();if(scene.bytes)C3D_FrameSync();
 sm_audio_reset();sm_vm_clear(&vm);sm_canvas_unload(false);sm_assets_free(&scene);
 scene=data->assets;memset(&data->assets,0,sizeof data->assets);
 bool uploaded=sm_canvas_upload(&scene,data->pixels,false);u64 uploaded_at=osGetTime();
 if(!uploaded)return false;
 sm_vm_init(&vm,&common,&scene,sm_game_event,sm_game_named,sm_audio_event,NULL);sm_game_init(&vm,index,outro,sm_audio_music);
 if(logfile){fprintf(logfile,"load scene=%s assets_ms=%llu textures_ms=%llu commit_ms=%llu vm_ms=%llu\n",levels[index],(unsigned long long)data->assets_ms,(unsigned long long)data->textures_ms,(unsigned long long)(uploaded_at-start),(unsigned long long)(osGetTime()-uploaded_at));fflush(logfile);}
 char info[256];snprintf(info,sizeof info,"scene=%s outro=%d source_invalid_vertices=%u nodes=%u sensors=%u baked_frames=%u relief_images=%u relief_source=%s",levels[index],outro,scene.invalid_vertices,vm.count,sm_game_sensor_count(),scene.baked.frame_count,scene.depth_count,data->builtin_relief?"embedded":"sd");message(info);return !vm.failed;
}
static bool load(unsigned index,bool outro){
 if(!sm_scene_prepare(&boot_data,directory,levels[index]))return false;
 bool ok=activate(index,outro,&boot_data);sm_scene_dispose(&boot_data);return ok;
}
static void save(unsigned index){char path[512],temp[512];snprintf(path,sizeof path,"%s/save.txt",directory);snprintf(temp,sizeof temp,"%s/save.tmp",directory);FILE *f=fopen(temp,"w");if(!f)return;bool ok=fprintf(f,"SAMOROST1 1 %u\n",index)>0;ok=fclose(f)==0&&ok;if(ok&&rename(temp,path)!=0){remove(path);rename(temp,path);}}
int main(void){
 /* GPU output is RGB8 on both screens; consoleInit switches to RGB565.
    Only initialize the text console after shutting down the GPU on errors. */
 gfxInit(GSP_BGR8_OES,GSP_BGR8_OES,false);gfxSet3D(true);osSetSpeedupEnable(true);
 char path[512],error[256]="Missing game data. Copy samorost1 beside samorost1.3dsx.";snprintf(path,sizeof path,"%s/runtime.log",directory);logfile=fopen(path,"w");message("Samorost native stereo prototype; layers=all_scenes_v2");
 unsigned current=0;snprintf(path,sizeof path,"%s/save.txt",directory);FILE *saved=fopen(path,"r");if(saved){unsigned version,index;if(fscanf(saved,"SAMOROST1 %u %u",&version,&index)==2&&version==1&&index<7)current=index;fclose(saved);}
 sm_scene_loader_init(directory);bool canvas=sm_canvas_init();if(!canvas){snprintf(error,sizeof error,"GPU initialization failed.");goto failed;}
 snprintf(path,sizeof path,"%s/levels/Univerzal.spk",directory);if(!sm_assets_load(&common,path,"Univerzal")||!sm_canvas_load(&common,directory,true))goto failed;
 bool sound=sm_audio_init(directory);if(logfile){fprintf(logfile,"ndsp initial=%08lx final=%08lx hle_retry=%d\n",sm_audio_initial_result(),sm_audio_final_result(),sm_audio_used_hle());fflush(logfile);}message(sound?"DSP streaming enabled":"DSP unavailable; continuing silently");if(!load(current,false))goto failed;
 u64 previous=osGetTime(),report=previous;unsigned frames=0;bool preparing=false,sync_fallback=false;LumeViewport viewport;lume_view_init(&viewport,previous);SmMatrix bottom_presented=sm_bottom_view_matrix(&viewport);touchPosition start={0},touch={160,120};bool dragged=false;
 while(aptMainLoop()){
  hidScanInput();u32 down=hidKeysDown(),held=hidKeysHeld(),up=hidKeysUp();if(down&KEY_START)break;
  u64 now=osGetTime();float dt=(now-previous)*0.001f;previous=now;dt=fminf(0.1f,fmaxf(0,dt));circlePosition circle;hidCircleRead(&circle);
  if(held&KEY_TOUCH){hidTouchRead(&touch);if(down&KEY_TOUCH){start=touch;dragged=false;}if(abs((int)touch.px-start.px)+abs((int)touch.py-start.py)>8)dragged=true;}
  if((down|up)&KEY_TOUCH){float x=touch.px,y=touch.py;if(sm_inverse_point(bottom_presented,&x,&y))sm_game_touch(x,y,!!(down&KEY_TOUCH),dragged);}
  int horizontal=!!(held&KEY_DRIGHT)-!!(held&KEY_DLEFT);
  int direction=horizontal?horizontal:!!(held&KEY_DUP)-!!(held&KEY_DDOWN);
  bool pressed=direction&&(down&(KEY_DUP|KEY_DDOWN|KEY_DLEFT|KEY_DRIGHT));
  if(sm_game_cinematic()||(down&KEY_Y))lume_view_reset(&viewport,now);
  else if(horizontal)lume_view_fast_tick(&viewport,now,direction,pressed,circle.dx,circle.dy);
  else lume_view_tick(&viewport,now,direction,pressed,circle.dx,circle.dy);
  SmMatrix bottom=sm_bottom_view_matrix(&viewport);
  if(sm_game_transition()<0){sm_game_update(dt);sm_tick(&vm,dt);}if(sm_game_cinematic()){lume_view_reset(&viewport,now);bottom=sm_bottom_view_matrix(&viewport);}sm_audio_update();if(vm.failed){snprintf(error,sizeof error,"%s",vm.error);goto failed;}
  int pending=sm_game_pending_transition();
  if(pending>=0&&!preparing&&!sync_fallback){preparing=sm_scene_loader_start(levels[pending]);if(!preparing){sync_fallback=true;message("Scene worker unavailable; synchronous load fallback");}}
  int next=sm_game_transition();
  if(next>=0&&(sync_fallback||sm_scene_loader_poll()!=0)){
   if(!sync_fallback&&sm_scene_loader_poll()<0){
    message("Scene preparation failed; retrying after releasing old scene memory");sm_scene_loader_shutdown();sync_fallback=true;
    C3D_FrameSync();sm_audio_reset();sm_vm_clear(&vm);sm_canvas_unload(false);sm_assets_free(&scene);
   }
   bool outro=next==0&&current==6;bool ok;
   if(sync_fallback)ok=load((unsigned)next,outro);
   else {SmSceneData *data=sm_scene_loader_take();ok=data&&activate((unsigned)next,outro,data);sm_scene_loader_release();}
   if(!ok)goto failed;current=(unsigned)next;save(current);preparing=sync_fallback=false;
   previous=osGetTime();report=previous;frames=0;lume_view_init(&viewport,previous);bottom_presented=sm_bottom_view_matrix(&viewport);continue;
  }
  float slider=osGet3DSliderState();if(sm_canvas_begin()){sm_canvas_draw(&vm,0,sm_game_camera(400,1,0,0),slider);if(slider>0)sm_canvas_draw(&vm,1,sm_game_camera(400,1,0,0),slider);sm_canvas_draw(&vm,2,bottom,0);sm_canvas_end();bottom_presented=bottom;}
  frames++;if(now-report>=5000){if(logfile){fprintf(logfile,"scene=%s fps=%.1f vertices=%u draws=%u c3d_draw_ms=%.2f c3d_process_ms=%.2f audio_active=%u audio_underruns=%u baked_steps=%u interpreted_steps=%u slider=%.2f\n",levels[current],frames*1000.0/(now-report),sm_canvas_vertices(),sm_canvas_draw_calls(),C3D_GetDrawingTime(),C3D_GetProcessingTime(),sm_audio_active(),sm_audio_underruns(),vm.baked_steps,vm.interpreted_steps,slider);fflush(logfile);}frames=0;report=now;}
 }
 goto cleanup;
failed:
 message(error);sm_canvas_close();gfxSet3D(false);consoleInit(GFX_BOTTOM,NULL);printf("Samorost 1\n\n%s\n\nSTART: exit\n",error);while(aptMainLoop()){hidScanInput();if(hidKeysDown()&KEY_START)break;gfxFlushBuffers();gfxSwapBuffers();gspWaitForVBlank();}
cleanup:
 sm_scene_loader_shutdown();sm_scene_dispose(&boot_data);sm_audio_close();sm_canvas_close();sm_assets_free(&scene);sm_assets_free(&common);if(logfile)fclose(logfile);gfxExit();return 0;
}

