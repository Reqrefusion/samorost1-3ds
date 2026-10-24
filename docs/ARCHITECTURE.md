# Runtime and source bindings

## Boundaries

The player compiles shared modules from the workspace `source/engine/` using
`source/engine/modules.json`; there is no local runtime copy or forwarding source.
The native scene VM lives in `adapters/sprite_timeline`. The GPU canvas and audio
stream orchestration remain application adapters in `source/`; this is not a
general AIR/ActionScript virtual machine.

| Original system | Port |
| --- | --- |
| Assets LVL reader, pooled strings and atlas tables | `tools/lvl.py`, `tools/timeline.py`, `tools/prepare.py` |
| ATF DXT5 and JPEG-XR channel streams | `tools/atf.py`, Windows WPF decoder in `tools/decode_jxr.ps1` |
| TimelineMovie / TimelineSprite | `source/engine/adapters/sprite_timeline/timeline.c` |
| Scene onAnimLabel, onTouchSensor, onAddNamedObject | inspected handlers translated by `tools/port_rules.py` into `source/rules.inc` |
| Starling object methods and sensor transforms | generation-aware Movie facade in `source/game.cpp` |
| Global audio calls | native DSP PCM streaming in `source/audio.c` |
| Lume DSP initialization and Azahar compatibility | shared `source/engine/platform/3ds/audio_device.h`, also used by the original Lume audio module |
| Global scene fade completion | deferred `onZatmeni` entry event and scene transition queue |
| Starling GPU drawing | `source/canvas.c`, Citro3D and the SDK Citro2D vertex shader |

The inspected Main.swf fingerprint and handler source fingerprints are recorded
in `rules-provenance.json`. Decompiled files and conversion intermediates stay
in ignored `work/`. To regenerate rules during development, export the scripts
from the owned Main.swf with the existing FFDec installation to
`work/actionscript/`, then run `python tools/port_rules.py`. The generator rejects
unhandled decompiler expressions. Two malformed Vodarna child-reordering
expressions and Starling-only tweens have explicit native replacements.

## Game events and lifetime

Actors are resolved by their original names. Sensors use original rectangles,
including rotations and nested actor transforms. The touch-down sensor remains
captured until release; generation checks discard stale actor/sensor references.
Hidden ancestors cannot receive coordinate-based input.

Stopped outer movie wrappers keep independently playing children alive. Seeking
the same frame preserves those children. Looping first frames reuse matching
children. These distinctions prevent the restart and delayed-action failures
previously encountered in Lume. A scene-entry completion event is essential:
Strom starts the gondola/bird sequence from `onZatmeni`, not from an ordinary
button click. Transitions are deferred until after timeline iteration.

## Converted data

All integers are little-endian unless explicitly noted by the original format.

* SPK1: version, root ID and table counts; UTF-8 strings; texture dimensions;
  atlas image rectangles; sprite frame offsets and original opcodes; colored
  shapes, copied/aligned vertex/index buffers; sound names and actor names.
* STX1: width, height, raw byte count, packed byte count, then a safe LZ4 block.
  Pixels are straight-alpha RGBA8 in ABGR byte order and 8x8 Morton tiles.
  Atlas coordinates retain original dimensions after texture downscaling.
* PCM1: sample rate, channel count, sample width and frame count, then signed
  little-endian 16-bit stereo PCM at 32 kHz. Audio is streamed in bounded DSP
  buffers, with two music channels and six effect voices.

Package, table, cursor, mesh-index and texture bounds are checked. Four original
Vcely vertices contain non-finite positions: triangles referencing these are
skipped and counted in the log. Colors must remain finite. Original files are
never modified or executed by the converter.

## Stereo and performance

Top-left and top-right share one VM tick and camera. Layer depth is stable for
the lifetime of each branch; descendants inherit the branch plane so connected
animation pieces do not acquire independent disparity. The slider scales the
offset; zero is monoscopic. Bottom input uses the inverse of its own camera,
without an eye offset. The top image is fitted at 304x240 and remains a fixed overview. Only the bottom
view receives the shared Lume viewport transform. D-pad up/down animates zoom,
right/left steps it immediately, and Circle Pad pans when zoomed. Touch mapping
uses the last presented transform rather than the next animation step. The
viewport header now has configurable stage bounds: Lume retains 13..227 by
default, while Samorost selects 0..240.

Textures load per scene, outside the render loop. Images draw as GPU triangles;
colored meshes submit each shape as a batch. PCM streaming avoids whole-track
allocations. Frame-rate, submitted-vertex and audio-underrun counters are logged
every five seconds. New 3DS can use its faster CPU mode; Old 3DS uses the same
code path. Neither console's performance has been measured yet.

## Checks

With an existing host C/C++ compiler:

