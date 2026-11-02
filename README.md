# ECSEngine

Windows C++ game engine built around a custom ECS, plus two sample games: **Colony** (`BuildingGame`) and **Orbital**.  
Rendering uses **BGFX**, UI mixes **imgui** / **RmlUi**, audio uses **miniaudio**, and profiling uses **Tracy**. Builds are driven by **FASTBuild** + MSVC.

This is a **toy / learning engine**, not a production framework. A lot of the tech is unfinished, experimental, or mid-refactor — the repo exists mainly as a playground to try out new techniques (ECS design, physics, rendering, tooling, etc.). Expect rough edges, incomplete systems, and APIs that change when something more interesting comes along.

## Engine capabilities

What the codebase currently explores (inferred from `SRC/`; completeness varies):

- **ECS / gameplay composition** — Entity worlds, templates, and composable **modules** (position, orientation, appearance, rigidbody, colliders, ownership links, …). New modules can be scaffolded with `GenerateModule.py`.
- **Sample games** — **Colony** is the building game on a circular grid (construction, storage, recipe production, energy, selection UI). **Orbital** is a separate executable with the rocket and a free camera. Shared engine code is compiled once; each game links only its own gameplay library.
- **Physics** — Custom rigid-body engine with island solving, contact generation, broadphase (AABB tree), and shapes: **box / sphere / capsule** (including OBB and mixed intersections).
- **Pathfinding** — Navmesh-style requests over world polygons/obstacles (`NavMeshPathfindingManager`), plus circular-grid path graphs for colony movement.
- **Rendering (BGFX)** — Multi-pass materials/programs, mesh drawing (including skinned poses), terrain / heightmap + quad-tree LOD, editor grid/scene views, ID-buffer picking, selection outlines, and a final combine pass.
- **UI** — Immediate-mode **ImGui** tools/editors, plus **RmlUi** for data-driven in-game / panel UI.
- **Editor tooling** (`--editor`) — Scene / scenario editing, entity-template and game-rules editors, GFX representation editor, physics config UI, resource-cache and logger debug views.
- **Content pipeline** — Assimp-based **AssetCooker**, resource cache loading from loose files or **datapacks**, and **DataPacker** for shipping `Assets.datapack` / `Sounds.datapack`.
- **Audio** — miniaudio-backed sound engine with grouped sounds (e.g. music vs SFX) and async resource loading.
- **Serialization** — cereal-based save/load for configs, templates, modules, and application setup (JSON-friendly workflows).
- **Profiling & debug draw** — Tracy instrumentation; debug primitives (AABBs, OOBs, frustums, spheres, arrows, lines) in the draw command buffer.

## What this repo contains

| Path | Purpose |
|------|---------|
| `SRC/` | Engine and game code |
| `BFF/` | FASTBuild project definitions (libs, exes, solution generation) |
| `Script/` | Python helpers: clone submodules, generate `fbuild.bff`, build, cook assets, release zip |
| `External/` | Third-party deps (mostly git submodules: bgfx/bx/bimg, imgui, cereal, assimp, tracy, doctest, …) |
| `Assets/` | Content submodule. `Shared/` is meshes, shaders, sounds, and UI. `Colony/` and `Orbital/` hold each game's configs and scenes |
| `tools/` | Bundled `FBuild.exe`, `Tracy.exe` |
| `bin/` | Build outputs (generated) |
| `build/` | Intermediates / generated VS solution (generated) |

### `SRC/` modules

| Module | Role |
|--------|------|
| `ECSCore` | Entity/world/module core |
| `ECSGameplay_Common` | Gameplay modules shared by every prototype |
| `Games/Colony`, `Games/Orbital` | Per-game modules, scenario, and loader |
| `Games/ColonyLauncher`, `Games/OrbitalLauncher` | Which loaders and asset folder each executable boots with |
| `Physics` | Physics (island solver / collision; inspired by qu3e) |
| `Rendering` / `RenderingCore` | Scene rendering, picking, materials, BGFX backend |
| `UICore` | UI layer (RmlUi integration) |
| `SoundCore` / `MiniaudioSoundEngine` | Audio |
| `Application` / `Launcher` | App loop, loaders, and the shared `main` |
| `Common` / `Math` / `DataPack` | Shared utilities, math, datapack I/O |
| `imgui` / `ImGuiTools` | ImGui + editor tooling |
| `Tools/AssetCooker` | Cooks assets into engine-ready data |
| `Tools/DataPacker` | Packs cooked files into `Assets.datapack` / `Sounds.datapack` |
| `Tools/UnitTests` | doctest-based unit tests |

### Build products

| Executable | Description |
|------------|-------------|
| `bin/BuildingGame-x64-<Config>.exe` | Colony |
| `bin/Orbital-x64-<Config>.exe` | Orbital |
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

This regenerates `fbuild.bff` from your local VS install, then builds every executable listed in `Script/Generation/prototypes.json` (`BuildingGame` and `Orbital` today), plus `AssetCooker` and `DataPacker`, into `bin/`.

Custom MSBuild path if needed:

```bash
py -u Script/Compil/BuildAll.py -e -c Release -m msbuild.exe
```

