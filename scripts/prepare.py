"""Prepare external Samorost assets from the installed game; never executes its EXE."""
from pathlib import Path
import argparse,hashlib,json,os,shutil,struct,subprocess,sys
import numpy as np
import lz4.block
from PIL import Image
sys.path.insert(0,str(Path(__file__).resolve().parent))
from lvl import parse
from timeline import operations,atlas
from atf import prepare
ROOT=Path(__file__).resolve().parents[1]
LEVELS=['Univerzal','Start','Lyzar','Rybar','Vcely','Strom','Vodarna','Strojovna']

def sha(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def tile(image):
 a=np.asarray(image,np.uint8)[:,:,[3,2,1,0]];h,w=a.shape[:2]
 # GPU_RGBA8 uses ABGR bytes and 8x8 Morton tiles. Logical image origin is top-left.
 x=np.arange(w,dtype=np.uint32)[None,:];y=np.arange(h,dtype=np.uint32)[:,None]
 morton=sum(((x>>bit)&1)<<(2*bit) for bit in range(3))+sum(((y>>bit)&1)<<(2*bit+1) for bit in range(3))
 index=((y//8)*(w//8)+x//8)*64+morton
 out=np.empty((w*h,4),np.uint8);out[index.ravel()]=a.reshape(-1,4);return out.tobytes()
def save_texture(image,path):
 width=min(1024,image.width);height=min(1024,image.height)
 if image.size!=(width,height):image=image.resize((width,height),Image.Resampling.LANCZOS)
 raw=tile(image);body=lz4.block.compress(raw,mode='high_compression',compression=9,store_size=False)
 path.write_bytes(struct.pack('<4s4I',b'STX1',width,height,len(raw),len(body))+body)
 return width,height

def package(level,desktop,images,out,work,ffmpeg):
 name=level['level'];folder=out/'textures'/name;folder.mkdir(parents=True,exist_ok=True)
 textures=list(desktop['textures']);texnames={n:i for i,n in enumerate(textures)};sizes={}
 for texture in textures:sizes[texture]=save_texture(images[texture],folder/(texture+'.stx'))
 image_defs={}
 for texture,blob in level['atlas_data'].items():image_defs.update(atlas(blob,texture))
 infos={ident:operations(blob) for ident,blob in level['sprites'].items()}
 scene='Scene'+name;root=level['names'].get(scene,0)
 groups=[level['strings'],textures,image_defs,level['sprites'],level['shapes'],desktop['vertices'],level['sounds'],level['names']]
 data=bytearray(struct.pack('<4s10I',b'SPK1',1,root,*map(len,groups)))
 def u(*values):data.extend(struct.pack('<'+'I'*len(values),*values))
 def text(s):b=s.encode('utf-8');u(len(b));data.extend(b)
 for ident,s in sorted(level['strings'].items()):u(ident);text(s)
 for texture in textures:
  text(texture);u(images[texture].width,images[texture].height,*sizes[texture])
 for ident,record in sorted(image_defs.items()):texture,x,y,w,h=record;u(ident,texnames[texture],x,y,w,h)
 for ident,blob in sorted(level['sprites'].items()):
  frames=infos[ident]['frames'];u(ident,len(blob),len(frames))
  u(*(frame[0]['offset'] for frame in frames));data.extend(blob)
 for ident,(buffer,first,triangles) in sorted(level['shapes'].items()):u(ident,buffer,first,triangles)
 for index,vertices in sorted(desktop['vertices'].items()):
  indices=desktop['indices'][index];u(index,len(vertices)//24,len(indices)//2);data.extend(vertices);data.extend(indices)
 audio=out/'audio'/name;audio.mkdir(parents=True,exist_ok=True)
 for ident,sound in sorted(level['sounds'].items()):
  u(ident);text(sound['name']);mp3=work/(str(ident)+'.mp3');target=audio/(str(ident)+'.pcm')
  mp3.write_bytes(sound['data'])
  original=hashlib.sha256(sound['data']).hexdigest();stamp=target.with_suffix('.json')
  if not target.is_file() or not stamp.is_file() or json.loads(stamp.read_text()).get('input')!=original:
   raw=work/(str(ident)+'.pcm')
   subprocess.run([str(ffmpeg),'-v','error','-y','-i',str(mp3),'-f','s16le','-acodec','pcm_s16le','-ar','32000','-ac','2',str(raw)],check=True,stdout=subprocess.DEVNULL,stderr=subprocess.PIPE)
   samples=raw.read_bytes();target.write_bytes(struct.pack('<4sIHHI',b'PCM1',32000,2,16,len(samples)//4)+samples);stamp.write_text(json.dumps({'input':original}))
 for n,ident in sorted(level['names'].items()):u(ident);text(n)
 (out/'levels').mkdir(exist_ok=True);(out/'levels'/(name+'.spk')).write_bytes(data)
 from bake_animations import prepare_level
 prepared_animation=prepare_level(level['sprites'],out/'levels'/(name+'.spk'))
 return {'baked_animation':prepared_animation,'root':root,'sprites':len(infos),'frames':sum(i['total'] for i in infos.values()),'images':len(image_defs),'shapes':len(level['shapes']),'textures':len(textures),'sounds':len(level['sounds']),'package_bytes':len(data)}

def run(game,levels):
 game=Path(game).resolve();source=game/'Data'
 if sha(source/'Main.swf')!='0ba23a7ca9c0d401a7439188c2407be5953e12a0c3ecbca5b792846aa0ff5b03':raise ValueError('Unsupported Samorost 1 game version: scene rules must be reviewed for this Main.swf')
 out=ROOT/'out/3ds/samorost1';out.mkdir(parents=True,exist_ok=True);work=ROOT/'work/assets';work.mkdir(parents=True,exist_ok=True)
 ffmpeg=shutil.which('ffmpeg')
 if not ffmpeg:raise ValueError('An existing FFmpeg installation is required')
 report={}
 for name in levels:
  print('Preparing',name,flush=True);level=parse(source/(name+'.lvl'));desktop=parse(source/'desktop'/(name+'.lvl'))
  cache=work/name;cache.mkdir(exist_ok=True);stamp=cache/'textures.json'
  key=hashlib.sha256((desktop['sha256']+sha(Path(__file__).with_name('atf.py'))+sha(Path(__file__).with_name('decode_jxr.ps1'))).encode()).hexdigest()
  if stamp.is_file() and json.loads(stamp.read_text()).get('key')==key:
   images={n:Image.open(cache/(n+'.png')).convert('RGBA') for n in desktop['textures']}
  else:
   images=prepare(desktop['textures'],cache);stamp.write_text(json.dumps({'key':key}))
  report[name]=package(level,desktop,images,out,cache,ffmpeg);print(name,report[name],flush=True)
 (ROOT/'logs').mkdir(exist_ok=True);(ROOT/'logs/assets.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
 files={p.relative_to(out).as_posix():{'bytes':p.stat().st_size,'sha256':sha(p)} for p in out.rglob('*') if p.is_file() and p.suffix!='.json' and p.name not in {'save.txt','runtime.log'}}
 (out/'content-manifest.json').write_text(json.dumps({'source':str(game),'files':files},indent=2),encoding='utf-8')
 print('External content ready:',out,flush=True)
 return report
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--game',default=r'C:\Program Files (x86)\Steam\steamapps\common\Samorost 1');p.add_argument('--level',action='append',choices=LEVELS);args=p.parse_args();run(args.game,args.level or LEVELS)