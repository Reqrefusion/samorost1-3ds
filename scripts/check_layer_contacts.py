"""Find first-pose paint/depth contradictions using original alpha silhouettes.

This catches contact mistakes which a definition-only inventory cannot see.
It does not replace a visual review of all animation states on a real display.
"""
from pathlib import Path
import argparse,json,sys
import numpy as np,cv2
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(Path(__file__).resolve().parent))
from lvl import parse
from timeline import operations,atlas

def check(game):
    config=json.loads((ROOT/'docs/layer-profiles.json').read_text());base=Path(game)/'Data';candidates=[];tested=0
    for level,scene in config['scenes'].items():
        if level=='Start':continue # Cinematics are covered by the flight audit.
        source=parse(base/(level+'.lvl'));desktop=parse(base/'desktop'/(level+'.lvl'));images={};cache={}
        movies={i:operations(b) for i,b in source['sprites'].items()}
        for texture,blob in source['atlas_data'].items():images.update(atlas(blob,texture))
        def matrix(op):
            a,d=op.get('ad',(1,1));b,c=op.get('bc',(0,0));x,y=op.get('xy',(0,0));return np.array([[a,c,x],[b,d,y],[0,0,1]],np.float32)
        fit=np.array([[.16,0,8],[0,.16,-16 if level=='Strojovna' else 0],[0,0,1]],np.float32)
        def paint(kind,ident,m,out,path):
            if (kind,ident) in path:return
            if kind==0 and ident in images:
                texture,x,y,w,h=images[ident]
                if texture not in cache:cache[texture]=np.asarray(Image.open(ROOT/'work/assets'/level/(texture+'.png')).convert('RGBA'))[:,:,3]
                alpha=cache[texture][y:y+h,x:x+w];warped=cv2.warpAffine(alpha,(fit@m)[:2],(320,240));np.maximum(out,warped,out=out)
            elif kind==1 and ident in movies:
                for op in movies[ident]['frames'][0]:
                    if op['op'] in (1,5,10) and op.get('alpha',255)>25:paint(op['flags']&7,op['id'],m@matrix(op),out,path|{(kind,ident)})
            elif kind==2 and ident in source['shapes']:
                buf,first,count=source['shapes'][ident];vertices=np.frombuffer(desktop['vertices'][buf],'<f4').reshape(-1,6)
                indices=np.frombuffer(desktop['indices'][buf],'<u2')[first:first+3*count];positions=np.c_[vertices[:,:2],np.ones(len(vertices))]@(fit@m).T
                for triangle in indices.reshape(-1,3):
                    p=positions[triangle,:2]
                    if np.isfinite(p).all() and vertices[triangle,5].max()>.1:cv2.fillConvexPoly(out,np.rint(p).astype(np.int32),255)
        branches=[]
        for op in movies[scene['root']]['frames'][0]:
            if op['op'] not in (1,5,10):continue
            kind=op['flags']&7;ident=op['id'];name=source['strings'].get(op.get('name',-1),'');plane=scene['plane'];group='scenery'
            for item in scene['groups']:
                if name in item['names'] or ident in item['images' if kind==0 else 'sprites']:plane=item['plane'];group=item['label'];break
            if level=='Rybar' and name=='Sup':plane=.1;group='bird'
            mask=np.zeros((240,320),np.uint8);paint(kind,ident,matrix(op),mask,set());branches.append((f'{name or "asset"}/{ident}',plane,group,mask))
        for i,(name,depth,group,mask) in enumerate(branches):
            for other,z,other_group,other_mask in branches[i+1:]:
                area=int(((mask>25)&(other_mask>25)).sum());tested+=1
                if depth-z>0.15001 and area>=5:candidates.append({'scene':level,'covered':name,'cover':other,'covered_group':group,'cover_group':other_group,'depth_gap':depth-z,'overlap_pixels':area})
    result={'scope':'first-pose opaque overlap, six interactive scenes','pair_checks':tested,'threshold':'plane difference >0.15, overlap >=5 pixels','candidates':candidates}
    (ROOT/'docs/layer-contact-check.json').write_text(json.dumps(result,indent=2)+'\n')
    print('Contact audit:',tested,'pair checks;',len(candidates),'candidates')
    if candidates:raise ValueError('Review paint/depth contradictions in docs/layer-contact-check.json')
if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('--game',default=r'C:/Program Files (x86)/Steam/steamapps/common/Samorost 1');check(parser.parse_args().game)
