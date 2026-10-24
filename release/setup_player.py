"""Convert the owner's installed Samorost 1 data; use prepared 3D maps and fixed app."""
from pathlib import Path
import argparse,contextlib,hashlib,json,os,shutil,sys
ROOT=Path(__file__).resolve().parents[1]
MAIN_SHA='0ba23a7ca9c0d401a7439188c2407be5953e12a0c3ecbca5b792846aa0ff5b03'
sys.path.insert(0,str(ROOT/'scripts'));sys.path.insert(0,str(ROOT/'release'))
from runtime_manifest import digest,verify_runtime

def find_original(explicit=None):
 candidates=[Path(explicit)] if explicit else [Path(os.environ.get('ProgramFiles(x86)',r'C:/Program Files (x86)'))/'Steam/steamapps/common/Samorost 1']
 for folder in candidates:
  if folder.is_file():folder=folder.parent
  main=folder/'Data/Main.swf'
  if main.is_file() and digest(main)==MAIN_SHA:return folder.resolve()
 raise ValueError('Supported Samorost 1 folder not found. Select the installed game folder.')

def prepared_maps():
 metadata=json.loads((ROOT/'maps/manifest.json').read_text())
 for name,item in metadata['files'].items():
  path=(ROOT/name).resolve()
  if not path.is_relative_to((ROOT/'maps').resolve()) or digest(path)!=item['sha256'] or path.stat().st_size!=item['bytes']:raise ValueError('Prepared map verification failed: '+name)
 return metadata['files']

def ffmpeg_path(explicit=None):
 if explicit:
  path=Path(explicit).resolve()
  if not path.is_file():raise ValueError('FFmpeg does not exist')
  return path
 installed=shutil.which('ffmpeg')
 if installed:return Path(installed)
 cache=ROOT/'tools/ffmpeg';hits=list(cache.rglob('ffmpeg.exe'))
 if hits:return hits[0]
 from toolchain import fetch_json,download,unzip
 metadata=fetch_json('https://api.github.com/repos/BtbN/FFmpeg-Builds/releases/tags/latest')
 asset=next(a for a in metadata['assets'] if a['name']=='ffmpeg-n8.1-latest-win64-lgpl-shared-8.1.zip')
 expected=asset.get('digest','').removeprefix('sha256:')
 archive=download(asset['browser_download_url'],ROOT/'tools/downloads'/asset['name'],expected);unzip(archive,cache)
 hits=list(cache.rglob('ffmpeg.exe'))
 if len(hits)!=1:raise ValueError('Ambiguous FFmpeg download')
 return hits[0]

def publish(app,content,output,files):
 content,output=Path(content).resolve(),Path(output).resolve();game=output/'3ds/samorost1';game.mkdir(parents=True,exist_ok=True)
 for name,item in files.items():
  relative=Path(name)
  if relative.is_absolute() or '..' in relative.parts or relative.name in {'save.txt','runtime.log'}:raise ValueError('Invalid content entry')
  source=content/relative;target=game/relative
  if not source.is_file() or digest(source)!=item['sha256']:raise ValueError('Converted content mismatch: '+name)
  if source.resolve()!=target.resolve():
   target.parent.mkdir(parents=True,exist_ok=True);temporary=target.with_name(target.name+'.tmp');shutil.copyfile(source,temporary);temporary.replace(target)
 binary=output/'3ds/samorost1.3dsx'
 if Path(app).resolve()!=binary.resolve():shutil.copyfile(app,binary)
 return binary

def run(args):
 original=find_original(args.game);app=verify_runtime(ROOT);maps=prepared_maps();ffmpeg=ffmpeg_path(args.ffmpeg)
 os.environ['PATH']=str(ffmpeg.parent)+os.pathsep+os.environ.get('PATH','')
 # Player setup cannot infer depth or compile native code.
 class NoInference:
  def find_spec(self,fullname,path=None,target=None):
   if fullname.split('.')[0] in {'onnxruntime','torch','depth_estimator','depth_filter'}:raise RuntimeError('Player setup must use the prepared maps')
 sys.meta_path.insert(0,NoInference())
 from prepare import run as convert,LEVELS
 print('Preparing original artwork, animations and audio...',flush=True);convert(original,LEVELS)
 content=ROOT/'out/3ds/samorost1'
 for name in maps:
  target=content/'depth'/Path(name).name;target.parent.mkdir(exist_ok=True);shutil.copyfile(ROOT/name,target)
 manifest_path=content/'content-manifest.json';manifest=json.loads(manifest_path.read_text())
 for name,item in maps.items():manifest['files']['depth/'+Path(name).name]=item
 manifest['files']={n:v for n,v in manifest['files'].items() if Path(n).name not in {'save.txt','runtime.log'}}
 manifest_path.write_text(json.dumps(manifest,indent=2)+'\n')
 game=publish(app,content,args.output,manifest['files'])
 print('READY:',game,flush=True);print('Copy out/3ds to the SD card. Preserve saves in /3ds/samorost1/.',flush=True)
 return game
if __name__=='__main__':
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game');p.add_argument('--ffmpeg');p.add_argument('--output',default=str(ROOT/'out'));a=p.parse_args()
 logs=ROOT/'logs';logs.mkdir(exist_ok=True)
 class Tee:
  def __init__(self,console,log):self.console,self.log=console,log
  def write(self,text):self.console.write(text);self.log.write(text);self.flush();return len(text)
  def flush(self):self.console.flush();self.log.flush()
 with (logs/'player-setup.log').open('a',encoding='utf-8') as log,contextlib.redirect_stdout(Tee(sys.stdout,log)),contextlib.redirect_stderr(Tee(sys.stderr,log)):
  try:run(a)
  except Exception:
   import traceback;traceback.print_exc();raise SystemExit(1)
