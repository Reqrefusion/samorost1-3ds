"""Link the production shader-size helper without any SDK _size symbol."""
from pathlib import Path
import os,shutil,subprocess,tempfile,unittest
ROOT=Path(__file__).resolve().parents[2]
class ShaderSdkTests(unittest.TestCase):
 def test_shader_boundaries_support_old_and_header_only_size_sdks(self):
  sdk=Path(os.environ.get('DEVKITPRO','C:/devkitPro' if os.name=='nt' else '/opt/devkitpro'))
  suffix='.exe' if os.name=='nt' else '';cc=sdk/'devkitARM/bin'/('arm-none-eabi-gcc'+suffix);nm=sdk/'devkitARM/bin'/('arm-none-eabi-nm'+suffix)
  if not cc.is_file():self.skipTest('devkitARM unavailable')
  source=(ROOT/'source/canvas.c').read_text();self.assertNotIn('render2d_shbin_size',source)
  a=source.index('static uint32_t shader_binary_size(');b=source.index('}',a)+1;helper=source[a:b]
  with tempfile.TemporaryDirectory() as folder:
   work=Path(folder);c=work/'check.c';c.write_text('#include <stdint.h>\nextern const uint8_t render2d_shbin[],render2d_shbin_end[];\n'+helper+'\nuint32_t probe(void){return shader_binary_size();}\n')
   for legacy in [False,True]:
    asm=work/'blob.s';text='.section .rodata\n.balign 4\n.global render2d_shbin\nrender2d_shbin:\n.space 16\n.global render2d_shbin_end\nrender2d_shbin_end:\n'
    if legacy:text+='.global render2d_shbin_size\nrender2d_shbin_size: .word 16\n'
    asm.write_text(text);elf=work/('legacy.elf' if legacy else 'header-size.elf')
    result=subprocess.run([str(cc),'-march=armv6k','-mfloat-abi=hard','-O2','-nostdlib','-nostartfiles','-Wl,-e,probe',str(c),str(asm),'-o',str(elf)],capture_output=True,text=True)
    self.assertEqual(result.returncode,0,result.stdout+result.stderr)
    self.assertFalse(subprocess.check_output([str(nm),'-u',str(elf)],text=True).strip())
if __name__=='__main__':unittest.main()