### 3. Visual Studio solution (optional)

```bash
py -u Script/Compil/BuildAll.py -sln
```

Generates `build/ECSEngine.sln` (classic `.sln` + makefile-style `.vcxproj`s that call FASTBuild). Open it in Visual Studio; builds still go through `FBuild.exe`.

Set the startup project to **ColonyLauncher** or **OrbitalLauncher** (solution folder **GameLaunchers**). Each one builds that game and launches it with `--editor --nodatapack`. The working directory is `build/`. The **Colony** and **Orbital** projects compile the gameplay libraries only.

### 4. Unit tests

```bash
py -u Script/Compil/BuildAll.py -ut -c Release
```

### 5. Full pipeline (BGFX + engine + tests)

```bash
py -u Script/Compil/BuildAll.py -a -c Release
```

## Assets pipeline

Loose files are split by owner:

| Folder | Contents |
|--------|----------|
| `Assets/Shared` | Meshes, shaders, textures, sounds, UI, fonts |
| `Assets/Colony` | Colony configs and scenes (`testgameplay`) |
| `Assets/Orbital` | Orbital configs and scenes (`TestPhysicsOneObject`) |

A relative path such as `Meshes\...` or `2D\Textures\...` is resolved in the game folder first, then in `Assets/Shared`. Cooked outputs (`.gen`, `.ktx`, `.bin`, `.sha1`) are written next to the source file.

1. Clone assets (`--assets` above).
2. Build tools (`-e` or `-et`). `GenerateData.py` runs the **Release** AssetCooker.
3. Build BGFX tools once so `texturec` and `shaderc` exist (`BuildAll.py -b -c Release -t`).
4. Cook:

```bash
mkdir build   # GenerateData.py chdirs into build/
py -u Script/Generation/GenerateData.py
```

One run cooks meshes, textures, and shaders. Later runs skip files whose `.sha1` still matches.

5. Pack one game plus `Assets/Shared`. Run DataPacker from `bin/` or `build/`. The output names stay `Assets.datapack` and `Sounds.datapack`; sounds always come from `Assets/Shared/Sounds`. The default game is Colony.

```bash
cd bin
./DataPacker-x64-Release.exe --game Colony
./DataPacker-x64-Release.exe --game Orbital
```

`--assets` still overrides the game folder when both are passed. A Final game loads `Assets.datapack`, so a packed Orbital build is what `Orbital-x64-Final.exe` reads. Debug runs keep using `--nodatapack`.

### Release zip

```bash
py -u Script/FinalBuild/FinalBuild.py --game Colony
```

Builds every Final executable, cooks, packs the chosen game (default Colony), and writes a zip under `../Release/`. The zip contains that game's Final exe plus the two datapacks. `Colony` ships `BuildingGame-x64-Final.exe`. `Orbital` ships `Orbital-x64-Final.exe`.

## Run a game

From `bin/` or `build/` (both sit one level under the repo, so `..\Assets` resolves):

```bash
cd bin
./BuildingGame-x64-Debug.exe --nodatapack --editor
./Orbital-x64-Debug.exe --nodatapack --editor
```

`BuildingGame` loads `Assets/Colony` and starts `testgameplay`. `Orbital` loads `Assets/Orbital` and starts `TestPhysicsOneObject`. Both also read `Assets/Shared`.

Useful flags (non-`Final` builds where noted):

| Flag | Effect |
|------|--------|
| `--editor` | Enable editor mode |
| `--nodatapack` | Load loose files instead of datapacks. Required for Orbital |
| `--nosound` | Disable sound |
| `--assets <dir>` | Use this game asset folder instead of the executable's default |
| `--shared-assets <dir>` | Use this shared asset folder instead of `Assets/Shared` |

Profile with a `Profile` build + `tools/Tracy.exe`.

## Add a prototype

`Script/Generation/prototypes.json` lists the games. To stamp another one (PascalCase name, sources under `SRC/Games/<Name>`, launcher under `SRC/Games/<Name>Launcher`, content under `Assets/<Name>`):

```bash
py -u Script/Generation/NewPrototype.py --name MyPrototype
# --exe OtherName     # executable name, if it should not match the prototype
# --register-only     # sources already exist; only update the manifest and FASTBuild files
```

The script regenerates `BFF/Generated/Prototypes.bff` and the shared id lists, then refreshes `fbuild.bff`. It does not compile. New prototypes boot the `STANDARD` and `CAMERA` worlds. Adding a world is a manual edit of `SRC/ECSCore/WorldIds.h`.

Module, message, and scene-action ids are one list shared by every executable. Append new ids; do not reorder existing ones.

## Scaffold a gameplay module

```bash
py -u Script/CodeGenerators/GenerateModule.py -n MyModule -w Colony
# -w Common   → SRC/ECSGameplay_Common
# -w Colony   → SRC/Games/Colony
# -w Orbital  → SRC/Games/Orbital
```

`-w` is required. It must be `Common` or a name from `prototypes.json`. The script creates the header and source, registers the module, and regenerates the solution.

## License

MIT — see [LICENSE](LICENSE).
