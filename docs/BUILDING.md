# Building and conversion

## Players

Run `SETUP.cmd` on Windows x64 with your own installed Samorost 1. It reuses a
compatible Python 3.12 conversion environment or prepares pinned packages, and
reuses FFmpeg or downloads a verified copy. Windows WPF supplies the JPEG-XR
decoder. No Java, native 3DS compiler, depth model or game executable is run.

Custom paths:

```powershell
powershell -File release/setup_player.ps1 -GameDirectory "D:/Games/Samorost 1" -OutputDirectory "D:/SD-output"
```

Supported `Data/Main.swf` SHA256:

```
0ba23a7ca9c0d401a7439188c2407be5953e12a0c3ecbca5b792846aa0ff5b03
```

Setup logs: `logs/player-setup.log`. Do not share generated game media from
`out/` or caches under `work/`. Only numerical depth maps belong under `maps/`.

## Developers

Use Python 3.11+ and an existing devkitPro installation with devkitARM, libctru,
Citro2D, Citro3D and 3ds-lz4. Install only missing packages with devkitPro pacman.

```sh
python build/build_player_runtime.py --sdk C:/devkitPro
# Linux:
python3 build/build_player_runtime.py --sdk /opt/devkitpro
```

The build requires no original game, converted content, neural model or image
conversion packages. It creates `player/samorost1.3dsx`, `player/runtime.json` and
`out/3ds/samorost1.3dsx`. The application contains only native code, compact depth
grids and the menu icon; original game textures, audio and animation stay external.

The shared modules are explicitly listed in `source/engine/modules.json`. Edit common
code in `source/engine`, not a copied runtime under game source. See
`engine/BAKED_ANIMATION.md` for prepared animation behavior and fallback.
Developer conversion scripts live in `scripts/`; `scripts/depth.py --model PATH`
can generate replacement maps with a separately supplied compatible model.

## GitHub Actions and releases

**Actions > Build 3DSX > Run workflow** builds the application. Leave Release name
empty to update **latest** as a prerelease, or enter a name such as **beta**, **1.0**
or **Summer build**. Spaces become hyphens in the tag; Git tag rules still apply.
Default-branch pushes also update latest, tag pushes create named releases, and
pull requests/other branches produce only the application artifact.

A release attaches **only samorost1.3dsx**. Download the repository separately
for conversion tools and maps. To prepare a release locally:

```sh
python release/build_release.py --dest ../samorost1-release
```

## Checks

```sh
python -m unittest discover -s release/tests -v
```

These checks require no original game or SDK. The development tests under
`tests/` exercise real converted content, gameplay and prepared/interpreted
animation equivalence; run setup before those tests. Real-console performance
and stereo comfort require hardware testing.
