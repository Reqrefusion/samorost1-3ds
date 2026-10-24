#ifndef SAMOROST_STEREO_H
#define SAMOROST_STEREO_H
#include <stdint.h>
static inline float sm_stereo_eye(unsigned view) {
  return view==0?3.0f:view==1?-3.0f:0.0f;
}
static inline float sm_stereo_surface(unsigned view,float slider,float plane,int8_t relief) {
  if(slider<0)slider=0;if(slider>1)slider=1;
  float sample=relief< -127?-1.0f:relief/127.0f;
  return sm_stereo_eye(view)*slider*(plane+0.70f*sample);
}
#endif
