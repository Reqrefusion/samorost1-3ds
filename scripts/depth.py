"""Generate small, stable relief grids with the existing Lume depth model.

Only substantial static images or images reused unchanged by a timeline receive
relief in this first pass. Animated pose sequences keep their shared layer depth.
"""
from pathlib import Path
import argparse,hashlib,json,struct,sys
import numpy as np
import cv2
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from lvl import parse
from timeline import operations,atlas
sys.path.insert(0,str(ROOT/"source/engine/tools"))
from depth_filter import shape_grid

def make_grid(rgba,stereo):
    rgba=np.asarray(rgba);alpha=rgba[:,:,3].astype(np.float32)/255
    rgb=np.rint(rgba[:,:,:3]*alpha[:,:,None]+127*(1-alpha[:,:,None])).astype(np.uint8)
    # Bound aspect ratio before inference; tiny fragments never enter this pass.
    scale=min(1,768/max(rgb.shape[:2]));size=(max(64,round(rgb.shape[1]*scale)),max(64,round(rgb.shape[0]*scale)))
    depth=stereo.depth(cv2.resize(rgb,size),False)
    mask=cv2.resize(alpha,size)>0.95
    if mask.sum()<64:return np.zeros((9,9),np.int8),depth
    grid=shape_grid(depth,mask)
    return grid,depth


def run(game,model):
    # Reuse the already installed Lume estimator rather than installing a model.
    from depth_estimator import Stereo
    model=Path(model);stereo=Stereo(model);model_hash=hashlib.sha256(model.read_bytes()).hexdigest()
    data=Path(game)/'Data';out=ROOT/'out/3ds/samorost1';dest=out/'depth';dest.mkdir(exist_ok=True)
    cache=ROOT/'work/depth';cache.mkdir(exist_ok=True);report={'model_sha256':model_hash,'grid':9,'pass':'large stable images','levels':{}}
    for level in ['Start','Lyzar','Rybar','Vcely','Strom','Vodarna','Strojovna']:
        source=parse(data/(level+'.lvl'));images={}
        for texture,blob in source['atlas_data'].items():images.update(atlas(blob,texture))
        stable=set()
        for blob in source['sprites'].values():
            movie=operations(blob)
            ids={op['id'] for frame in movie['frames'] for op in frame if op['op'] in (1,5,10) and op['flags']&7==0}
            if movie['total']==1 or len(ids)==1:stable.update(ids)
        selected=[ident for ident in sorted(stable) if ident in images and images[ident][3]*images[ident][4]>=65536 and min(images[ident][3:])>=128]
        atlases={};records=[];details=[]
        for ident in selected:
            texture,x,y,w,h=images[ident]
            if texture not in atlases:atlases[texture]=Image.open(ROOT/'work/assets'/level/(texture+'.png')).convert('RGBA')
            rgba=atlases[texture].crop((x,y,x+w,y+h));key=hashlib.sha256(rgba.tobytes()+model_hash.encode()+Path(__file__).read_bytes()+(ROOT/"source/engine/tools/depth_filter.py").read_bytes()+(ROOT/"source/engine/tools/depth_estimator.py").read_bytes()).hexdigest()
            path=cache/(key+'.bin')
            if path.exists():grid=np.frombuffer(path.read_bytes(),np.int8).reshape(9,9)
            else:
                grid,depth=make_grid(rgba,stereo);path.write_bytes(grid.tobytes())
                Image.fromarray(np.rint(np.clip(depth,0,1)*255).astype(np.uint8)).save(cache/(level+'-'+str(ident)+'-depth.png'))
            records.append(struct.pack('<I',ident)+grid.tobytes());details.append({'id':ident,'texture':texture,'rect':[x,y,w,h],'range':[int(grid.min()),int(grid.max())]})
            print(level,ident,'relief ready',flush=True)
        target=dest/(level+'.smd');target.write_bytes(struct.pack('<4sIII',b'SMD1',1,9,len(records))+b''.join(records))
        report['levels'][level]={'images':details,'bytes':target.stat().st_size}
    (ROOT/'logs/depth.json').write_text(json.dumps(report,indent=2)+'\n')
    manifest=out/'content-manifest.json';metadata=json.loads(manifest.read_text())
    for path in dest.glob('*.smd'):metadata['files'][path.relative_to(out).as_posix()]={'bytes':path.stat().st_size,'sha256':hashlib.sha256(path.read_bytes()).hexdigest()}
    manifest.write_text(json.dumps(metadata,indent=2)+'\n')
    from embed_depth import embed
    embed(ROOT)
    print('Depth maps ready:',dest)
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--game',default=r'C:/Program Files (x86)/Steam/steamapps/common/Samorost 1');p.add_argument('--model',default=str(ROOT/'tools/exe-production/model/depth-small.onnx'));a=p.parse_args();run(a.game,a.model)
