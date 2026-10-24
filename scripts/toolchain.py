"""Verified conversion-tool downloads; no game-specific extraction."""
from pathlib import Path,PurePosixPath
import hashlib,json,os,re,shutil,time,urllib.request,zipfile

def digest(p):
 h=hashlib.sha256()
 with Path(p).open('rb') as f:
  while b:=f.read(1024*1024):h.update(b)
 return h.hexdigest()

def dump(p,d):
 p=Path(p);p.parent.mkdir(parents=True,exist_ok=True);tmp=p.with_suffix(p.suffix+'.tmp');tmp.write_text(json.dumps(d,indent=2)+'\n',encoding='utf-8');os.replace(tmp,p)

def request(url):
 return urllib.request.urlopen(urllib.request.Request(url,headers={'User-Agent':'Lume-local-detail-converter','Accept':'application/json'}),timeout=60)

def fetch_json(url):
 with request(url) as f:return json.load(f)

def download(url,path,expected):
 if not re.fullmatch('[0-9a-fA-F]{64}',expected):raise ValueError('Missing tool SHA256.')
 path=Path(path);path.parent.mkdir(parents=True,exist_ok=True)
 if path.is_file() and digest(path)==expected.lower():return path
 tmp=path.with_suffix(path.suffix+'.part')
 for attempt in range(3):
  try:
   print('Downloading:',path.name,flush=True)
   with request(url) as src,tmp.open('wb') as dst:shutil.copyfileobj(src,dst,1024*1024)
   if digest(tmp)!=expected.lower():raise ValueError('Downloaded tool SHA256 mismatch: '+path.name)
   os.replace(tmp,path);return path
  except Exception:
   tmp.unlink(missing_ok=True)
   if attempt==2:raise
   time.sleep(1)

def unzip(src,dest):
 dest=Path(dest);stage=dest.with_name(dest.name+'.extracting');shutil.rmtree(stage,ignore_errors=True);stage.mkdir(parents=True)
 with zipfile.ZipFile(src) as z:
  for entry in z.infolist():
   p=PurePosixPath(entry.filename.replace('\\','/'))
   if p.is_absolute() or '..' in p.parts or ':' in str(p) or ((entry.external_attr>>16)&0o170000)==0o120000:raise ValueError('Invalid tool ZIP path.')
   if not (dest/str(p)).resolve().is_relative_to(dest.resolve()):raise ValueError('Tool ZIP path escapes the destination.')
  z.extractall(stage)
 if dest.exists():shutil.rmtree(dest)
 os.replace(stage,dest)
