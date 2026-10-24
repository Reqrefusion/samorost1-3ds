#include "canvas.h"
#include "depth.h"
#include "stereo.h"
#include "engine/adapters/sprite_timeline/stroke.h"
#include "layers.h"
#include "engine/render/texture_filter.h"
#include "game.h"
#include <lz4.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/* Reuse the SDK's existing Citro2D shader binary.
   Uniform names and attribute layout are checked at startup. No shader tools
   or game-specific shader compilation are required by the player. */
extern const uint8_t render2d_shbin[];
extern const uint32_t render2d_shbin_size;
#define VERTEX_LIMIT 120000u
#define TRANSFER_FLAGS (GX_TRANSFER_FLIP_VERT(0)|GX_TRANSFER_OUT_TILED(0)|GX_TRANSFER_RAW_COPY(0)|GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8)|GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8)|GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))
typedef struct {float x,y,z,u,v;uint8_t color[4];} Vertex;
static C3D_RenderTarget *targets[3];static C3D_Tex textures[2][SM_TEXTURES],white;
static SmAssets *owners[2];static DVLB_s *shader;static shaderProgram_s program;
static Vertex *vertices;static unsigned used;static int projection_loc,model_loc;static bool initialized;
static unsigned batch_start,batch_count,draw_calls;static C3D_Tex *batch_texture;static float batch_offset[4];
static float eye_shift,slider_strength,relief_strength;static unsigned active_view;static uint32_t random_state=0x123987u;
static uint8_t byte(float value){return value<=0?0:value>=1?255:(uint8_t)(value*255.0f+0.5f);}
static uint32_t color4(const float *c){return (uint32_t)byte(c[0])|(uint32_t)byte(c[1])<<8|(uint32_t)byte(c[2])<<16|(uint32_t)byte(c[3])<<24;}
static void transform(SmMatrix m,float x,float y,float *outx,float *outy){*outx=m.a*x+m.c*y+m.x;*outy=m.b*x+m.d*y+m.y;}
void sm_canvas_unload(bool common){unsigned slot=common?0:1;for(unsigned i=0;i<SM_TEXTURES;i++)if(textures[slot][i].data){C3D_TexDelete(&textures[slot][i]);memset(&textures[slot][i],0,sizeof textures[slot][i]);}owners[slot]=NULL;}
void sm_canvas_close(void){
 if(!initialized)return;sm_canvas_unload(true);sm_canvas_unload(false);
 for(unsigned i=0;i<3;i++)if(targets[i]){C3D_RenderTargetDelete(targets[i]);targets[i]=NULL;}
 if(white.data)C3D_TexDelete(&white);if(vertices)linearFree(vertices);vertices=NULL;
 shaderProgramFree(&program);if(shader)DVLB_Free(shader);shader=NULL;C3D_Fini();initialized=false;
}
bool sm_canvas_init(void){
 if(gfxGetScreenFormat(GFX_TOP)!=GSP_BGR8_OES||gfxGetScreenFormat(GFX_BOTTOM)!=GSP_BGR8_OES)return false;
 if(!C3D_Init(0x100000))return false;initialized=true;
 shader=DVLB_ParseFile((uint32_t*)render2d_shbin,render2d_shbin_size);if(!shader)goto fail;
 shaderProgramInit(&program);shaderProgramSetVsh(&program,&shader->DVLE[0]);
 projection_loc=shaderInstanceGetUniformLocation(program.vertexShader,"projMtx");model_loc=shaderInstanceGetUniformLocation(program.vertexShader,"mdlvMtx");if(projection_loc<0||model_loc<0)goto fail;
 for(unsigned i=0;i<3;i++){targets[i]=C3D_RenderTargetCreate(240,i==2?320:400,GPU_RB_RGBA8,GPU_RB_DEPTH24_STENCIL8);if(!targets[i])goto fail;C3D_RenderTargetSetOutput(targets[i],i==2?GFX_BOTTOM:GFX_TOP,i==1?GFX_RIGHT:GFX_LEFT,TRANSFER_FLAGS);}
 vertices=linearAlloc(VERTEX_LIMIT*sizeof *vertices);if(!vertices)goto fail;
 if(!C3D_TexInit(&white,8,8,GPU_RGBA8))goto fail;memset(white.data,255,8*8*4);C3D_TexFlush(&white);C3D_TexSetFilter(&white,GPU_NEAREST,GPU_NEAREST);
 return true;
fail:sm_canvas_close();return false;
}
bool sm_canvas_upload(SmAssets *assets,SmTexturePixels *pixels,bool common){
 unsigned slot=common?0:1;sm_canvas_unload(common);owners[slot]=assets;
 for(unsigned i=0;i<assets->texture_count;i++){
  SmTexturePixels *p=pixels+i;C3D_Tex *texture=&textures[slot][i];
  if(!p->data||p->width!=assets->textures[i].w||p->height!=assets->textures[i].h)goto fail;
  unsigned levels=p->mip_levels;
  if(!C3D_TexInitWithParams(texture,NULL,(C3D_TexInitParams){p->width,p->height,levels,GPU_RGBA8,GPU_TEX_2D,false})){
   levels=0;if(!C3D_TexInit(texture,p->width,p->height,GPU_RGBA8))goto fail;
  }
  memcpy(texture->data,p->data,sm_mip_size(p->width*p->height*4,levels));
  C3D_TexSetFilter(texture,GPU_LINEAR,GPU_LINEAR);C3D_TexSetFilterMipmap(texture,GPU_LINEAR);
  C3D_TexSetWrap(texture,GPU_CLAMP_TO_EDGE,GPU_CLAMP_TO_EDGE);C3D_TexFlush(texture);
 }
 return true;
fail:sm_canvas_unload(common);return false;
}
bool sm_canvas_load(SmAssets *assets,const char *directory,bool common){
 SmTexturePixels pixels[SM_TEXTURES];if(!sm_texture_pixels_load(assets,directory,pixels))return false;
 bool ok=sm_canvas_upload(assets,pixels,common);sm_texture_pixels_free(pixels);return ok;
}
static void configure_shader(void){
 C3D_BindProgram(&program);C3D_AttrInfo attr;AttrInfo_Init(&attr);AttrInfo_AddLoader(&attr,0,GPU_FLOAT,3);AttrInfo_AddLoader(&attr,1,GPU_FLOAT,2);AttrInfo_AddLoader(&attr,3,GPU_UNSIGNED_BYTE,4);int fixed=AttrInfo_AddFixed(&attr,2);C3D_SetAttrInfo(&attr);C3D_FixedAttribSet(fixed,0,0,0,0);
 C3D_BufInfo *buf=C3D_GetBufInfo();BufInfo_Init(buf);BufInfo_Add(buf,vertices,sizeof *vertices,3,0x210);
 C3D_Mtx identity;Mtx_Identity(&identity);C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER,model_loc,&identity);
 C3D_CullFace(GPU_CULL_NONE);C3D_DepthTest(false,GPU_ALWAYS,GPU_WRITE_COLOR);
 C3D_AlphaBlend(GPU_BLEND_ADD,GPU_BLEND_ADD,GPU_SRC_ALPHA,GPU_ONE_MINUS_SRC_ALPHA,GPU_ONE,GPU_ONE_MINUS_SRC_ALPHA);
 C3D_AlphaTest(true,GPU_GREATER,0);
}
bool sm_canvas_begin(void){if(!initialized||!C3D_FrameBegin(C3D_FRAME_SYNCDRAW))return false;used=batch_count=draw_calls=0;batch_texture=NULL;configure_shader();return true;}
static void flush_batch(void){if(batch_count){C3D_DrawArrays(GPU_TRIANGLES,batch_start,batch_count);draw_calls++;batch_count=0;}}
static void environment(C3D_Tex *texture,const float *offset){
 if(batch_texture==texture&&!memcmp(batch_offset,offset,sizeof batch_offset))return;
 flush_batch();batch_texture=texture;memcpy(batch_offset,offset,sizeof batch_offset);batch_start=used;
 C3D_TexBind(0,texture);for(unsigned i=0;i<6;i++)C3D_TexEnvInit(C3D_GetTexEnv((int)i));
 C3D_TexEnv *env=C3D_GetTexEnv(0);C3D_TexEnvSrc(env,C3D_Both,GPU_TEXTURE0,GPU_PRIMARY_COLOR,0);C3D_TexEnvFunc(env,C3D_Both,GPU_MODULATE);
 float positive[4],negative[4];bool pos=false,neg=false;
 for(unsigned i=0;i<4;i++){positive[i]=fmaxf(0,offset[i]);negative[i]=fmaxf(0,-offset[i]);pos|=positive[i]>0;neg|=negative[i]>0;}
 if(pos){env=C3D_GetTexEnv(1);C3D_TexEnvSrc(env,C3D_Both,GPU_PREVIOUS,GPU_CONSTANT,0);C3D_TexEnvFunc(env,C3D_Both,GPU_ADD);C3D_TexEnvColor(env,color4(positive));}
 if(neg){env=C3D_GetTexEnv(2);C3D_TexEnvSrc(env,C3D_Both,GPU_PREVIOUS,GPU_CONSTANT,0);C3D_TexEnvFunc(env,C3D_Both,GPU_SUBTRACT);C3D_TexEnvColor(env,color4(negative));}
}
static Vertex vertex(SmMatrix m,float x,float y,float u,float v,const float *color){Vertex result;transform(m,x,y,&result.x,&result.y);result.z=0.5f;result.u=u;result.v=v;for(unsigned i=0;i<4;i++)result.color[i]=byte(color[i]);return result;}
/* Preserve subpixel wires in both eyes. Only long, slender rectangles qualify;
   the midpoint and UVs stay fixed, and zoomed-in artwork is unchanged. */
