# Samorost 1 3DS

A native stereoscopic adaptation for Old and New Nintendo 3DS. You need your own
supported installed copy of Samorost 1 to prepare the game files.

## Play

1. Download **samorost1.3dsx** from [Releases](https://github.com/Reqrefusion/samorost1-3ds/releases).
2. Download this repository with **Code > Download ZIP** and extract it on Windows.
3. Run **SETUP.cmd**. Select your installed **Samorost 1** folder if prompted.
4. Copy **out/3ds/samorost1/** to **SD:/3ds/samorost1/** and the application to
   **SD:/3ds/samorost1.3dsx**.
5. Launch Samorost 1 from Homebrew Launcher.

Setup checks `C:/Program Files (x86)/Steam/steamapps/common/Samorost 1`.
It converts your original artwork, audio and animation data and uses the reviewed
3D maps included here. No 3DS SDK, native compiler or depth model runs during setup.
Compatible installed conversion tools are reused; missing tools are prepared
locally. Original game files are never executed or uploaded.

```
3ds/
  samorost1.3dsx
  samorost1/
    levels/
    textures/
    audio/
    depth/
```

Saves (`save.txt`) and logs (`runtime.log`) are stored in **SD:/3ds/samorost1/**.
Preserve them when updating. For an application update, replace only the 3DSX;
when data changes, rerun setup and copy the updated data too. Prepared animation
`.lbk` files are generated from your own game and live beside its `.spk` levels.

## Controls

The top screen shows the stereoscopic overview. Touch the bottom screen to
interact. Use the D-pad for zoom and Circle Pad to pan the bottom view; Y resets
it. The 3D slider controls depth on the top screen.

## Development

See [BUILDING.md](docs/BUILDING.md). `BUILD.cmd --sdk C:/devkitPro` compiles the
fixed application without original game files. GitHub Actions builds it and
publishes only the 3DSX; players download the repository separately for setup.

The shared runtime is in `source/engine`, shared production utilities in
`source/engine/tools`, and Samorost integration in `source`. The engine snapshot matches
the Lume repository and is identified by `source/engine/source-manifest.json`.

## License

Project code and tools: GPL-3.0-only. Original Samorost 1 content is outside this
license. See [LICENSE](LICENSE), [COPYRIGHT.md](COPYRIGHT.md) and
[third-party notices](third_party/README.md). Do not redistribute converted media.
