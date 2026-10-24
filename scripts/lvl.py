"""Bounded reader for the owner's Samorost 1 LVL asset and desktop texture streams."""
from pathlib import Path
import hashlib,struct

class Reader:
 def __init__(self,data):self.data=memoryview(data);self.pos=0
 def take(self,n):
  if n<0 or self.pos+n>len(self.data):raise ValueError('Truncated LVL at '+str(self.pos))
  value=self.data[self.pos:self.pos+n].tobytes();self.pos+=n;return value
 def number(self,fmt):return struct.unpack('<'+fmt,self.take(struct.calcsize('<'+fmt)))[0]
 def u8(self):return self.number('B')
 def u16(self):return self.number('H')
 def u32(self):return self.number('I')
 def utf(self):return self.take(self.u16()).decode('utf-8')

def parse(path):
 path=Path(path);data=path.read_bytes();r=Reader(data)
 if r.take(3)!=b'LVL':raise ValueError('Not a Samorost LVL')
 out={'file':str(path),'sha256':hashlib.sha256(data).hexdigest(),'level':None,'strings':{},'textures':{},'atlas_data':{},'images':{},'sounds':{},'sprites':{},'shapes':{},'names':{},'groups':{},'vertices':{},'indices':{},'commands':[]}
 while True:
  cmd=r.u8();start=r.pos-1;out['commands'].append((start,cmd))
  if cmd==0:break
  if cmd==1:out['level']=r.utf()
  elif cmd==31:
   for _ in range(r.u16()):ident=r.u16();out['strings'][ident]=r.utf()
  elif cmd==2:
   name=r.utf();flag=r.u8();blob=r.take(r.u32());out['textures'][name]={'atf':bool(flag),'data':blob}
  elif cmd==102:
   name=r.utf();out['atlas_data'][name]=r.take(r.u32())
  elif cmd==103:r.utf()
  elif cmd==30:
   for _ in range(r.u16()):name=r.utf();out['images'][name]=r.u16()
  elif cmd==8:
   name=r.utf();r.take(r.u32());raise ValueError('Embedded SWF requires an explicit decoder: '+name)
  elif cmd==15:
   name=r.utf();ident=r.u16();out['sounds'][ident]={'name':name,'data':r.take(r.u32())}
  elif cmd==17:
   name=r.utf();ident=r.u32();out['sounds'][ident]={'name':name,'data':r.take(r.u32()),'loop':True}
  elif cmd==18:
   name=r.utf();out.setdefault('raw_sounds',{})[name]=r.take(r.u32())
  elif cmd==41:
   count=r.u8()
   for index in range(count):out['vertices'][index]=r.take(r.u32()*24)
   for index in range(count):out['indices'][index]=r.take(r.u32()*2)
  elif cmd in (42,43):
   length=r.u32();index=r.u8();out['vertices' if cmd==42 else 'indices'][index]=r.take(length-1)
  elif cmd in (10,40):
   declared=r.u32();section=r.pos
   while True:
    op=r.u8()
    if op==0:break
    if op==11:ident=r.u16();out['sprites'][ident]=r.take(r.u32())
    elif op==13:ident=r.u16();out['shapes'][ident]=(r.u8(),r.u32(),r.u32())
    else:raise ValueError('Unknown timeline definition '+str(op)+' at '+str(r.pos-1))
   if r.pos-section not in (declared,declared+1):raise ValueError('Timeline section length mismatch: '+str((declared,r.pos-section)))
   for _ in range(r.u32()):ident=r.u16();out['names'][r.utf()]=ident
   for _ in range(r.u32()):ident=r.u8();out['groups'][r.utf()]=ident
  elif cmd not in (20,25):raise ValueError('Unknown LVL command '+str(cmd)+' at '+str(start))
 if r.pos!=len(data):raise ValueError('Trailing LVL data: '+str(len(data)-r.pos))
 return out