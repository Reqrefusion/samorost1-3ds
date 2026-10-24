"""Inspect every deployed relief grid and create a fixed-scale contact sheet."""
from pathlib import Path
import json,struct
import numpy as np
from PIL import Image,ImageDraw
ROOT=Path(__file__).resolve().parents[1]
def read(path):
 data=path.read_bytes();magic,version,side,count=struct.unpack_from('<4I',data)
 assert data[:4]==b'SMD1' and version==1 and side==9 and len(data)==16+count*85
 return {struct.unpack_from('<I',data,16+i*85)[0]:np.frombuffer(data,np.int8,count=81,offset=20+i*85).reshape(9,9).astype(float) for i in range(count)}
def curvature(g):
 return float((np.abs(np.diff(g,n=2,axis=0)).mean()+np.abs(np.diff(g,n=2,axis=1)).mean())/2)
rows=[];tiles=[]
for path in sorted((ROOT/'out/3ds/samorost1/depth').glob('*.smd')):
 oldpath=ROOT/'work/depth-before-filter'/path.name;old=read(oldpath) if oldpath.exists() else {}
 for ident,grid in read(path).items():
  assert not np.any(grid[0]) and not np.any(grid[-1]) and not np.any(grid[:,0]) and not np.any(grid[:,-1])
  before=old.get(ident,grid);rows.append({'scene':path.stem,'id':ident,'range':[int(grid.min()),int(grid.max())],'curvature_before':curvature(before),'curvature_after':curvature(grid)})
  tile=Image.new('RGB',(216,130),'#20252b');draw=ImageDraw.Draw(tile);draw.text((4,2),path.stem+' '+str(ident),fill='white')
  for i,g in enumerate([before,grid]):
   image=Image.fromarray(np.rint(g+128).clip(0,255).astype('uint8')).resize((100,100),Image.Resampling.BILINEAR)
   tile.paste(image,(4+i*108,24))
  tiles.append(tile)
assert len(rows)==40,len(rows)
sheet=Image.new('RGB',(216*5,130*((len(tiles)+4)//5)),'#20252b')
for i,tile in enumerate(tiles):sheet.paste(tile,((i%5)*216,(i//5)*130))
out=ROOT/'out/stereo';out.mkdir(exist_ok=True);sheet.save(out/'all-depth-before-after.png')
report={'images':rows,'mean_curvature_before':float(np.mean([r['curvature_before'] for r in rows])),'mean_curvature_after':float(np.mean([r['curvature_after'] for r in rows])),'note':'Curvature measures grid bends, not physical display comfort. Left old / right current, same grayscale scale.'}
(ROOT/'logs/depth-grid-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('PASS',len(rows),'grids; mean grid curvature',report['mean_curvature_before'],'->',report['mean_curvature_after'])
