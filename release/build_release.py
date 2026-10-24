"""Prepare a release containing only the fixed 3DSX application."""
from pathlib import Path
import argparse,shutil,sys
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'release'))
from runtime_manifest import verify_runtime

def run(dest):
 dest=Path(dest).resolve()
 if dest.exists() or dest==ROOT or dest.is_relative_to(ROOT):raise ValueError('Choose a new output directory outside the repository')
 app=verify_runtime(ROOT);dest.mkdir(parents=True);target=dest/app.name;shutil.copyfile(app,target);return target
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('--dest',required=True);a=p.parse_args();print('READY:',run(a.dest))