static SmMatrix thin_matrix(SmMatrix m,float w,float h){
 float x=hypotf(m.a*w,m.b*w),y=hypotf(m.c*h,m.d*h);
 float area=fabsf((m.a*m.d-m.b*m.c)*w*h),longest=fmaxf(x,y);
 float thickness=longest>0?area/longest:0;
 if(longest>=8&&thickness>0.001f&&thickness<1.10f&&longest/thickness>=12){
  float gain=1.10f/thickness;
  if(x<y){float a=m.a,b=m.b;m.a*=gain;m.b*=gain;m.x+=(a-m.a)*w*0.5f;m.y+=(b-m.b)*w*0.5f;}
  else{float c=m.c,d=m.d;m.c*=gain;m.d*=gain;m.x+=(c-m.c)*h*0.5f;m.y+=(d-m.d)*h*0.5f;}
 }
 return m;
}
static void quad(SmMatrix m,float w,float h,float left,float top,float right,float bottom,const float *color,const float *offset,C3D_Tex *tex){
 if(used+6>VERTEX_LIMIT)return;environment(tex,offset);
 vertices[used++]=vertex(m,0,0,left,top,color);vertices[used++]=vertex(m,w,0,right,top,color);vertices[used++]=vertex(m,0,h,left,bottom,color);
 vertices[used++]=vertex(m,w,0,right,top,color);vertices[used++]=vertex(m,w,h,right,bottom,color);vertices[used++]=vertex(m,0,h,left,bottom,color);
 batch_count+=6;
}
static void relief_quad(SmMatrix m,SmImage *image,SmTexture *tex,const float *color,const float *offset,C3D_Tex *texture){
 const unsigned side=SM_DEPTH_GRID,cells=side-1,count=cells*cells*6;
 if(used+count>VERTEX_LIMIT)return;environment(texture,offset);Vertex grid[SM_DEPTH_GRID*SM_DEPTH_GRID];
 for(unsigned y=0;y<side;y++)for(unsigned x=0;x<side;x++){
  unsigned i=y*side+x;float px=(float)image->w*x/cells,py=(float)image->h*y/cells;
  grid[i]=vertex(m,px,py,(image->x+px)/tex->original_w,1-(image->y+py)/tex->original_h,color);
  grid[i].x+=relief_strength*sm_stereo_surface(active_view,slider_strength,0,image->relief[i]);
 }
 for(unsigned y=0;y<cells;y++)for(unsigned x=0;x<cells;x++){
  unsigned i=y*side+x;vertices[used++]=grid[i];vertices[used++]=grid[i+1];vertices[used++]=grid[i+side];
  vertices[used++]=grid[i+1];vertices[used++]=grid[i+side+1];vertices[used++]=grid[i+side];
 }
 batch_count+=count;
}
/* Four shared rails: transparent edge, two body edges, transparent edge.
   Each segment shares its rail vertices, so bends do not create alpha seams. */
