"""Embed the tiny generated relief grids so deployment cannot omit them."""
from pathlib import Path
import struct

def embed(root):
    root=Path(root);paths=sorted((root/'maps/depth').glob('*.smd'))
    target=root/'source/depth_builtin.inc'
    if not paths and target.is_file():return
    if len(paths)!=7:raise ValueError('All seven relief sidecars are required to update the embedded maps')
    lines=['/* Generated compact relief maps; original game textures are not embedded. */'];entries=[]
    for path in paths:
        b=path.read_bytes();magic,version,side,count=struct.unpack_from('<4sIII',b)
        if magic!=b'SMD1' or version!=1 or side!=9 or len(b)!=16+85*count:raise ValueError('Invalid relief sidecar: '+str(path))
        name='depth_'+path.stem;lines+=['static const unsigned char '+name+'[]={',','.join(map(str,b)),'};'];entries.append('{"'+path.stem+'",'+name+',sizeof '+name+'}')
    lines+=['static const struct {const char *name;const unsigned char *bytes;unsigned size;} builtin_depth[]={',',\n'.join(entries),'};']
    text='\n'.join(lines)+'\n'
    if not target.exists() or target.read_text()!=text:target.write_text(text,encoding='utf-8',newline='\n')
