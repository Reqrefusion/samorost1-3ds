#ifndef SAMOROST_AUDIO_H
#define SAMOROST_AUDIO_H
#include "engine/adapters/sprite_timeline/timeline.h"
bool sm_audio_init(const char*);
void sm_audio_close(void);
void sm_audio_update(void);
void sm_audio_reset(void);
void sm_audio_music(const char*,float,bool);
void sm_audio_event(SmVm*,SmAssets*,unsigned,float,float,void*);
unsigned sm_audio_underruns(void);
unsigned sm_audio_active(void);
unsigned long sm_audio_initial_result(void);
unsigned long sm_audio_final_result(void);
bool sm_audio_used_hle(void);
#endif
