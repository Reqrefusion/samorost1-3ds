"""Validate and index the original compact timeline opcodes; no ActionScript execution."""
import struct
from lvl import Reader

def operations(blob):
 r=Reader(blob);total=r.u16();frames=[];frame=[];labels=[]
 while r.pos<len(blob):
  start=r.pos;op=r.u8();record={'op':op,'offset':start}
  if op in (1,5,10,2):
   if op==10:record['name']=r.u8()
   if op!=2:record['id']=r.u16()
   record['depth']=r.u16();flags=record['flags']=r.u8()
   if op!=2 and flags&7 not in (0,1,2):raise ValueError('Unsupported child kind')
   if flags&8:record['xy']=struct.unpack('<2f',r.take(8))
   if flags&16:record['ad']=struct.unpack('<2f',r.take(8))
   if flags&32:record['bc']=struct.unpack('<2f',r.take(8))
   if flags&64:record['color']=struct.unpack('<8f',r.take(32))
   elif flags&128:record['alpha']=r.u8()
  elif op==3:record['depth']=r.u16()
  elif op==4:record['label']=r.u8();labels.append((len(frames)+1,record['label']))
  elif op==6:record['sound'],record['loops'],record['group'],record['left'],record['right']=struct.unpack('<HHBHH',r.take(9))
  elif op in (8,9):
   record['sound_name']=r.u8()
   if op==8:record['variants']=r.u8()
   grouped=r.u8()
   if grouped:record['group_name']=r.u8()
  elif op in (30,32):record['fps']=r.u8()
  elif op==31:record['fps_range']=(r.u8(),r.u8())
  elif op not in (0,7,33,34,35,36,37,128):raise ValueError('Unknown timeline opcode '+str(op)+' at '+str(start))
  record['end']=r.pos;frame.append(record)
  if op==128:frames.append(frame);frame=[]
  if op==0:
   if frame:frames.append(frame);frame=[]
   break
 if r.pos!=len(blob):raise ValueError('Unused timeline bytes')
 return {'total':total,'frames':frames,'labels':labels}

def atlas(blob,texture):
 r=Reader(blob);images={}
 while r.pos<len(blob):
  op=r.u8()
  if op==0:break
  if op==3:
   ident=r.u16();images[ident]=(texture,*struct.unpack('<4H',r.take(8)))
  else:raise ValueError('Unsupported texture atlas command '+str(op))
 if r.pos!=len(blob):raise ValueError('Trailing atlas bytes')
 return images