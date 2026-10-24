"""Prepare pure property-update frames without converting textures, audio or depth."""
from pathlib import Path
import argparse,hashlib,json,sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'source/engine/tools'))
sys.path.insert(0,str(Path(__file__).resolve().parent))
from bake_animation import build,save
from lvl import parse
from timeline import operations
LEVELS=['Univerzal','Start','Lyzar','Rybar','Vcely','Strom','Vodarna','Strojovna']
def frames_from_sprites(sprites):
    result=[]
    for ident,blob in sorted(sprites.items()):
        movie=operations(blob)
        for index,frame in enumerate(movie['frames']):
            if not frame or frame[-1]['op']!=128 or any(op['op'] not in (2,128) for op in frame):continue
            moves=[]
            for op in frame[:-1]:
                values=[0.0]*15;mask=op['flags']&248
                if mask&8:values[0:2]=op['xy']
                if mask&16:values[2:4]=op['ad']
                if mask&32:values[4:6]=op['bc']
                if mask&64:values[6:14]=op['color'];mask&=~128
                elif mask&128:values[14]=op['alpha']/255.0
                moves.append({'slot':op['depth'],'mask':mask,'values':values})
            result.append({'clip':ident,'frame':index,'pointer':frame[0]['offset'],'next':frame[-1]['end'],'moves':moves})
    return result

def prepare_level(sprites,package):
    package=Path(package);frames=frames_from_sprites(sprites);data=build(package.read_bytes(),frames);target=package.with_suffix('.lbk');save(target,data)
    return {'frames':len(frames),'moves':sum(len(f['moves']) for f in frames),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest()}
def run(game,levels=LEVELS):
    out=ROOT/'out/3ds/samorost1';report={}
    for name in levels:
        source=parse(Path(game)/'Data'/(name+'.lvl'))
        report[name]=prepare_level(source['sprites'],out/'levels'/(name+'.spk'));print(name,report[name],flush=True)
    (ROOT/'logs/baked-animations.json').write_text(json.dumps(report,indent=2)+'\n')
    path=out/'content-manifest.json';manifest=json.loads(path.read_text())
    for name in levels:
        target=out/'levels'/(name+'.lbk');manifest['files'][target.relative_to(out).as_posix()]={'bytes':target.stat().st_size,'sha256':hashlib.sha256(target.read_bytes()).hexdigest()}
    path.write_text(json.dumps(manifest,indent=2)+'\n')
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',default=r'C:/Program Files (x86)/Steam/steamapps/common/Samorost 1');a=p.parse_args();run(a.game)
