"""Decode ATF DXT5 channel streams using the existing Windows JPEG-XR codec."""
from pathlib import Path
import json,lzma,struct,subprocess
import numpy as np
from PIL import Image

def atf_records(data):
 if data[:3]!=b'ATF' or data[6]!=255:raise ValueError('Unsupported ATF header')
 width,height=1<<data[13],1<<data[14]
 if data[12]&127 not in (4,13):raise ValueError('Unsupported ATF pixel format')
 at=16;records=[]
 for _ in range(17 if data[12]&127==13 else 16):
  if at+4>len(data):raise ValueError('Truncated ATF record')
  length=int.from_bytes(data[at:at+4],'big');at+=4
  if at+length>len(data):raise ValueError('Truncated ATF payload')
  records.append(data[at:at+length]);at+=length
 return width,height,records[:4]

def unpack_lzma(data,expected):
 if len(data)<5:raise ValueError('Truncated LZMA header')
 decoder=lzma.LZMADecompressor(format=lzma.FORMAT_ALONE)
 raw=decoder.decompress(data[:5]+struct.pack('<Q',expected)+data[5:],max_length=expected+1)
 if len(raw)!=expected:raise ValueError('ATF LZMA output size mismatch')
 return raw

def decode_blocks(width,height,records,alpha,colors):
 blocks=((width+3)//4)*((height+3)//4)
 ai=np.frombuffer(unpack_lzma(records[0],blocks*6),np.uint8).reshape(-1,6)
 ci=np.frombuffer(unpack_lzma(records[2],blocks*4),'<u4')
 ends=np.frombuffer(alpha,np.uint8).reshape(2,blocks).astype(np.uint16)
 rgb=np.frombuffer(colors,'<u2').reshape(2,blocks).astype(np.uint32)
 a0,a1=ends;palette=np.zeros((blocks,8),np.uint16);palette[:,0]=a0;palette[:,1]=a1
 for i in range(2,8):
  palette[:,i]=np.where(a0>a1,((8-i)*a0+(i-1)*a1)//7,((6-i)*a0+(i-1)*a1)//5 if i<6 else 0 if i==6 else 255)
 cp=np.empty((blocks,4,3),np.uint16)
 for endpoint in range(2):
  v=rgb[endpoint];cp[:,endpoint,0]=((v>>11)*255+15)//31;cp[:,endpoint,1]=(((v>>5)&63)*255+31)//63;cp[:,endpoint,2]=((v&31)*255+15)//31
 cp[:,2]=(2*cp[:,0]+cp[:,1])//3;cp[:,3]=(cp[:,0]+2*cp[:,1])//3
 mask=np.zeros(blocks,np.uint64)
 for i in range(6):mask|=ai[:,i].astype(np.uint64)<<(8*i)
 pixels=np.empty((blocks,16,4),np.uint8);ix=np.arange(blocks)
 for i in range(16):
  pixels[:,i,:3]=cp[ix,(ci>>(2*i))&3];pixels[:,i,3]=palette[ix,(mask>>(3*i))&7]
 rgba=pixels.reshape(height//4,width//4,4,4,4).transpose(0,2,1,3,4).reshape(height,width,4)
 return Image.fromarray(rgba,'RGBA')

def prepare(textures,work):
 work=Path(work);work.mkdir(parents=True,exist_ok=True);jobs=[];items={}
 for name,tex in textures.items():
  if not tex['atf']:items[name]=('png',tex['data']);continue
  width,height,records=atf_records(tex['data']);items[name]=(width,height,records)
  for kind,index,fmt in [('alpha',1,'Gray8'),('colors',3,'Bgr565')]:
   source=work/(name+'-'+kind+'.jxr');target=work/(name+'-'+kind+'.raw');source.write_bytes(records[index]);jobs.append({'source':str(source.resolve()),'target':str(target.resolve()),'format':fmt,'width':width//4,'height':height//2})
 manifest=work/'wic-jobs.json';manifest.write_text(json.dumps(jobs),encoding='utf-8')
 script=Path(__file__).with_name('decode_jxr.ps1')
 if jobs:subprocess.run(['powershell.exe','-NoProfile','-ExecutionPolicy','Bypass','-File',str(script.resolve()),'-Manifest',str(manifest.resolve())],check=True)
 output={}
 import io
 for name,item in items.items():
  if item[0]=='png':image=Image.open(io.BytesIO(item[1])).convert('RGBA')
  else:
   w,h,records=item;image=decode_blocks(w,h,records,(work/(name+'-alpha.raw')).read_bytes(),(work/(name+'-colors.raw')).read_bytes())
  image.save(work/(name+'.png'));output[name]=image
 return output