# Oblivion Run

Oblivion Run is a 2D action-platformer game built in C with the raylib game library. The project includes a main menu, name entry screen, tutorial flow, combat, enemy patterns, health and score systems, and a game-over loop.

## Features
- Fast arcade-style combat and enemy behavior
- Health, score, and difficulty systems
- Multiple screens: menu, tutorial, gameplay, credits, and game over
- Built on raylib for lightweight 2D graphics and audio
- Windows-focused project setup using MinGW and VS Code

## Requirements
- Windows 10 or later
- MinGW / GCC installed and available on PATH
- raylib library files already included in the project under `third_party/raylib`
- VS Code with the C/C++ extension (recommended)

## Build and Run
1. Open the project in VS Code.
2. Make sure your MinGW compiler path matches the one used in `.vscode/tasks.json`.
3. Build the project using the default VS Code task: `Build Raylib App`.
4. Run the game by pressing `F5` while `src/main.c` is open.

The project is configured to output the executable to `build/main.exe`.

## Important runtime behavior
The display logic is handled automatically in `src/main.c`:
- On 1080p or smaller displays, the game opens in a borderless windowed mode.
- On larger screens, it opens a centered 1920x1080 window.

This means you do not need to manually edit window settings in the source.

## Project structure
- `src/` - main game logic and gameplay systems
- `include/` - shared headers
- `assets/` - art, sound, and game resources
- `third_party/raylib/` - local raylib library files
- `.vscode/` - editor configuration for build and debugging

## Notes
Some standard C headers such as `stdio.h`, `string.h`, and `math.h` are used throughout the project. If your compiler location differs from the current setup, update the paths in `.vscode/tasks.json` and `launch.json` before building.

## Quick start example
If your toolchain is already configured, the build command is effectively:

```bash
gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe
```

Then launch the generated `build/main.exe` file.
