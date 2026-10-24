#include "view.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>

void sm_test_view(void) {
  LumeViewport v;
  lume_view_init(&v,0);
  assert(v.scale==1024);
  lume_view_tick(&v,0,1,true,0,0);
  assert(v.target==1536&&v.scale==1024);
  for(unsigned t=50;t<=200;t+=50)lume_view_tick(&v,t,0,false,0,0);
  assert(v.scale==1536);
  lume_view_fast_tick(&v,201,1,true,0,0);
  assert(v.scale==2048);
  SmMatrix presented=sm_bottom_view_matrix(&v);
  float x=900,y=700;
  float screen_x=presented.a*x+presented.c*y+presented.x;
  float screen_y=presented.b*x+presented.d*y+presented.y;
  lume_view_tick(&v,251,0,false,120,-100);
  SmMatrix upcoming=sm_bottom_view_matrix(&v);
  assert(upcoming.x!=presented.x&&upcoming.y!=presented.y);
  assert(sm_inverse_point(presented,&screen_x,&screen_y));
  assert(fabsf(screen_x-x)<0.001f&&fabsf(screen_y-y)<0.001f);
  v.cx=v.cy=0;lume_view_clamp(&v);
  assert(v.cx==80*1024&&v.cy==60*1024);
  v.cx=320*1024;v.cy=240*1024;lume_view_clamp(&v);
  assert(v.cx==240*1024&&v.cy==180*1024);
  lume_view_reset(&v,300);
  assert(v.scale==1024&&v.cx==160*1024&&v.cy==120*1024);
  puts("PASS shared smooth/instant zoom, lower-camera bounds, presented touch inverse and reset");
}
