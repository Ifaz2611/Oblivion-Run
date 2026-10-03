# Oblivion Run

Oblivion Run is a 2D action-platformer and endless survival game built in C with raylib. It is a fast, arcade-style game where the player keeps running through a haunted forest, fights enemies, avoids hazards, and tries to survive as long as possible for a high score.

This repository is not a game engine or a library. It is a complete playable game project that includes source code, assets, sound, build configuration, and a Windows-focused setup for running it locally.

## What is this project about?

You wake up in a dark forest and must survive an endless run through the ruins of a cursed world. The character automatically moves forward, and the player must:

- jump over gaps and hazards
- dash to escape danger
- attack enemies with melee strikes
- collect health pickups
- avoid poison fog, bombs, spikes, and pursuing skeletons
- survive as long as possible to beat the high score

The game includes a title screen, player name entry, tutorial flow, gameplay loop, and game-over / score screen.

## Core features

- Side-scrolling endless-runner gameplay
- 2D combat with melee attacks and dashing
- Enemy AI and varied hazards
- Health and score systems
- Difficulty scaling over time
- High-score saving to `data/highscore.txt`
- Built-in tutorial and game flow screens
- Lightweight C + raylib rendering and audio

## Controls

- A / D or Left / Right: move
- Space: jump
- Left Shift: dash
- Left Mouse Click: attack
- Enter: confirm selections and continue

## Requirements

- Windows 10 or later
- MinGW / GCC installed and available on PATH
- VS Code recommended for building/debugging
- Raylib files are already included in `third_party/raylib`

## Quick start

1. Open the project folder in VS Code.
2. Make sure your compiler is installed and available in PATH.
3. Use the default VS Code task: `Build Raylib App`.
4. Press `F5` to run, or launch the generated executable from `build/main.exe`.

## Build command

If you prefer to compile from a terminal, run this from the repository root:

```bash
gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe
```

Then run:

```bash
./build/main.exe
```

## Repository layout

- `src/` - main game code and gameplay systems
- `include/` - shared headers and state structures
- `assets/` - sprites, fonts, background images, and sound files
- `data/` - saved score data
- `third_party/raylib/` - bundled raylib library headers and binaries
- `.vscode/` - VS Code build and debug configuration
- `build/` - compiled game output

## Important notes

- The project is configured for Windows and MinGW.
- Asset paths are relative to the project root, so run the executable from the repository root or use the VS Code launch setup.
- The game will open in a window sized for the current display setup, with automatic handling in `src/main.c`.

## Why this repo is useful

This repository is ideal for:

- learning how a small C game is structured
- studying raylib-based gameplay systems
- modifying game logic, enemies, UI, or difficulty
- using as a starting point for a custom 2D action game

For step-by-step setup and troubleshooting, see `getstart.md`.