static void stroke_run(SmMesh *mesh,const SmStrokeRun *run,SmMatrix matrix,const float *color){
 unsigned count=(run->pairs-1)*18;if(used+count>VERTEX_LIMIT)return;
 Vertex rails[1024];
 for(unsigned i=0;i<run->pairs;i++){
  const float *a=mesh->vertices+(run->first+i*2)*6,*b=a+6;float ax,ay,bx,by;
  sm_stroke_pair(matrix,a,b,&ax,&ay,&bx,&by);
  float dx=bx-ax,dy=by-ay,width=hypotf(dx,dy),nx=width>0.00001f?dx/width:0,ny=width>0.00001f?dy/width:0;
  for(unsigned rail=0;rail<4;rail++){
   bool right=rail>=2;const float *p=right?b:a;float shaded[4];
   for(unsigned c=0;c<4;c++)shaded[c]=p[c+2]*color[c]+color[c+4];
   if(rail==0||rail==3)shaded[3]=0;
   Vertex v=vertex(matrix,p[0],p[1],0.5f,0.5f,shaded);
   float fringe=rail==0?-0.65f:rail==3?0.65f:0;
   v.x=(right?bx:ax)+nx*fringe;v.y=(right?by:ay)+ny*fringe;rails[i*4+rail]=v;
  }
 }
 for(unsigned i=0;i+1<run->pairs;i++)for(unsigned band=0;band<3;band++){
  unsigned a=i*4+band,b=a+1,c=a+4,d=c+1;
  vertices[used++]=rails[a];vertices[used++]=rails[c];vertices[used++]=rails[b];
  vertices[used++]=rails[b];vertices[used++]=rails[c];vertices[used++]=rails[d];
 }
 batch_count+=count;
}
static void tree(SmVm *v,int id,SmMatrix parent,const float *parent_color,SmLayerState layer,unsigned recursion){
 if(id<0||id>=SM_NODES||recursion>64)return;SmNode *n=v->nodes+id;if(!n->alive||!n->visible||n->alpha<=0)return;
 SmMatrix matrix=sm_multiply(parent,n->transform);float color[8];
 for(unsigned i=0;i<4;i++){color[i]=parent_color[i]*n->color[i];color[i+4]=parent_color[i]*n->color[i+4]+parent_color[i+4];}color[3]*=n->alpha;color[7]*=n->alpha;
 layer=sm_layer_resolve(v,id,layer);
 if(n->kind==1){for(unsigned i=0;i<n->children_count;i++)tree(v,n->children[i],matrix,color,layer,recursion+1);return;}
 matrix.x+=sm_stereo_surface(active_view,slider_strength,layer.plane,0);
 if(n->kind==0){SmImage *image=sm_image(n->assets,n->id);if(!image)return;unsigned slot=owners[0]==n->assets?0:1;SmTexture *tex=n->assets->textures+image->texture;
  matrix=thin_matrix(matrix,(float)image->w,(float)image->h);
  relief_strength=image->relief&&slider_strength>0&&layer.relief>0?layer.relief*sm_layer_relief_visibility(matrix,image->w,image->h):0;
  if(image->relief&&slider_strength>0&&relief_strength>0)relief_quad(matrix,image,tex,color,color+4,&textures[slot][image->texture]);
  else quad(matrix,(float)image->w,(float)image->h,(float)image->x/tex->original_w,1-(float)image->y/tex->original_h,(float)(image->x+image->w)/tex->original_w,1-(float)(image->y+image->h)/tex->original_h,color,color+4,&textures[slot][image->texture]);
 }else if(n->kind==3){quad(matrix,n->width,n->height,0,1,1,0,color,color+4,&white);
 }else if(n->kind==2){SmShape *shape=sm_shape(n->assets,n->id);if(!shape)return;SmMesh *mesh=n->assets->mesh+shape->buffer;float offset[4]={0};environment(&white,offset);
  if(!shape->stroke_count&&shape->triangles==2){
   float minx=INFINITY,miny=INFINITY,maxx=-INFINITY,maxy=-INFINITY;
   for(unsigned i=0;i<6;i++){float *p=mesh->vertices+mesh->index[shape->first+i]*6;if(!isfinite(p[0])||!isfinite(p[1]))continue;minx=fminf(minx,p[0]);miny=fminf(miny,p[1]);maxx=fmaxf(maxx,p[0]);maxy=fmaxf(maxy,p[1]);}
   if(maxx>minx&&maxy>miny){SmMatrix local=matrix;local.x+=local.a*minx+local.c*miny;local.y+=local.b*minx+local.d*miny;local=thin_matrix(local,maxx-minx,maxy-miny);local.x-=local.a*minx+local.c*miny;local.y-=local.b*minx+local.d*miny;matrix=local;}
  }
  unsigned shape_start=used,run=0;for(unsigned t=0;t<shape->triangles;t++){
   if(run<shape->stroke_count&&t==shape->strokes[run].triangle){
    batch_count+=used-shape_start;stroke_run(mesh,shape->strokes+run,matrix,color);shape_start=used;
    t+=shape->strokes[run].triangles-1;run++;continue;
   }
   if(used+3>VERTEX_LIMIT)break;unsigned first=shape->first+t*3;float *p[3];bool valid=true;for(unsigned i=0;i<3;i++){p[i]=mesh->vertices+mesh->index[first+i]*6;valid&=isfinite(p[i][0])&&isfinite(p[i][1]);}if(!valid)continue;
   for(unsigned i=0;i<3;i++){float shaded[4];for(unsigned c=0;c<4;c++)shaded[c]=p[i][c+2]*color[c]+color[c+4];vertices[used++]=vertex(matrix,p[i][0],p[i][1],0.5f,0.5f,shaded);}
  }batch_count+=used-shape_start;
 }
}
void sm_canvas_rect(float x,float y,float w,float h,uint32_t rgba){float color[4]={(rgba&255)/255.0f,((rgba>>8)&255)/255.0f,((rgba>>16)&255)/255.0f,(rgba>>24)/255.0f},offset[4]={0};SmMatrix matrix=sm_identity();matrix.x=x;matrix.y=y;quad(matrix,w,h,0,1,1,0,color,offset,&white);}
void sm_canvas_draw(SmVm *v,unsigned view,SmMatrix camera,float strength){
 if(view>=3)return;flush_batch();batch_texture=NULL;C3D_RenderTargetClear(targets[view],C3D_CLEAR_ALL,0x04070bff,0);C3D_FrameDrawOn(targets[view]);C3D_Mtx projection;Mtx_OrthoTilt(&projection,0,view==2?320:400,240,0,0,1,true);C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER,projection_loc,&projection);
 active_view=view;eye_shift=sm_stereo_eye(view);slider_strength=strength;
 float star_width=view==2?320.0f:400.0f,motion[3];for(unsigned band=0;band<3;band++)motion[band]=fmodf(sm_game_space_phase()*(1.15f-0.30f*band),star_width);
 random_state=0x123987u;for(unsigned i=0;i<72;i++){random_state=random_state*1664525u+1013904223u;float x=(float)(random_state%(view==2?320u:400u));random_state=random_state*1664525u+1013904223u;float y=(float)(random_state%240);unsigned band=i%3;
  x+=motion[band];if(x<0)x+=star_width;else if(x>=star_width)x-=star_width;
  float plane=-0.90f-0.30f*band;uint32_t brightness=band==0?0xffa0a0a0:band==1?0xff808080:0xff606060;
  sm_canvas_rect(x+eye_shift*strength*plane,y,1,1,brightness);}
 float color[8]={1,1,1,1,0,0,0,0};SmLayerState layer={0,1,SM_LAYER_NONE};tree(v,v->root,camera,color,layer,0);
 // Match the authored window on both eyes; no scene fragments enter the bars.
 float width=view==2?320:400,left=fmaxf(0,fminf(width,camera.x)),right=fmaxf(0,fminf(width,camera.x+1900*camera.a));
 float top=fmaxf(0,fminf(240,camera.y)),bottom=fmaxf(0,fminf(240,camera.y+1500*camera.d));
 if(left>0)sm_canvas_rect(0,0,left,240,0xff000000);
 if(right<width)sm_canvas_rect(right,0,width-right,240,0xff000000);
 if(top>0)sm_canvas_rect(0,0,width,top,0xff000000);
 if(bottom<240)sm_canvas_rect(0,bottom,width,240-bottom,0xff000000);
}
void sm_canvas_end(void){flush_batch();GSPGPU_FlushDataCache(vertices,used*sizeof *vertices);C3D_FrameEnd(GX_CMDLIST_FLUSH);}
unsigned sm_canvas_vertices(void){return used;}
unsigned sm_canvas_draw_calls(void){return draw_calls;}
