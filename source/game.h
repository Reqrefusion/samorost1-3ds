#ifndef SAMOROST_GAME_H
#define SAMOROST_GAME_H
#ifdef __cplusplus
extern "C" {
#endif
#include "engine/adapters/sprite_timeline/timeline.h"
typedef void (*SmMusic)(const char *name,float gain,bool stop);
void sm_game_init(SmVm*,unsigned,bool,SmMusic);
void sm_game_event(SmVm*,int,const char*,void*);
void sm_game_named(SmVm*,int,void*);
void sm_game_touch(float,float,bool,bool);
void sm_game_update(float);
int sm_game_transition(void);
int sm_game_pending_transition(void);
bool sm_game_cinematic(void);
float sm_game_space_phase(void);
SmMatrix sm_game_camera(unsigned,float,float,float);
unsigned sm_game_sensor_count(void);
bool sm_game_click_named(const char*,bool);
#ifdef __cplusplus
}
#endif
#endif
