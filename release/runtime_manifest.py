"""Native code identity for a fixed external-data application."""
from pathlib import Path
import hashlib,json,struct

def digest(path):return hashlib.sha256(Path(path).read_bytes()).hexdigest()
def source_inventory(root):
 root=Path(root)
 return {p.relative_to(root).as_posix():digest(p) for folder in ['source'] for p in sorted((root/folder).rglob('*')) if p.is_file() and p.suffix in {'.c','.cpp','.h','.inc'}}
def verify_runtime(root):
 root=Path(root);metadata=json.loads((root/'player/runtime.json').read_text());app=root/'player/samorost1.3dsx';data=app.read_bytes()
 if data[:4]!=b'3DSX' or struct.unpack_from('<I',data,40)[0]!=0 or digest(app)!=metadata['sha256'] or metadata.get('original_game_content_embedded') is not False:raise ValueError('Invalid fixed application')
 if source_inventory(root)!=metadata['source']:raise ValueError('Native source changed; rebuild the player before setup or release')
 return app

def write_runtime_manifest(root,app):
 root=Path(root);metadata={'format':1,'storage':'external_sd','original_game_content_embedded':False,'original_icon_included':True,'sha256':digest(app),'bytes':Path(app).stat().st_size,'source':source_inventory(root)}
 (root/'player/runtime.json').write_text(json.dumps(metadata,indent=2)+'\n')