```text
python tests/native_smoke.py
python tests/game_smoke.py
```

The scene tests call the actual C/C++ VM and gameplay rules, with only GPU/audio
omitted. They simulate legitimate enabled-sensor actions rather than forcing
scene IDs. All seven next-scene assertions and the outro return currently pass.
Visual correctness, sound mixing and physical stereo remain hardware tests.

## Reusable findings for Lume

The original shared `engine/core/jobs_state.c` now supports MSVC interlocked
operations as well as the existing GCC atomics. The shared standalone build
template accepts Citro libraries in either libctru or portlibs. These fixes are
also present in this runtime export. Lume gameplay and media were not rebuilt.

Samorost-specific frame opcodes and puzzle handlers remain separate. The VM's
lifetime checks, captured-input contract and scene-entry-event regression are
the reusable design findings; they should be adapted and tested against Lume's
own ActionScript rather than copying Samorost's puzzle rules into Lume.

## First player-test fixes

The first user log reported `DSP unavailable; continuing silently`. The player
had omitted Lume's emulator-specific DSP initialization retry. This initializer
is now extracted to a shared platform header used by both games. A controlled
service test covers normal initialization, physical-console guards, both emulator
IDs, query failure and retry failure. Runtime diagnostics report initial/final
DSP results, whether HLE was used, and the number of active audio voices.

Normal startup no longer calls `consoleInit`: its RGB565 framebuffer conflicts
with the renderer's RGB8 output transfer and corrupts the lower screen. Both
screens explicitly start in RGB8, and the canvas checks their formats. The text
console is only initialized after the GPU is closed on an error. Neither fix
changes the per-frame animation or stereo rendering path. GPU output and audible
playback still require confirmation from the updated player.

## First relief pass, batching and final-scene framing

`tools/depth.py` uses the parent's existing `stereo_depth.Stereo` estimator and
model, with an input/model/script hash cache. It selects large images referenced
by a static sprite or reused unchanged by a timeline. Forty images receive 9x9
signed relief grids. Transparent silhouettes and image rims converge to the
branch plane; neighboring pieces therefore retain their authored boundary. Pose
sequences keep plane stereo pending a temporally coherent second pass. These
are inferred draft depth maps, not hand-validated surface reconstructions.

SMD1 contains version, grid side, record count, then image-ID/81-byte records.
`source/depth.c` checks IDs, duplicates, table limits and exact file length. An
invalid sidecar clears its partial data and falls back to plane stereo with a
log entry. Source assets free relief data with the scene. At nonzero slider
strength only, upper-screen images use small triangulated surfaces. The lower
screen and zero-slider rendering retain ordinary quads. Inference never runs
on the console.

The renderer now merges adjacent geometry with identical texture and color
offset, flushing before changing either state or render target/projection. It
does not reorder transparent objects. This batches the starfield and adjacent
colored shapes as well as atlas images. Component color conversion avoids
per-vertex libm rounding calls. A production-function test checks texture/offset
flushes and view-boundary submission order. Updated logs report draw calls and
the SDK's drawing/processing timing counters alongside frame rate. Scene-loading
time no longer contaminates the next scene's rolling frame-rate sample.

The inspected Strojovna source uses pivotY=850 for the room, while Planetka uses
center-relative coordinates. The adapter translates the room upward by 100
authored units and places Planetka at (950,750) in the shared coordinate system.
The exterior cutscene resets bottom zoom before presentation. Opaque border
masks enforce the authored 1900x1500 window on both eyes; input also rejects
points outside it. Native tests exercise all seven exits, the outro, relief
loading and bad-map fallback, pinned relief borders and cinematic coordinates.
Actual image quality and frame-rate improvement still require an updated
emulator/console run; the prior log is the baseline, not a new measurement.

## Deployment audit and asynchronous transitions

The subsequent real user log had `relief_images=0` for every scene: Azahar's SD
copy contained no depth directory. Plane stereo was active, but the relief pass
was not. `source/depth_builtin.inc` now embeds the generated 3,512-byte data;
`tools/embed_depth.py` updates it during a build and after map generation. A
missing/corrupt sidecar falls back to these maps, and the log records the source.
The console still runs no model inference.

`source/stereo.h` is the shared eye-displacement contract used by the renderer
and its audit. The audit compiles the production quad/relief mesh functions,
stages real packages/textures and rasterizes the resulting triangles on the CPU.
Its output is a geometry reference, not an emulator screenshot. All embedded
grids match the generated sidecars, zero-slider eyes are byte-identical,
vertical disparity is zero, and opposite eyes have symmetric, nonuniform
horizontal displacement. The Start/1300 reference has roughly 0.05..2.41 pixels
of left-right disparity at the audit's 0.25 branch plane. This verifies surface
variation rather than only moving a flat picture. The CPU preview and JSON
report live in `out/stereo/` and `logs/stereo-audit.json`. Physical stereo comfort
and actual GPU presentation remain device checks.

