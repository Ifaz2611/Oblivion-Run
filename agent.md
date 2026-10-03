# AGENT.md — Instructions for AI Coding Agents on DREADGROVE

This file tells an AI agent how to work safely in this repo. The game is a C + raylib 2D endless-runner. Read `docs.md` first for full game knowledge; this file is about *how to change it*.

## 1. Build / run / verify (always do this)

- Toolchain: Windows + MinGW GCC. Bundled raylib: `third_party/raylib/{include,lib}`.
- Canonical build (same as `.vscode/tasks.json` → `Build Raylib App`):
```bash
gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe
```
- Working directory MUST be the repo root (`C:\Users\zahin\Downloads\gamesss\Project-Z_Game`) — `assets/...` and `data/highscore.txt` are relative paths.
- After every code change: rebuild with `-Wall -Wextra` and fix all warnings. Then run `build/main.exe` and smoke-test: menu → name → tutorial-skip (hold ENTER) → move/jump/dash/attack → take damage → die → gameover → ENTER to menu.
- If the compiler path in `.vscode/tasks.json` doesn't exist on this machine, update it to the local `mingw64/bin/gcc.exe` (and `gdb.exe` in `launch.json`) — that is an expected edit.

## 2. Architecture invariants (do not break)

- Single master state: `GS` in `include/types.h`. Pass `GS*` everywhere; do not add globals (one legacy `float timer` in `enemy.c` is unused — leave or remove, don't copy the pattern).
- Screen enum is non-sequential: `GAME=0, MENU=1, NAME_ENTRY=2, GAMEOVER=3, TUTORIAL=4, CREDITS=5`. Never assume `+1` ordering; always use named constants.
- Player state order is semantic: `idle < running < jumping < attacking < dashing < hurting < dead`. Guards like `current_state > X` depend on it — do not reorder without updating all guards in `player.c` / `combat.c`.
- Frame update order in `updateGameplay()` (`src/game.c`) is load-bearing (physics → collisions → state → damage → world → camera → enemies → combat → gameover). Do not reorder; insert new updates adjacent to their logical group.
- Draw order in `drawGame()` is load-bearing: background → tint → chunks/health-items → spikes/bombs/drops/explosions → player → enemies → fog → float texts → (screen-space HUD in `main.c`).
- Pools are fixed ring buffers: chunks 50, enemies 100, spikes 30, bombs 20, explosions 20, healthDrops 20, floatTexts 10, bush 400, detail 200, fogpuffs 25. Reuse the `*_index = (*_index+1) % N` idiom; never `malloc` per-frame.
- Tuning lives in `include/constants.h`; types in `include/types.h`. Prefer adding a macro over a magic number.
- Textures use `POINT` filtering (`LoadPixelTexture`). Fonts: `menu_font1=Pixelmania, font2=StayPixelDEMO, font3=BoldPixels` loaded at size 200.
- Audio: `UpdateMusicStream()` must be called every frame (done in `main.c` via `updateMusic`). New music needs the same treatment; new SFX need `LoadSound` in `load_audio()` + `UnloadSound` in `unloadAudio()`.

## 3. Where to change what

| Task | Edit |
|---|---|
| Player speed/jump/dash/attack timing | `include/constants.h` only |
| Enemy HP/speed/aggro/cooldown | `constants.h` + `src/enemy.c` (`loadEnemy`, `spawnEnemy`, `updateEnemy`, `startEnemyAttack`) |
| Damage numbers / hit windows | `src/combat.c` + `constants.h` (`attackstartframe/endframe`, `enemy_attack_start/end_frame`, `*_power`, `bomb_explosion_range`, `spike_damage`) |
| New ground/obstacle layout | `src/pattern.c` (ASCII maps + `all_easy`/`all_medium`) using legend `. G P E S H B` |
| Spawn rates / gaps / difficulty curve | `src/ground.c` (`updateGround`, `getDifficultyFactor`) + `constants.h` (`difficulty_ramp_distance`) |
| Score formula / high scores | `src/game.c` (`updatescore`) + `src/score.c` |
| HUD / menus / tutorial text | `src/game.c`, `src/health.c`, `src/score.c`, `src/tutorial.c` |
| New asset | Add file under `assets/...`, load in `src/texture.c`, declare in `tex` struct (`types.h`), unload in `unloadTexture()` |
| New sound/music | Add under `assets/music/`, wire in `src/sound.c` (`load_audio` + `unloadAudio`) |

## 4. Common recipes

- Add a pattern: define `static const char* myRows[] = {...}` in `pattern.c`, wrap in `pattern myMap = {myRows, N}`, append to `all_easy_patterns` or `all_medium_patterns`, bump `pattern_count1/2`. Keep rows rectangular-ish; `G` in last row stretches to screen bottom. Test at both `distance < 40000` (easy set) and after.
- Add an enemy type: extend `Enemy`/`enemystate` in `types.h` only if needed; otherwise parametrize `spawnEnemy()` (HP/speed by difficulty already exists as template).
- Add a pickup: follow `HealthDrop` ring pattern (`spawnHealthDrop` / `updateHealthDropPickup` / `drawHealthDrops` in `health.c`) — fixed array + `active` flag + AABB check vs `getPlayerRect()`.
- Change controls: edit `player.c` (`playerMovement`, `playerDashUpdate`, `hitting`) AND update tutorial page 1 text in `tutorial.c` AND `docs.md` §13.

## 5. Do-not-do list

- Do not change `gamescreen` numeric values or `playerstate` order without a repo-wide audit.
- Do not convert relative asset paths to absolute ones. The two existing absolute paths in `texture.c` (`D:\programming\...`) are bugs to fix (port to `assets/PNG/...`), not a pattern to follow.
- Do not add per-frame allocations, blocking I/O, or new threads. High-score file I/O stays on gameover path only (`score.c`).
- Do not "fix" known bugs in `docs.md` §14 silently — confirm with the user first (e.g., bomb full-damage-after-escape, hurt-texture missing, dash/attack sheet slicing, unload leaks).
- Do not commit `build/main.exe` or edit `data/highscore.txt` content as part of code changes. Do not create new `.md` files unless asked.
- Keep raylib API usage version-pinned to `third_party/`; don't upgrade raylib or add external deps without asking.

## 6. Debugging tips

- Black/invisible sprite → check `texture.c` path (watch backslashes/spaces/typos like `Posionous`), then `animation.c` frame slicing (`frameWidth = tex.width / N` must match sheet), then `timedependent/looping` flags.
- Collision feels wrong → remember sprite `position` ≠ hitbox: hitbox = `position + collisionOffset, width × height` (`17×32 × 4.8`). Use `getPlayerRect()` / `getEnemyRect()`, never raw `position`.
- Enemy not appearing → pool exhausted (100 max) or despawned left of camera; check `spawnEnemy` TraceLog warning.
- No audio → `InitAudioDevice()` + `load_audio()` ran? `updateMusic()` called? CWD correct?
- Crash on start → almost always a missing asset path or wrong CWD. Run from repo root.

## 7. Response contract for agents

- Before editing: state which files you'll touch and why (map to §3 table).
- After editing: rebuild, report `gcc` warnings (or "clean"), and describe the smoke test performed + result.
- If a request conflicts with §2/§5, explain the conflict and propose the smallest safe alternative.
