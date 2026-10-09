# Getting started

This guide takes you from a fresh checkout to a playable build of **Oblivion
Run**. The project currently targets Windows and MinGW-w64; raylib's headers
and libraries are included in the repository.

## Prerequisites

- Windows 10 or later
- A 64-bit MinGW-w64 GCC toolchain available on `PATH`
- Git, if cloning the repository
- VS Code and its C/C++ extension (optional)

Check that the compiler is available in a new PowerShell window:

```powershell
gcc.exe --version
```

If this command is not found, install MinGW-w64 and add its `bin` directory to
your `PATH`. Use a toolchain compatible with the bundled raylib library.

## Get the source

Clone the repository and open its root folder:

```powershell
git clone https://github.com/Ifaz2611/Oblivion-Run.git
Set-Location Oblivion-Run
```

If you already have a checkout, open that directory in PowerShell instead. Keep
the repository root as the working directory when building and launching; the
game loads assets and reads/writes `data/highscore.txt` using relative paths.

## Build and run

Create the output folder (it may not exist in a fresh checkout), compile, and
launch:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe
.\build\main.exe
```

The build links against the included raylib library and the Windows OpenGL,
GDI, and multimedia libraries. Keep the compiler and bundled library
architectures compatible.

## Build and debug with VS Code

Open the repository root in VS Code and run **Terminal → Run Build Task** to
select `Build Raylib App`; use `F5` to debug with the `Debug Raylib App`
configuration.

The checked-in VS Code task and debugger configuration contain a MinGW
installation path from the original development machine. On a different
computer, update the compiler path in `.vscode/tasks.json` and the GDB path in
`.vscode/launch.json` to point to your local installation. Alternatively, use
the PowerShell build command above.

## Start playing

From the title menu, start a run and choose **Easy**, **Medium**, or **Hard**.
Enter a hero name, then follow the tutorial or skip it. The game has no final
level: survive, collect health, defeat enemies, and try to beat the local
high-score table.

| Action | Keyboard / mouse | Gamepad |
| --- | --- | --- |
| Move | `A` / `D` or `←` / `→` | Left stick or D-pad |
| Jump | `Space`, `W`, or `↑` | `A` |
| Dash | `Left Shift` or `Right Shift` | `RB` |
| Attack | Left mouse, `Down`, `X`, or `J` | `X`, `LT`, or `RT` |
| Pause / back | `Esc` | `Start` / `B` |

On-screen buttons are also drawn during gameplay for touch input. Menu
navigation uses `W` / `S`, the arrow keys, mouse, or gamepad; confirm with
`Enter` / `Space` or the gamepad confirm button. In the tutorial, press
`Enter` / `Space` to reveal or advance text. During the pre-run tutorial, hold
`Enter` to skip.

## Troubleshooting

### `gcc.exe` is not recognized

Install MinGW-w64 and add its `bin` directory to `PATH`, then restart
PowerShell or VS Code and try `gcc.exe --version` again.

### The linker cannot find raylib or reports incompatible files

Confirm that `third_party/raylib/include/raylib.h` and
`third_party/raylib/lib/libraylib.a` exist. Use a Windows MinGW-w64 compiler
compatible with the bundled library; do not substitute a Linux or macOS
toolchain.

### The build cannot create `build/main.exe`

Create the `build` directory from the repository root before compiling:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
```

### The game reports missing assets or does not save scores

Launch `.\build\main.exe` with the repository root as the working directory.
The game expects the `assets/` and `data/` paths to be relative to that folder.

### VS Code cannot find GCC or GDB

Update the local executable paths in `.vscode/tasks.json` and
`.vscode/launch.json`, or build and run from PowerShell as described above.

## Next steps

- Read **[docs.md](docs.md)** for the source layout, systems, and game-state
  overview.
- Read **[CONTRIBUTING.md](CONTRIBUTING.md)** before proposing changes.
