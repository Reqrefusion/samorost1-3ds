"""Compile only the small native player; assets are prepared separately."""
from pathlib import Path
import argparse,os,subprocess,json,hashlib,shutil
ROOT=Path(__file__).resolve().parents[1]
SHARED=ROOT/'source'
import sys
sys.path.insert(0,str(ROOT/'scripts'));sys.path.insert(0,str(ROOT/'release'))
from embed_depth import embed
from review_layers import export_profiles

def build(sdk):
    embed(ROOT)
    export_profiles(ROOT)
    sdk=Path(sdk).resolve();arm=sdk/'devkitARM';ctru=sdk/'libctru';port=sdk/'portlibs/3ds'
    ext='.exe' if os.name=='nt' else '';cc=arm/'bin'/('arm-none-eabi-gcc'+ext);cxx=arm/'bin'/('arm-none-eabi-g++'+ext)
    dest=ROOT/'work/objects';dest.mkdir(parents=True,exist_ok=True);out=ROOT/'out/3ds';out.mkdir(parents=True,exist_ok=True)
    env=dict(os.environ)
    for key in ['GCC_EXEC_PREFIX','COMPILER_PATH','LIBRARY_PATH','C_INCLUDE_PATH','CPLUS_INCLUDE_PATH']:env.pop(key,None)
    env['PATH']=os.pathsep.join([str(arm/'bin'),str(sdk/'tools/bin'),env.get('PATH','')])
    def run(args):subprocess.run([str(x) for x in args],cwd=ROOT,env=env,check=True)
    arch=['-march=armv6k','-mtune=mpcore','-mfloat-abi=hard','-mtp=soft']
    flags=arch+['-mword-relocations','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-ffunction-sections','-fdata-sections','-D__3DS__','-DARM11',*[f'-I{x}' for x in [ROOT/'source',ctru/'include',port/'include']]]
    units=[*[SHARED/p for p in json.loads((ROOT/'source/engine/modules.json').read_text())['samorost1']],*sorted((ROOT/'source').glob('*.c')),*sorted((ROOT/'source').glob('*.cpp'))]
    objects=[]
    for i,src in enumerate(units):
        obj=dest/f'{i}.o';cpp=src.suffix=='.cpp';run([cxx if cpp else cc,*flags,*(['-std=gnu++17','-fno-exceptions','-fno-rtti'] if cpp else ['-std=gnu11']),'-c',src,'-o',obj]);objects.append(obj)
    elf=dest/'samorost1.elf';icon=dest/'samorost1.smdh';binary=ROOT/'player/samorost1.3dsx';binary.parent.mkdir(exist_ok=True)
    run([cxx,*arch,'-specs=3dsx.specs','-Wl,--gc-sections',*objects,f'-L{port/"lib"}',f'-L{ctru/"lib"}','-lcitro2d','-lcitro3d','-llz4','-lctru','-lm','-o',elf])
    original=ROOT/'resources/icon.png'
    run([sdk/'tools/bin'/('smdhtool'+ext),'--create','Samorost 1','Native stereoscopic adventure','Homebrew',original if original.is_file() else ctru/'default_icon.png',icon])
    run([sdk/'tools/bin'/('3dsxtool'+ext),elf,binary,'--smdh='+str(icon)])
    (dest/'build.json').write_text(json.dumps({'sha256':hashlib.sha256(binary.read_bytes()).hexdigest(),'sources':{str(s.relative_to(ROOT)):hashlib.sha256(s.read_bytes()).hexdigest() for s in sorted(set(units)|{p for p in SHARED.rglob('*') if p.is_file() and p.suffix in {'.h','.inc','.c'}}|set((ROOT/'source').glob('*.h'))|set((ROOT/'source').glob('*.inc')))}},indent=2))
    shutil.copyfile(binary,out/'samorost1.3dsx')
    from runtime_manifest import write_runtime_manifest
    write_runtime_manifest(ROOT,binary)
    print('READY:',binary)
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('--sdk',default=os.environ.get('DEVKITPRO','C:/devkitPro'));build(p.parse_args().sdk)
