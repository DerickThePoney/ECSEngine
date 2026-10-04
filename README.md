# ECSEngine

Windows C++ game engine built around a custom ECS, plus a sample game (`BuildingGame`).  
Rendering uses **BGFX**, UI mixes **imgui** / **RmlUi**, audio uses **miniaudio**, and profiling uses **Tracy**. Builds are driven by **FASTBuild** + MSVC.

## What this repo contains

| Path | Purpose |
|------|---------|
| `SRC/` | Engine and game code |
| `BFF/` | FASTBuild project definitions (libs, exes, solution generation) |
| `Script/` | Python helpers: clone submodules, generate `fbuild.bff`, build, cook assets, release zip |
| `External/` | Third-party deps (mostly git submodules: bgfx/bx/bimg, imgui, cereal, assimp, tracy, doctest, …) |
| `Assets/` | Game content submodule (configs, meshes, sounds, …) |
| `tools/` | Bundled `FBuild.exe`, `Tracy.exe` |
| `bin/` | Build outputs (generated) |
| `build/` | Intermediates / generated VS solution (generated) |

### `SRC/` modules

| Module | Role |
|--------|------|
| `ECSCore` | Entity/world/module core |
| `ECSGameplay_Common` / `ECSGameplay_Specific` | Shared + game-specific gameplay modules |
| `Physics` | Physics (island solver / collision; inspired by qu3e) |
| `Rendering` / `RenderingCore` | Scene rendering, picking, materials, BGFX backend |
| `UICore` | UI layer (RmlUi integration) |
| `SoundCore` / `MiniaudioSoundEngine` | Audio |
| `Application` / `Launcher` | App loop, loaders, `BuildingGame` entry point |
| `Common` / `Math` / `DataPack` | Shared utilities, math, datapack I/O |
| `imgui` / `ImGuiTools` | ImGui + editor tooling |
| `Tools/AssetCooker` | Cooks assets into engine-ready data |
| `Tools/DataPacker` | Packs cooked files into `Assets.datapack` / `Sounds.datapack` |
| `Tools/UnitTests` | doctest-based unit tests |

### Build products

| Executable | Description |
|------------|-------------|
| `bin/BuildingGame-x64-<Config>.exe` | Sample game / engine runner |
| `bin/AssetCooker-x64-<Config>.exe` | Asset cooking tool |
| `bin/DataPacker-x64-<Config>.exe` | Datapack builder |
| `bin/UnitTests-x64-<Config>.exe` | Unit tests |

Configs: `Debug` | `Release` | `Profile` | `Final`.

## Requirements

- Windows 10/11 x64
- Visual Studio **2026** recommended on the `Physics` branch (toolset `v145`, `/std:c++latest`); VS 2022 may still work depending on your local setup
- Python 3 (`py` on PATH)
- Git with submodule support

`tools/FBuild.exe` and `tools/Tracy.exe` are already in the repo.

## Clone

```bash
git clone https://github.com/DerickThePoney/ECSEngine.git
cd ECSEngine
git checkout Physics   # if you want the current development branch
```

Pull dependencies:

```bash
# Engine externals (bgfx, imgui, cereal, assimp, tracy, …)
py -u Script/Cloning/CloneSubmodules.py -e

# Game assets (needed to cook/run content)
py -u Script/Cloning/CloneSubmodules.py --assets

# Unit-test deps (doctest)
py -u Script/Cloning/CloneSubmodules.py -t

# Or everything:
py -u Script/Cloning/CloneSubmodules.py -a
```

## Build

All build entry points go through `Script/Compil/BuildAll.py`.

### 1. Build BGFX (once, or when BGFX changes)

```bash
py -u Script/Compil/BuildAll.py -b -c Release
py -u Script/Compil/BuildAll.py -b -c Debug
```

Optional:

```bash
py -u Script/Compil/BuildAll.py -b -c Release -t   # BGFX tools (shaderc, texturec)
py -u Script/Compil/BuildAll.py -b -c Release -s   # BGFX examples
```

> Note: BGFX is generated with Genie `vs2026` → `.build/projects/vs2026/bgfx.slnx` and libs under  
> `External/BGFX/bgfx/.build/win64_vs2026/`. The engine uses the VS 2026 (`v145`) toolset via FASTBuild.

### 2. Build the engine / game

```bash
py -u Script/Compil/BuildAll.py -e -c Release
```

This regenerates `fbuild.bff` from your local VS install, then builds `BuildingGame`, `AssetCooker`, and `DataPacker` into `bin/`.

Custom MSBuild path if needed:

```bash
py -u Script/Compil/BuildAll.py -e -c Release -m msbuild.exe
```

### 3. Visual Studio solution (optional)

```bash
py -u Script/Compil/BuildAll.py -sln
```

Generates `build/ECSEngine.sln` (classic `.sln` + makefile-style `.vcxproj`s that call FASTBuild). Open it in Visual Studio; builds still go through `FBuild.exe`.

### 4. Unit tests

```bash
py -u Script/Compil/BuildAll.py -ut -c Release
```

### 5. Full pipeline (BGFX + engine + tests)

```bash
py -u Script/Compil/BuildAll.py -a -c Release
```

## Assets pipeline

1. Clone assets (`--assets` above).
2. Build tools (`-e` or `-et`).
3. Cook:

```bash
mkdir build   # GenerateData.py chdirs into build/
py -u Script/Generation/GenerateData.py
```

4. Pack:

```bash
cd bin
./DataPacker-x64-Release.exe
```

Produces `Assets.datapack` and `Sounds.datapack`.

### Release zip

```bash
py -u Script/FinalBuild/FinalBuild.py
```

Builds Final, cooks/packs, and writes a zip under `../Release/`.

## Run the game

```bash
cd bin
./BuildingGame-x64-Release.exe
```

Useful flags (non-`Final` builds where noted):

| Flag | Effect |
|------|--------|
| `--editor` | Enable editor mode |
| `--nodatapack` | Load loose files from `Assets/` instead of datapacks |
| `--nosound` | Disable sound |

Day-to-day iteration:

```bash
./BuildingGame-x64-Debug.exe --nodatapack --editor
```

Profile with a `Profile` build + `tools/Tracy.exe`.

## Scaffold a gameplay module

```bash
py -u Script/CodeGenerators/GenerateModule.py -n MyModule -w Specific
# -w Common   → SRC/ECSGameplay_Common
# -w Specific → SRC/ECSGameplay_Specific (default)
```

Creates header/source, registers the module, regenerates the solution.

## License

MIT — see [LICENSE](LICENSE).