`source/scene_loader.c` prepares immutable next-scene data on a worker. It reads
compressed textures in bounded chunks and decompresses into at most 12 MiB of
normal heap memory. It makes no GPU, gameplay or audio calls. A lock publishes
completion, and the main thread joins before taking ownership. It synchronizes
the previous GPU frame, releases old textures, uploads prepared pixels, transfers
asset ownership, then initializes the new VM. Data is freed on every failure and
on exit. If preparing alongside the old scene cannot succeed, recovery releases
the old scene first and retries synchronously, preserving the low-memory path.

The transition request starts preparation during the original delay. A finished
scene can keep presenting its last frame and servicing audio until data is ready.
Startup remains synchronous. Stage timings are logged separately; host staging
tests are not a substitute for new Old/New 3DS measurements.

## Authored flight depth groups

Display-list position is no longer used as a distance estimate. It split
connected original plates and changed newly inserted/replaced sprites' planes.
`source/layers.c` resolves immutable semantic anchors; unprofiled branches remain
coplanar and still retain eligible relief maps. Draw order remains unchanged.

* Start's rocket (sprite 615) owns its complete hull, flame and pose subtree.
  Its plane follows the original affine scale through a bounded log curve,
  rather than assigning independent planes to parts. Very small sprite images
  no longer receive constant screen-sized relief.
* The home planet (610), external cap/pipe pieces, distant planet (611), portrait
  animation and orange effects have explicit groups. Home-planet attachments
  share the same scale-derived anchor even when they are root-level siblings.
* Rybar's bird wrapper (776) owns incoming-flight, perched and takeoff variants,
  including the rider added to flight 775. Arrival eases from -0.10 to +0.10;
  departure eases from +0.10 to +0.65 over the authored takeoff interval. The
  photographic stage remains at -0.40. Pose changes cannot split the group.
* Flight groups disable independent image relief. This keeps wings, passenger
  and flame joined even when they use separate source images or have tiny
  authored transforms. The rocket plane is bounded to -0.45..+0.80.
* Stars occupy three more distant planes. Their shared phase follows Start's
  original Hvezdy event labels; nearer stars move faster. Both eyes read the
  same phase, and rendering no longer writes depth into VM nodes.

The native gameplay run checks every active flight group's descendants across
3,498 rocket and 959 bird samples. It verifies identical inherited disparity,
independence from sibling count, bounded depth and suppression of relief on
subpixel images. It observed rocket planes +0.11..+0.80 and bird planes
-0.10..+0.65 across the played paths. These are artistic depth profiles; actual
console comfort and composition still need a player check.

## Full source-layer assessment

The review now covers 2,946 image definitions, 459 timelines, 1,455 vector
definitions and 21,155 authored frame records, including the common library.
All 107 scene-local object names have reviewed policies. Thirty-one configured
assemblies supplement the native flight curves. The exported JSON records
contextual ownership for reused definitions and separately marks 175 definitions
not reachable from exported roots; they were not deleted.

All scenes have a deliberate base plane. The profile table groups smoker/chair/
shadow/hand, zipline posts and carrier, lift supports and passenger, tree/sucker/
cover, snail, swarm, squirrel/cabinet/hollow, branch/rope/owl/gondola, worms and
woodpecker, bridge/railings/ants/hero, door, switchboard/knobs, mushrooms, moths,
and machine/labels/needles/wheel/shadow. Animated parts inherit those anchors
rather than receiving another depth from pose index, alpha or child count.
Runtime black overlays use zero disparity.

A first-pose original-alpha/vector contact audit made 2,240 ordered branch-pair
checks. It found that Strom's hollow cover must share the front cabinet/squirrel
plane, a bark cover must meet the branch plane, and Vodarna's rock abutment must
meet the bridge railings rather than being treated as a distant silhouette. No
candidates remain at the audit's >0.15-plane / >=5-pixel threshold. This is a
limited geometric screen-space check; it does not certify every pose's physical
occlusion or subjective comfort.

Rybar's passenger now approaches the bird plane during jump frames 10..35, and
the bone approaches the incoming bird during arrival frames 200..246. Endpoints
use the exact destination plane before original visibility/ownership handoffs.
This avoids the depth pop when the passenger or carried object is recreated
inside the bird animation. Full native gameplay checks validate these handoffs
and shared contacts across roughly 4.9 million active-node samples. All timeline
definitions also receive an exhaustive authored-frame replay without gameplay
callbacks, in addition to the complete scene-exit run.

Shared runtime ownership and build rules: see `../../../engine/README.md`.
