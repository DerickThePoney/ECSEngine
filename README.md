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

## External libraries

Third-party code lives under `External/` (plus `tools/` for host binaries). Most are git submodules pulled by `Script/Cloning/CloneSubmodules.py`; some are vendored trees or prebuilt binaries checked into the repo.

| Library | Path | Role | How it gets there |
|---------|------|------|-------------------|
| **bgfx** | `External/BGFX/bgfx` | Cross-platform rendering backend | Submodule (`-e` / `-b`) |
| **bx** | `External/BGFX/bx` | bgfx base utilities + Genie (`genie.exe`) | Submodule (with BGFX) |
| **bimg** | `External/BGFX/bimg` | bgfx image encode/decode | Submodule (with BGFX) |
| **GLFW** | `External/glfw-3.3.bin.WIN64` | Window / input (prebuilt Win64 + `glfw3.dll`) | Vendored binaries in repo |
| **Dear ImGui** | `External/imgui` (and legacy `External/IMGUI` entry in `.gitmodules`) | Immediate-mode UI / editor widgets | Submodule (`-e`) |
| **RmlUi** | `External/RMLUI/RMLUI` | HTML/CSS-like runtime UI | Vendored under `External/RMLUI` |
| **FreeType** | `External/RMLUI/FreeType` | Font rasterization for RmlUi | Vendored under `External/RMLUI` |
| **Assimp** | `External/assimp` + `External/assimpBinaries` | 3D asset import (AssetCooker); prebuilt `assimp-vc142-*.dll` / libs | Sources via submodule; binaries vendored |
| **miniaudio** | `External/miniaudio` | Audio playback / sound engine backend | Submodule (`-e`) |
| **cereal** | `External/cereal` | Serialization (JSON/binary archives) | Submodule (`-e`) |
| **Tracy** | `External/tracy` (+ `tools/Tracy.exe`) | Frame profiler client; standalone profiler UI in `tools/` | Submodule (`-e`); UI binary in `tools/` |
| **brigand** | `External/brigand/brigand` | Compile-time metaprogramming helpers | Submodule (`-e`) |
| **GLM** | `External/glm` | Mathematics (vectors/matrices); include-only | Expected under `External/glm` (see BFF `glm.bff`) |
| **Boost** (subset) | `External/boost/...` | Header-only pieces used by the engine (e.g. variant, mpl, stacktrace, …) | Expected modular Boost headers under `External/boost` (see BFF `boost.bff`) |
| **polypartition** | `External/polypartition-master` | Polygon partitioning / triangulation | Vendored sources in repo |
| **teeny-sha1** | `External/teeny-sha1-master` | SHA-1 hashing (AssetCooker) | Vendored sources in repo |
| **doctest** | `External/Test/doctest` | Unit-test framework | Submodule (`-t`) |
| **FASTBuild** | `tools/FBuild.exe` | Build system used for the engine/solution | Binary in `tools/` |

Notes:

- BGFX is built separately (`BuildAll.py -b`); engine links `bgfx` / `bimg` / `bx` from `External/BGFX/bgfx/.build/win64_vs2026/bin/`.
- Assimp DLLs are copied into `bin/` for AssetCooker; GLFW’s `glfw3.dll` is copied for the game.
- Clone helpers: `-e` / `--engine` pulls engine externals (includes BGFX), `-b` only BGFX, `-t` / `--tests` pulls doctest, `-a` / `--all` pulls engine + assets + tests.

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
