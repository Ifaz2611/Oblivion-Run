# Getting Started with Oblivion Run

If you found this repository on GitHub, this is the fastest way to understand what it is and how to run it.

## 1. What is this project?

This repo contains a complete 2D action-platformer game called Oblivion Run.

It is a survival game where the player:

- keeps running automatically to the right
- jumps over gaps and traps
- dashes and attacks enemies
- avoids poison fog, bombs, spikes, and skeleton attackers
- collects health and tries to survive as long as possible
- aims for a high score before dying

In short: this is a playable arcade game, not just a code sample.

## 2. What technologies does it use?

- C programming language
- raylib for rendering, input, and audio
- Windows + MinGW GCC toolchain
- VS Code for editing, building, and debugging

The project includes local raylib files under `third_party/raylib`, so you do not need to install a separate external raylib package just to compile it.

## 3. Prerequisites

Before running the game, make sure you have:

- Windows 10 or newer
- MinGW-w64 or GCC installed and available on your PATH
- VS Code (recommended)
- The C/C++ extension in VS Code (recommended)

## 4. Recommended setup in VS Code

1. Open the repository in VS Code.
2. Make sure the compiler is installed and visible in the terminal:

```bash
gcc --version
```

3. Build the project using the default task named `Build Raylib App`.
4. Run with `F5` while `src/main.c` is open.

The compiled output will be generated in:

```bash
build/main.exe
```

## 5. Manual build command

From the project root, run:

```bash
gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe
```

Then start the game:

```bash
./build/main.exe
```

## 6. How the game flows

When you run the game, the normal flow is:

1. Main menu
2. Player name entry
3. Tutorial / controls screen
4. Actual endless survival gameplay
5. Game over screen with score tracking

The game also saves high scores to:

```bash
data/highscore.txt
```

## 7. Controls

- A / D or Left / Right: move
- Space: jump
- Left Shift: dash
- Left Mouse Click: attack
- Enter: confirm menu choices and continue

## 8. Project structure

```text
Project-Z_Game/
├── src/                # main game logic
├── include/            # shared headers and types
├── assets/             # sprites, fonts, music, and artwork
├── data/               # saves score data
├── third_party/        # local raylib files
├── .vscode/            # build and debug config
├── build/              # compiled executable output
├── readme.md           # project overview
├── getstart.md         # setup guide for new users
├── docs.md             # deeper technical documentation
└── agent.md            # instructions for AI coding agents
```

## 9. Troubleshooting

### Compiler not found
If `gcc` is not recognized, install MinGW and make sure `gcc.exe` is on PATH.

### Build fails
Check that:

- the project is opened from the repository root
- the correct raylib paths exist in `third_party/raylib`
- you are using a Windows-compatible GCC build

### Assets or sounds are missing
Make sure you run the game from the repo root, because asset paths are relative to the working directory.

### The game opens but behaves strangely
This game is designed around the repo's local file layout and build configuration. Use the included `.vscode/tasks.json` and `launch.json` setup when possible.

## 10. Why this repo is interesting

This is a good project if you want to:

- study a complete small game project in C
- learn raylib usage for 2D games
- tweak gameplay, enemy behavior, score systems, and UI
- build your own ideas on top of a working playable game

## 11. Recommended next step

Read `docs.md` if you want the full technical breakdown of gameplay systems, enemy behavior, and architecture.

Then open the source in `src/` and start experimenting.

Happy coding and good luck surviving Oblivion Run.
