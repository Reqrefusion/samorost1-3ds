#include "depth.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned u32(const unsigned char *p){return (unsigned)p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
bool sm_depth_load(SmAssets *assets,const char *directory){
 char path[512];snprintf(path,sizeof path,"%s/depth/%s.smd",directory,assets->name);FILE *f=fopen(path,"rb");if(!f)return true;
 unsigned char header[16];bool ok=fread(header,1,16,f)==16&&!memcmp(header,"SMD1",4)&&u32(header+4)==1&&u32(header+8)==SM_DEPTH_GRID;
 unsigned count=ok?u32(header+12):0;if(count>assets->image_count)ok=false;
 for(unsigned i=0;ok&&i<count;i++){
  unsigned char record[4+SM_DEPTH_GRID*SM_DEPTH_GRID];if(fread(record,1,sizeof record,f)!=sizeof record){ok=false;break;}
  SmImage *image=sm_image(assets,u32(record));if(!image||image->relief){ok=false;break;}
  image->relief=malloc(SM_DEPTH_GRID*SM_DEPTH_GRID);if(!image->relief){ok=false;break;}memcpy(image->relief,record+4,SM_DEPTH_GRID*SM_DEPTH_GRID);assets->depth_count++;
 }
 if(fgetc(f)!=EOF)ok=false;fclose(f);
 if(!ok){for(unsigned i=0;i<assets->image_count;i++){free(assets->images[i].relief);assets->images[i].relief=NULL;}assets->depth_count=0;}
 return ok;
}

#include "depth_builtin.inc"
bool sm_depth_builtin(SmAssets *assets){
 if(assets->depth_count)return true;
 for(unsigned i=0;i<sizeof builtin_depth/sizeof builtin_depth[0];i++)if(!strcmp(assets->name,builtin_depth[i].name)){
  const unsigned char *data=builtin_depth[i].bytes;unsigned count=u32(data+12);
  if(builtin_depth[i].size!=16+count*(4+SM_DEPTH_GRID*SM_DEPTH_GRID)||count>assets->image_count)return false;
  for(unsigned n=0;n<count;n++){
   const unsigned char *record=data+16+n*(4+SM_DEPTH_GRID*SM_DEPTH_GRID);SmImage *image=sm_image(assets,u32(record));
   if(!image||image->relief)goto fail;
   image->relief=malloc(SM_DEPTH_GRID*SM_DEPTH_GRID);if(!image->relief)goto fail;
   memcpy(image->relief,record+4,SM_DEPTH_GRID*SM_DEPTH_GRID);assets->depth_count++;
  }
  return true;
 }
 return false;
fail:for(unsigned n=0;n<assets->image_count;n++){free(assets->images[n].relief);assets->images[n].relief=NULL;}assets->depth_count=0;return false;
}
