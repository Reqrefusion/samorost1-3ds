# Stereo comfort pass

Depth is a broad surface cue, not a displacement map of the artwork texture.
`tools/depth.py` filters the estimator output with alpha-weighted Gaussian
smoothing (sigma 10% of the shorter image side), uses a 0.20 contrast floor,
area samples a 5x5 broad-shape grid, reconstructs the 9x9 runtime grid and smooths it once more. Image borders and transparent
silhouettes stay at their branch plane. Broad surface gain is 0.70 instead of
0.45; authored connected-object planes and flight depth curves are unchanged.
No inference or filtering runs per frame. Grid resolution and draw count are unchanged.

`canvas.c` gives slender image rectangles and two-triangle vector strokes a
minimum projected thickness of 1.10 pixels. It preserves their center and UVs,
applies equally to both eyes, and stops changing them once zoom makes them thick
enough. Curved vector ropes now use `stroke.h`: the loader recognizes paired-edge
strip topology once, including the wide joins in some authored poses. The
renderer widens each pair around its center to at least 1.35 display pixels,
then reuses the original indices, colors and painter order. Adjacent segments
share their adjusted vertices; individual triangles are not inflated. Rotation,
both eyes and the bottom-screen zoom use the same display-space rule. Other
mesh topology follows the original renderer. The temporary vertex cache lives
in a separate drawing function to avoid large recursive tree stack frames.
Texture minification now uses two alpha-correct mip levels and GPU trilinear
filtering. Mips are generated once during scene preparation, on the existing
prefetch worker for transitions; there is no per-frame CPU texture filter and no
STX file-format change. Extra texture memory is bounded at 31.25% (12 MiB of
scene base pixels becomes 15.75 MiB). Allocation failures fall back to base-only
textures in either staging or GPU upload. Small atlas entries can still lose
fine detail or blend at their boundaries; this deliberately trades very fine
texture contrast for more stable sampling. Physical display crosstalk remains a
separate limitation.

## Validation and deployment

Run `tools/audit_depth_grids.py`, `tests/depth_filter.py`, `tests/stereo_audit.py`, and `tests/game_smoke.py` and `tests/stroke_check.py`.
The first checks broad shape preservation and rejection of synthetic grain;
the second executes the renderer's real geometry functions, including rotated
thin-line and center-preservation checks. These are not hardware comfort tests.

Copy **both** `out/3ds/samorost1.3dsx` and the updated files under
`out/3ds/samorost1/depth/` to their matching SD locations. Existing SD depth maps
take precedence over the embedded fallback, so replacing only the executable
can retain old, noisy maps. No full texture or audio conversion is required.

The supplied real-console log includes roughly 24-25 fps in Strojovna and audio
underruns. This pass does not claim to fix those performance bottlenecks. Retest
rope visibility, broad planet/tree volume and comfort on the real device.

The stroke regression covers all 270 Strom rope definitions, including all 168
curved definitions, plus the other matching scene strokes. The later multipart pass covers 1,437
meshes and 390,438 pair/rotation/zoom/view checks, including all 115 Lyzar shapes. This tests geometry, not hardware pixel
coverage. The external SPK/STX/depth files do not change for this stroke fix;
replacing the player executable is sufficient if the previous filtered depth
maps are already deployed.

## Multipart ropes and edge coverage

Some Lyzar shapes contain several independent strips plus unmatched join/cap
triangles. The loader now records each qualifying `SmStrokeRun` with its original
triangle interval. The renderer walks those intervals in source order, widening
recognized strips while retaining unmatched triangles. Run storage is freed
with the owning scene, including failed-load cleanup. No package format changes.

Each run has four shared rails: transparent outer edge, opaque original edge,
opaque opposite edge, transparent outer edge. A 0.65-pixel alpha fringe on each
side softens the minimum 1.35-pixel body, including on the mono bottom display.
The original UV-independent vertex colors and scene color transforms remain.
Shared vertices prevent cracks between adjacent segments. The fringe triples
strip triangle count; no new draw calls, fullscreen pass or supersampled render
target is introduced. Real-console performance and pixel coverage still need
user testing. Frame-buffer exhaustion skips a whole run instead of emitting a
partial strip.

`tests/stereo_audit.py` executes the actual `stroke_run` from `canvas.c` over
both affected scenes, checking finite output, opaque/transparent coverage rails,
vertex counts and atomic handling of a nearly full vertex buffer.
