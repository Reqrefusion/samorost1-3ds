"""Find an existing host compiler without installing anything."""
from pathlib import Path
import os,shutil,subprocess
def compiler():
 if os.name!='nt':
  cc=shutil.which('cc') or shutil.which('clang') or shutil.which('gcc')
  return (cc,os.environ.copy(),False) if cc else None
 finder=Path(os.environ.get('ProgramFiles(x86)',r'C:\Program Files (x86)'))/'Microsoft Visual Studio/Installer/vswhere.exe'
 if not finder.is_file():return None
 install=subprocess.check_output([str(finder),'-latest','-products','*','-property','installationPath'],text=True).strip()
 if not install:return None
 cl=subprocess.check_output([str(finder),'-latest','-products','*','-find',r'VC\Tools\MSVC\**\bin\Hostx64\x64\cl.exe'],text=True).strip().splitlines()
 if not cl:return None
 env=dict(os.environ)
 msvc=Path(cl[0]).parents[3]
 sdk=Path(os.environ.get('ProgramFiles(x86)',r'C:\Program Files (x86)'))/'Windows Kits/10'
 versions=sorted(p.name for p in (sdk/'Include').iterdir() if p.is_dir() and (sdk/'Lib'/p.name).is_dir())
 if not versions:return None
 version=versions[-1]
 env['INCLUDE']=os.pathsep.join(map(str,[msvc/'include',*[sdk/'Include'/version/n for n in ['ucrt','shared','um','winrt']]]))
 env['LIB']=os.pathsep.join(map(str,[msvc/'lib/x64',sdk/'Lib'/version/'ucrt/x64',sdk/'Lib'/version/'um/x64']))
 env['PATH']=str(Path(cl[0]).parent)+os.pathsep+env.get('PATH','')
 return cl[0],env,True
