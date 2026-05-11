# JCR Racer V4

UE5 playable prototype scaffold for a luxury 3D country-road racing demo inspired by a 2026 rear-engine sports car. No Porsche logo, crest, model badge, or trademarked branding is used.

## What Is Implemented

- Chaos Vehicles pawn base: `AJCRVehiclePawn`
- Enhanced Input runtime mapping:
  - `W` / Right Trigger: throttle
  - `S` / Left Trigger: brake
  - `A`: steer left
  - `D`: steer right
  - Left stick X: analog steering
  - `1`: toggle ABS state
  - `2`: toggle TCS state
- SpringArm chase camera with lag, rotation lag, and speed FOV from 70 to 92
- High-speed steering reduction
- 8-speed automatic setup target, 1650 kg mass, 610 Nm torque target
- Runtime HUD widget for speed, RPM, gear, timer, checkpoint, ABS/TCS
- Runtime 1 km test route:
  - village start
  - rice-field straight
  - gentle curve
  - mountain approach
  - torii pass
  - finish gate
- Runtime checkpoint time attack manager
- Placeholder headlight, brake light, dust, and exhaust component hooks
- Lumen GI/reflection renderer settings
- No-asset prototype fallback: if no skeletal vehicle mesh is assigned, the pawn shows a simple placeholder body and uses lightweight driving movement so the route can still be tested.
- Visual-quality pass inspired by high-end open-world racing references:
  - low, blue prototype sports-car silhouette with cabin, rear wing, splitter, rotating wheels, lights
  - asphalt road with yellow center paint, white edge lines, gravel shoulders, guardrails
  - denser roadside shrubs, rocks, utility poles, houses, torii gate, distant mountain ridge
  - directional sun, skylight, height fog, bloom, subtle motion blur, saturation tuning

## First-Time Setup In Unreal Editor

### Easiest Path

Double-click `SetupAndRun.bat`.

On this machine Unreal Engine was found at `E:\Epic Games\UE_5.7`, so `Run_E_Drive_Unreal.bat` is the most direct launcher.

It will:

1. Find Unreal Engine 5.
2. Check for Visual Studio C++ tools.
3. Generate Visual Studio project files.
4. Compile `JCRRacerV4Editor`.
5. Launch Unreal Editor.
6. Run `Scripts/CreatePrototypeContent.py` to create `/Game/Maps/M_JapanCountryRoad_Test`.

Then open `/Game/Maps/M_JapanCountryRoad_Test` and press Play.

After code changes, close any running Unreal Editor instance and run `Run_E_Drive_Unreal.bat` again so the freshly compiled DLL is loaded.

### Other Shortcuts

- `CompileOnly.bat`: generate project files and compile, but do not open Unreal Editor.
- `OpenEditor.bat`: open Unreal Editor without compiling first.
- `Run_E_Drive_Unreal.bat`: compile and run using `E:\Epic Games\UE_5.7`.
- `InstallVisualStudioBuildTools.bat`: opens the official Visual Studio Build Tools download page if C++ tools are missing.

### Manual Path

1. Open `JCRRacerV4.uproject` with Unreal Engine 5.7 or newer.
2. Let Unreal generate project files and compile the C++ module.
3. Run `Scripts/CreatePrototypeContent.py` from `Tools > Execute Python Script` to create `/Game/Maps/M_JapanCountryRoad_Test`.
4. In World Settings, set GameMode Override to `JCRGameMode` if it is not picked up automatically.
5. Press Play.

The game mode spawns the course builder and race manager at runtime. The default pawn is `AJCRVehiclePawn`. Without a skeletal car mesh, it still moves as a visible prototype block car; after a proper Chaos vehicle mesh and wheel blueprints are assigned, Chaos Vehicles becomes the intended production path.

## Required Vehicle Asset Step

Chaos Vehicles need a rigged skeletal vehicle mesh with wheel bones and wheel blueprints. The C++ class configures the target behavior and includes a no-asset driving fallback, but an actual production-ready vehicle mesh is still needed for full tire simulation.

Recommended Blueprint setup:

1. Create `BP_VirtualGTS_Car` in `Content/Blueprints/Vehicle`.
2. Parent class: `JCRVehiclePawn`.
3. Assign a skeletal car mesh with four wheel bones.
4. Create Chaos wheel blueprints:
   - `BP_Wheel_Front`
   - `BP_Wheel_Rear`
5. In the inherited Chaos movement component, assign wheel setups:
   - Front left/right use `BP_Wheel_Front`
   - Rear left/right use `BP_Wheel_Rear`
6. Set the project GameMode default pawn to `BP_VirtualGTS_Car`.

## Blueprint Roles

### `BP_VirtualGTS_Car`

Parent: `JCRVehiclePawn`

Main variables:

- `VehicleMassKg`: 1650
- `MaxTorqueNm`: 610
- `FrontBrakeBias`: 0.62
- `bAbsEnabled`: true
- `bTcsEnabled`: true
- `BaseFov`: 70
- `MaxFov`: 92
- `HighSpeedSteeringScale`: 0.38

Event graph:

- `BeginPlay`: optional material initialization, Niagara asset assignment
- `Tick`: optional wheel smoke intensity from speed and steering angle
- Input is already handled in C++

### `BP_RaceManager`

Parent: `JCRRaceManager`

Role:

- Tracks race timer
- Requires checkpoints in numeric order
- Stops timer at finish checkpoint

Event graph:

- Use `HandleCheckpointPassed` extension points for split sounds, UI animation, and finish celebration.

### `BP_Checkpoint`

Parent: `JCRRaceCheckpoint`

Variables:

- `CheckpointIndex`
- `bFinish`

Role:

- Box overlap trigger for time attack progression.

### `BP_JapanCountryRoadBuilder`

Parent: `JCRWorldBuilder`

Role:

- Generates the 1 km test course with placeholder geometry.
- Replace placeholder cubes with Nanite road, house, pole, wire, torii, field, and mountain meshes as art becomes available.

### `WBP_RaceHUD`

Parent: `JCRRaceHUDWidget`

Role:

- The C++ widget already displays telemetry.
- For a richer UMG layout, create a Widget Blueprint using the same telemetry functions:
  - `GetSpeedKmh`
  - `GetDisplayRpm`
  - `GetDisplayGear`
  - `IsAbsEnabled`
  - `IsTcsEnabled`
  - Race manager `GetRaceTime`
  - Race manager `GetNextCheckpointIndex`
  - Race manager `GetCheckpointCount`

## Notes

- The runtime input mapping deliberately separates `A` and `D` so `A` always produces negative steering and `D` always produces positive steering.
- The visible route geometry is intentionally placeholder-friendly. It gives you a playable spatial prototype before replacing meshes with final art.
- No trademarked Porsche branding is present.
- Korean setup notes are in `Docs/BlueprintSetup_KO.md`.
- Korean compile/run instructions are in `?�행방법_?�국??md`.
