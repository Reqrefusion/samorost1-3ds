from pathlib import Path
import hashlib,json,struct,sys,tempfile,unittest
from unittest.mock import patch
ROOT=Path(__file__).resolve().parents[2]
sys.path.insert(0,str(ROOT/'release'))
import runtime_manifest,setup_player,build_release
class DistributionTests(unittest.TestCase):
 def test_fixed_app_and_current_source(self):
  app=runtime_manifest.verify_runtime(ROOT);self.assertEqual(struct.unpack_from('<I',app.read_bytes(),40)[0],0)
 def test_maps_are_only_small_numeric_grids(self):
  files=setup_player.prepared_maps();self.assertEqual(len(files),7);count=0
  for name in files:
   data=(ROOT/name).read_bytes();magic,version,side,n=struct.unpack_from('<4sIII',data)
   self.assertEqual((magic,version,side),(b'SMD1',1,9));self.assertEqual(len(data),16+n*85);count+=n
  self.assertEqual(count,40)
 def test_release_has_only_application(self):
  with tempfile.TemporaryDirectory() as folder:
   target=build_release.run(Path(folder)/'release');self.assertEqual([p.name for p in target.parent.iterdir()],['samorost1.3dsx'])
 def test_original_is_validated(self):
  with tempfile.TemporaryDirectory() as folder:
   game=Path(folder);(game/'Data').mkdir();main=game/'Data/Main.swf';main.write_bytes(b'owned-original')
   with self.assertRaises(ValueError):setup_player.find_original(game)
   with patch.object(setup_player,'MAIN_SHA',hashlib.sha256(main.read_bytes()).hexdigest()):self.assertEqual(setup_player.find_original(game),game.resolve())
 def test_updates_keep_save_and_log(self):
  with tempfile.TemporaryDirectory() as folder:
   base=Path(folder);content=base/'content';content.mkdir();(content/'image.bin').write_bytes(b'new')
   out=base/'out';game=out/'3ds/samorost1';game.mkdir(parents=True)
   for name in ['save.txt','runtime.log']:(game/name).write_bytes(b'keep')
   files={'image.bin':{'sha256':hashlib.sha256(b'new').hexdigest()}}
   setup_player.publish(ROOT/'player/samorost1.3dsx',content,out,files)
   for name in ['save.txt','runtime.log']:self.assertEqual((game/name).read_bytes(),b'keep')
   self.assertEqual((game/'image.bin').read_bytes(),b'new')
   with self.assertRaises(ValueError):setup_player.publish(ROOT/'player/samorost1.3dsx',content,out,{'../escape':files['image.bin']})
 def test_engine_profile_has_direct_sources(self):
  self.assertTrue((ROOT/'source/engine').is_dir());self.assertFalse((ROOT/'engine').exists())
  self.assertFalse((ROOT/'runtime').exists())
  for path in json.loads((ROOT/'source/engine/modules.json').read_text())['samorost1']:self.assertTrue((ROOT/'source'/path).is_file())
 def test_workflow_does_not_convert_game_assets(self):
  text=(ROOT/'.github/workflows/build-3dsx.yml').read_text()
  self.assertIn('build/build_player_runtime.py',text);self.assertNotIn('setup_player.py',text);self.assertNotIn('scripts/prepare.py',text)
if __name__=='__main__':unittest.main()
