# Oblivion Run — Complete Game Documentation (`docs.md`)

> 2D action-platformer / endless-runner in **C + raylib**. File: `src/main.c` → `build/main.exe`.
> Window: `1920x1080 @ 60 FPS`, `FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT`.

---

## 1. High-level: what is this game?

You run automatically to the right through a procedurally generated haunted forest (**OBLIVION RUN**). You:

1. Pick a hero name.
2. (Optionally) watch a 9-page story + controls tutorial.
3. Run / jump / dash / melee-attack endlessly while:
   - skeleton enemies chase and swing at you,
   - spikes and proximity bombs hurt you,
   - gaps in the ground kill you if you fall,
   - poison-gas fog chases you from the left,
   - health pickups (+25 on platforms, +15 from enemy drops) heal you.
4. Score = distance traveled. Die → game-over screen with top-5 high scores saved to `data/highscore.txt`.

There is no win condition — it is a survival high-score game. Difficulty ramps with distance.

---

## 2. Run / build

### Requirements
- Windows 10+, MinGW GCC on PATH, bundled raylib in `third_party/raylib/`, VS Code + C/C++ extension (recommended).

### VS Code (canonical)
- Build task: `.vscode/tasks.json` → `Build Raylib App` (default build task):
  `C:/Users/zahin/AppData/Local/Microsoft/WinGet/Packages/BrechtSanders.WinLibs.POSIX.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe/mingw64/bin/gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe`
- Debug: `.vscode/launch.json` → `Debug Raylib App` (cppdbg + gdb, `preLaunchTask: Build Raylib App`, `program: ${workspaceFolder}/build/main.exe`, `cwd: ${workspaceFolder}`).
- Run with `F5` while `src/main.c` is open. **Must launch from workspace root** — asset paths (`assets/...`) and `data/highscore.txt` are relative to CWD.

### CLI equivalent
```bash
gcc.exe -g src/main.c src/game.c src/score.c src/explosion.c src/tutorial.c src/sound.c src/player.c src/enemy.c src/ground.c src/pattern.c src/background.c src/camera.c src/texture.c src/animation.c src/health.c src/combat.c -Isrc -Iinclude -Ithird_party/raylib/include -Lthird_party/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra -o build/main.exe
./build/main.exe
```

### Display auto-logic (`src/main.c:20-29`)
- `monitor = GetCurrentMonitor()`, `mw/mh = GetMonitorWidth/Height()`.
- If `mw <= 1920 && mh <= 1080` → `ToggleBorderlessWindowed()` (fills 1080p-or-smaller screens exactly).
- Else → centered `1920x1080` window via `SetWindowPosition()`.

---

## 3. Project structure

```
Project-Z_Game/
  src/
    main.c        Entry, window, main loop, screen dispatch
    game.c/.h     Screen state machines (menu/name/tutorial/game/gameover), drawGame, restartGame, bombs
    player.c/.h   Movement, gravity, dash, melee trigger, collisions, health pickup
    enemy.c/.h    Enemy AI, pgas/fog movement + drawing, spikes damage, spawnEnemy
    combat.c/.h   Player→enemy hits, enemy→player hits, pgas contact, bomb fuse/explode
    health.c/.h   damagePlayer, updateHealth, HP UI, health drops, floating +/-HP text
    score.c/.h    High-score file I/O, game-over scores, difficulty meter
    explosion.c/.h Ring-buffer explosions (8-frame strip)
    tutorial.c/.h 9-page typewriter tutorial
    sound.c/.h    Music/SFX load, per-frame UpdateMusicStream, footsteps
    ground.c/.h   Endless ground gen, difficulty, spikes/bombs/decor spawning
    pattern.c/.h  ASCII tilemap patterns (easy/medium sets)
    background.c/.h 6-layer parallax (world + menu variants)
    camera.c/.h   One-way right-following camera
    texture.c/.h  All LoadTexture/LoadFontEx (POINT filter)
    animation.c/.h Player anim table + generic updateAnimation + pgas frame cycle
  include/
    types.h       All enums + structs + master `GS` state
    constants.h   All tuning macros (speeds, damage, sizes, timers)
  assets/
    sprites/      Player sheets (Idle, Run, JumpAndFall, Dash, Die, GroundCombo3, AirCombo2), bomb, explosion, health_item, spike_sprite, floating_platform
    enemy_sprites/Skeleton{Idle,Walk,Attack,Dead,Hit}.png
    background_elements/ BACKGROUND.png, WOODSFi/Se/Th/Fo.png, BUSH_BACKGROUND.png, floating_platform.png
    foreground_elements/ BUSH FOREGROUND 1-*.png (6), GRASS/MUSHROOM *.png (9)
    PNG/ Logo.png, pgas.png, Posionous_Smoke_Frame_*.png (12, note typo), Large_gap1/2.png, platform health.png, enemy_health_drop.png
    fonts/ Pixelmania.ttf (font1), StayPixelDEMO.ttf (font2), BoldPixels.ttf (font3), + unused
    music/ menu_background_music.mp3, game_music1.wav, jump/landing/running/dash/player_swing/enemy_swing/hurt/die/enemy_die/enemy_hurt/enemy_run/health_pickup/explosion/menu_click/menu_select/typing/heartbeat/game_over_sound.mp3 etc.
  data/highscore.txt  5 lines of `<name> <score>` (empty file = all zero)
  third_party/raylib/{include,lib}  Bundled raylib (`libraylib.a`, `libraylib.web.a`)
  .vscode/{tasks,launch,c_cpp_properties}.json
  build/main.exe  Build output
```

Module dependency direction: `main → game → {player, enemy, ground, pattern, background, camera, health, combat, score, sound, explosion, tutorial, texture, animation}`. Shared state flows via `GS*`. No globals except an unused `float timer` in `enemy.c`.

---

## 4. Master state (`include/types.h:291-377`, `GS`)

Everything lives in one `GS gs = {0}` created in `main()`:

| Group | Fields |
|---|---|
| Screen | `currentscreen: GAME=0, MENU=1, NAME_ENTRY=2, GAMEOVER=3, TUTORIAL=4, CREDITS=5` (non-sequential!) |
| Player | `player: Player`, `player_animations[8]`, `current_player_anim_name`, `current_player_state` |
| Camera | `camera: Camera2D`, `last_camera_x` (for parallax delta) |
| Ground | `gchunk[50]`, `next_spawn_point`, `chunk_index` (ring), `lastPatternEndX`, `gapBetweenTheNextPattern` |
| BG | `bgLayers[6]: parallax_layer{tex, scrollfactor, offsetX, source}` |
| Enemies | `enemy[100]` |
| Menu | `menu_selection 0..2`, `quit_game`, `playerName[25]`, `nameLetterCount`, `logo` |
| Pgas/fog | `pgas: poison_gas`, `fogpuffs[25]` |
| Combat FX | `bombs[20]+bomb_index`, `explosions[20]+explosion_index`, `healthDrops[20]+healthDrop_index`, `spikes[30]+spike_index+spike_cooldown`, `floatTexts[10]` |
| Score | `distance_traveled`, `score`, `highScores[5]`, `isNewHighScore`, `timer` (game-over delay) |
| Flow | `starting_timer (1.0 intro auto-run)`, `show_tutorial`, `pressed_how_to_play`, `skip_duration (2.0)`, `skip_pressed_timer`, `tutorial_page/charsShown/charTimer/alpha/state` |
| Decor | `bushDecor[400]+bushDecor_index+last_bush_x`, `detailDecor[200]+detail_index`, `draw_gap` |
| Misc | `cfonts{menu_font1,2,3}`, `audio: audio`, `is_game_over`, `play_walking_sound` |

Key sub-structs:
- `Player`: `position` (sprite top-left), `velocity`, `initial_position`, `width/height` (tight hitbox, not sprite), `collisionOffset`, `isgrounded`, `facing_left`, `isdashing/dashduration/dashcooldowntimer`, `isattacking/hitduration/hashitthiswing`, `invultimer`, `health/maxHealth/isDead`, `prevBottom`.
- `Enemy`: `position/velocity`, `width/height`, `facing_left`, `enemy_animations[5]`, `current_enemy_anim_name`, `state: idle/walking/attacking/hurting/dead`, `isactive`, `health/maxhealth/isdead`, `attack_cooldown`, `hashitplayerthisswing`, `invultimer`.
- `anim`: `tex, frameWidth/Height, framecount, currentframe, frameduration, frametimer, timedependent, looping, isfinished`.
- `groundChunk`: `groundChunkRect`, `hasHealthItem/healthItemRect/healthItemCollected`, `texture` (each chunk holds its own copy of floating_platform texture).
- `bomb{rect,isactive,armed,fuseTimer}`, `spike{rect,spike_sprite,isactive}`, `HealthDrop{rect,healAmount,active}`, `Explosion{position(center),timer,currentframe,active}`, `FloatingText{position,timer,maxTime,active,text[16],color}`, `fogpuff{offset,radius,speed,phase}`, `poison_gas{position,pgas_anim[12],velocity,pgas_damage,current_texture,frameduration,frametimer,attackcooldown,attacktimer}`.

Player state ordering matters: `idle < running < jumping < attacking < dashing < hurting < dead` — code uses `current_state > X` as "higher priority blocks lower" guards.

---

## 5. Main loop (`src/main.c:38-79`)

Per frame:
1. `dt = GetFrameTime()`.
2. `updateGame(&gs,&anim,dt)` — dispatches by `currentscreen` to `updateMenu / updateNameEntry / updateGameplay / updateTutorial / updateGameover`.
3. `updateMusic(&gs)` — `UpdateMusicStream(menuMusic); UpdateMusicStream(gameMusic);` every frame (required by raylib).
4. `BeginDrawing(); ClearBackground(RAYWHITE);` then:
   - `MENU → drawMenu`, `NAME_ENTRY → drawNameEntry`, `TUTORIAL → drawTutorial`,
   - `GAME → BeginMode2D(camera); drawGame(); EndMode2D(); drawHealthUI(); drawScoreHUD(); drawDifficultyMeter();`,
   - `GAMEOVER → drawGameover` (CREDITS branch is commented out).
5. `EndDrawing()`.
6. On exit: `unloadTexture, unloadenemy, unloadAudio, CloseAudioDevice, CloseWindow`.

Init (`initGame`, `src/game.c`): crosshair cursor, `loadTexture`, `logo = LoadTexture("assets/PNG/Logo.png")`, `loadAnimation`, `loadHighScores`, `currentscreen=MENU`, menu music play, player sized at `scale = 3.0*1.6 = 4.8` (`width = 17*scale`, `height = 32*scale`, `collisionOffset = {30*scale, 16*scale}`, `pos = {(80*scale)/2+200, ground_y-48*scale}`, `velocity.x=300`), camera `offset={s_width/2-200,0}, zoom=1`, health `100/100`, world `next_spawn_point=-s_width`, scrollfactors `{0.1,0.25,0.45,0.65,0.85,0.95}`, 100 enemies pre-loaded inactive, pgas at `{-800, ground_y-h+50}`, `starting_timer=1.0`.

---

## 6. Screens & flow

### MENU (`updateMenu/drawMenu`, `src/game.c`)
- Options index `0=START GAME, 1=TUTORIAL (how-to-play), 2=EXIT`.
- Keyboard: `DOWN/S` next (wrap), `UP/W` prev, `ENTER/KP_ENTER/SPACE` activate + `PlaySound(menu_click)`.
- Mouse: measures each label with `menu_font2 size 60` at `(700,h/2-50)`, `(760,h/2+25)`, `(820,h/2+100)`; hover sets selection, click activates (`menu_select` on hover change).
- Activate: `0 → NAME_ENTRY (clear name)`, `1 → TUTORIAL with pressed_how_to_play=true`, `2 → quit_game=true`.
- Draw: `drawBackgroundMenu()` + `0x000000AA` dim, 3 demo sprites (player air_attack frame2, enemy_hurt frame2 flipped, enemy_attack frame5), tiled ground strip at `player_posy+270`, `0x00000022` overlay, logo + shadow at top-center, 3 labels (selected = `"> LABEL <"` white, else dark gray).

### NAME_ENTRY
- Typing: `GetCharPressed()` loop, accept ASCII `32..125` while `count < 24`, `PlaySound(typing)` per char, null-terminate. `BACKSPACE` deletes (`menu_select`). `ENTER` with `count>0` → `menu_click`, `restartGame()`, stop menu music / play game music, go to `TUTORIAL` if `show_tutorial` else `GAME`.
- Draw: dimmed menu BG, centered `600x300` box, player idle sprite above box (`*1.6`), `"ENTER YOUR HERO NAME"` gold 30, input box `Fade(LIGHTGRAY,.6)`, name drawn in box (font3 40 black) + yellow copy above player, blinking `" _"` cursor, `"Press ENTER to Begin"` hint.

### TUTORIAL (`src/tutorial.c`, 9 pages, typewriter)
Pages: 0 story (poison gas / Oblivion Run), 1 controls (`A/D` move, `SPACE` jump, `LEFT SHIFT` dash, `LEFT CLICK` attack) + spikes/bombs warning, 2 enemy chase, 3 platform health, 4 enemy-drop health, 5 poison gas, 6 dash-gap tip (`Large_gap.png`), 7 duplicate tip (`Large_gap2.png`), 8 enjoy.
- State machine `tut_fadein → tut_typing → tut_waiting → tut_fadeout`: fade `alpha ±1.5/s`, type `0.03s/char`, `ENTER/SPACE` instant-completes typing or advances from waiting, fade-out then next page or exit (`pressed_how_to_play ? MENU : GAME`). Pre-game flow only: hold `ENTER 2s` to skip entirely (`show_tutorial=false → GAME`).
- Draw: menu BG + double dim, visible prefix `text[0..charsShown]`, manual newline split at `x=380, y=150, lineHeight=34`, font3 size 50 white with `alpha`, per-page illustration sprite when `waiting`, blinking continue prompt at bottom.

### GAME (see §7–§12)
Side-scrolling survival. HUD is screen-space (after `EndMode2D`): HP bar + hero name (top-left), `SCORE: %06d` (top-center), difficulty bar (top-right).

### GAMEOVER
- Trigger (`isGameover`): when `player.isDead && die anim isfinished`: `timer += dt` to `1.0s`, then stop game music / play menu music, `currentscreen=GAMEOVER`, `isNewHighScore = tryAddHighScore(...)`, `PlaySound(gameOverSting)`.
- Falling off world also kills: `player_has_fallen` if `playerRect.y >= ground_y + height/2` → `die` sound + `isDead=true`.
- Draw: menu BG + `Fade(BLACK,.8)`, die frame 3 `*1.8`, `"GAME OVER"` red 80, score + `"New High Score!"` + top-5 list, blinking `"Press ENTER to return to Menu"`. `ENTER → MENU`.

---

## 7. Gameplay update order (`updateGameplay`, `src/game.c`)

Order is load-bearing — keep it:

```
ShowCursor + CROSSHAIR
prevBottom = playerRect bottom
player_has_fallen | playerDashUpdate | Gravity | hitting
playerMovement | checkCeilingCollision | checkWallCollision
restrict_left_movement | groundedCheck | setplayerstate
updateJumpFrame | DamageFromSpikes | DamageFromBombs
updateExplosions | updateAnimation(player) | playerFootstepUpdate
updateHealth | checkHealthPickup | updateGround | cameraMovement
updatescore | move_pgas | updatePgasAnimation
parallax update (camera delta) | updateEnemyInvultimer
updatePlayerInvulnerability | updateCombat | updateEnemy
updateEnemyAnimations | updateHealthDropPickup
floating texts rise/fade | enemyFootstepUpdate | isGameover
```

`restartGame()` resets everything: player HP/pos/flags, camera, chunks/spikes/bombs/enemies/healthDrops/explosions/floatTexts/decor indices, pgas pos, score/timers/distance, then `updateGround()` to re-seed.

---

## 8. Player (`src/player.c`, `include/constants.h`)

Controls: `A/D` or `←/→`? No — only `A/D` move, `SPACE` jump, `LEFT_SHIFT` dash (grounded only), `LEFT_MOUSE` attack.

- Rects: `getPlayerRect() = {pos + collisionOffset, width, height}` (tight). Attack reach: 60px frontal slab `getplayerhitbox()`. Ceiling probe: top strip; wall probes: 8px side strips; ground probe: `{x+w/4, y+h, w/2, 2}`.
- `Gravity`: `velocity.y += 1400*dt`.
- `playerMovement`:
  - Intro: while `starting_timer != 0`: force `velocity.x=1000`, integrate, decrement, return (auto-run).
  - Dead/hurting: only vertical integrate (no control).
  - Dashing: full `velocity*dt` integrate, return.
  - Else: grounded `D→+1000 (700 while attacking)`, `A→-same`, air `±750`, else `0`; `SPACE && grounded → jump sound, velocity.y=-700, grounded=false`; integrate.
- `playerDashUpdate`: `cooldown-=dt`; on `SHIFT && !dashing && cooldown<=0 && grounded` → `dashing=true, dash sound, cooldown=0.6, duration=0.45, velocity.x=±2200, velocity.y=-350` (small hop). While `duration>=0` count down; expire → `dashing=false, velocity.x=0`. Blocked when `state > dashing` (hurting/dead).
- `hitting`: on `LEFT_CLICK && !attacking && !dashing`: grounded → `hitduration=0.54`, air → `0.56`, `swing` sound. Blocked when `state > attacking`.
- `setplayerstate` priority: dead → hurting (if `invul>0 && hurt !finished`) → dashing → air-attack → ground-attack → jumping (airborne) → running (`|vx|>10`) → idle. Changing anim resets `frame/timer/isfinished`.
- `updateJumpFrame`: jump anim is `timedependent=false`; manually `vy<0 → frame 0 (rise)`, `vy>250 → frame 1 (fall)`.
- `drawPlayerSprite`: `source = {frame*80, 0, facing?-w:w, 48}`, `dest = {pos, 80*4.8, 48*4.8}`, tint `RED` while hurting else `WHITE`.
- Collisions: `checkCeilingCollision` (head bump → `vy=0`, snap below), `checkWallCollision` (skip `w<=0` chunks and standing-on-top case via `prevBottom`; push out + `vx=0`), `restrict_left_movement` (`pos.x >= camera.target.x - s_width/2`), `groundedCheck` (snap to chunk top, `vy=0`; landing sound if `fallSpeed>50`).
- `checkHealthPickup`: platform health rects → `+25 clamp 100`, `health_pickup` sound, gold `+25 HP` float text.

Tuning: `pSpeed 1000, pAttackMoveSpeed 700, pSpeedAir 750, jumpSpeed 700, gravity 1400, dash 2200/0.45s/0.6cd, attack 0.54s (air 0.56s), SPRITE_SCALE 3.0 (player ×1.6 → 4.8), real hitbox 17×32`.

---

## 9. Enemies (`src/enemy.c`)

Pool: 100 pre-loaded, `isactive=false` until `spawnEnemy(x, groundY)` (first free slot; HP scales `60*(1+0.8*diff)`).

- Anims: idle 11f/0.08s loop, run 13f/0.04s loop, attack 18f/0.05s once, dead 15f/0.1s once, hurt 8f/0.08s once. Sizes from idle sheet `×3.0×1.8`.
- Draw: anchored so feet sit at `ground_y` (`dest.y = ground_y - drawH`), flipped by `facing_left`, plus HP bar (60×8, 14px above sprite; LIME→YELLOW→RED) unless dead.
- AI per frame (`updateEnemy`):
  - Despawn if fully left of `camera.left - 300`.
  - Face player (unless attacking/dead). `cooldown-=dt`. If player dead and walking/attacking → idle.
  - `idle`: `dist<=800 (aggro)` → walking.
  - `walking`: `dist<=120 (attackrange)` → attack (stop, `enemy_swing`, cooldown `0.96*(1-min(diff,0.5))`); else `vx=±900`, integrate, run anim.
  - `attacking`: `vx=0`; on finish: out-of-range → walking, else if `cooldown<=0` re-attack.
  - `hurting`: stun until hurt anim finishes, then attack-or-chase.
  - `dead`: `vx=0`; on finish → `isactive=false, isdead=true`.
- `damageEnemy(amount)`: ignores dead/invul (`0.32s`); `health-=amount`; `<=1 → dead + enemyDie sound + spawnHealthDrop(center)`; else → hurting + hurt anim.
- Footsteps: per-enemy timer, `enemy_run.mp3` every `0.35s` while walking.

---

## 10. Combat, traps, fog (`src/combat.c`, `src/health.c`)

- Player→enemy (`updateCombat`): while `isattacking`, during player frames `3..6`: if `playerHitbox ∩ enemyRect` → `damageEnemy(30)` + `hit` sound once per swing (`hashitthiswing`). Swing ends when `hitduration<=0`.
- Enemy→player: while enemy `attacking` frames `6..10`: if `enemyHitbox(50px) ∩ playerRect` and `!hashit` → `damagePlayer(5)`.
- Pgas contact: if `playerRect ∩ pgasRect` and `attacktimer==0` → `damagePlayer(10)`, reset `2.0s`.
- `damagePlayer(amount)`: ignores dead/invul (`0.04s`); `health-=amount`, red `-N HP` float text, `invul=0.04`; `<=0 → dead + die sound`; else `current=player_hurt, vx=0, hurt sound`.
- Spikes (`DamageFromSpikes`): shared `spike_cooldown 0.6s`; any active spike overlap with `cooldown==0` → `damagePlayer(25)`.
- Bombs (`DamageFromBombs`, 20-ring): proximity fuse — entering 250px radius arms (`fuse=0.6s`); leaving disarms; fuse expiry → deactivate, `spawnExplosion(center)`, `explosion` sound, `damagePlayer(30)` (no falloff, always full if armed — even if you ran away).
- Explosions (`explosion.c`, 20-ring): `currentframe++` every `0.05s` to 8 frames then off; drawn `bomb_explosion` strip `frameW=w/8`, scaled `×4`, centered.
- Health drops (`health.c`, 20-ring): enemy deaths drop `50×50` `+15`; touch → heal clamp, `health_pickup` + gold float text. Drawn with sine bob `3*sin(t*6)*8`.
- `updateHealth`: currently just `health<=0 → dead` (decay `4/s` is commented out).

---

## 11. World gen (`src/ground.c`, `src/pattern.c`)

- Chunks: 50-ring of `groundChunk`. `updateGround()`: while `next_spawn_point < player.x + s_width`: if `distSinceLastPattern >= gap` → spawn pattern, else lay plain `368px` ground tile (+10% bomb, +55% detail cluster).
- Patterns (`pattern.c`): ASCII rows, top-first, `.` empty, `G/P` ground block (182px wide), `E` enemy, `S` spike (92×80), `H` health rect, `B` bomb (defined, unused in maps). `getPatternWidth` = longest row; `spawn_pattern` converts cells: ground rows `40px` tall (bottom row stretches to screen bottom), enemies at `ground_y`, etc.
  - Easy (used while `distance < 40000`): `gapspike`, `floatingPlatform`, `spikeGauntlet`, `enemyAmbush`.
  - Medium (adds): `dashGap` (4-col gap), `dashOverSpikes`, `lowCeilingGap` (wide gap + platforms). At `distance 40000–41000` a `"!!!! DASH OVER THE GAPS !!!!"` hint is drawn (its fade math references dead `4300/6700` values — always ~constant alpha).
- Difficulty (`getDifficultyFactor`): `clamp(distance/80000, 0, 1)`; shrinks pattern gaps (`1600-800d … 2500-1000d`), speeds pgas (`280*(1+0.5d)`), shortens enemy cooldown, boosts enemy HP. Shown as green→yellow→red bar top-right.
- Score: `distance = pos.x - initial.x`, `score = distance*0.0025` (int), drawn `SCORE: %06d` top-center.
- Decor: bush line every 55px (`400`-ring, 6 sprites, scale 2.5–3.5, random flip, `last_bush_x` continuity tracker); detail clusters 2–5 pieces every ~50px on 55% of plain tiles (`200`-ring, 9 grass/mushroom sprites).

---

## 12. Camera, background, animation, audio

- Camera (`camera.c`): intro lock `target={0,0}` while `starting_timer>0`; else one-way: if `playerCenterX > target.x+450` → `target.x = center-450`. Never moves left/down/up. `restrict_left_movement` keeps player inside left edge.
- Background (`background.c`): 6 layers `BACKGROUND, WOODSFi, WOODSSe, WOODSTh, WOODSFo, BUSH_BACKGROUND` with scrollfactors `0.1…0.95`, `BG_SCALE 3.9`, tiled via `fmod(offsetX)`. World version offsets by `camera.target.x - camera.offset.x`, bottom layer pinned to `s_height-texH` and tinted `GRAY`; menu version is screen-space, all `WHITE`.
- Animation (`animation.c`): generic `updateAnimation` (skip if `!timedependent`; advance on `frameduration`; loop or clamp + `isfinished`). Player table: idle 1f/0.1 loop, run 8f/0.08 loop, jump 2f manual, dash 4f/0.08 once (sheet sliced `/6` — mismatch), attack `GroundCombo3` sliced `/14` but plays 9f/0.06 loop, air `AirCombo2` 7f/0.08 loop, die 4f/0.08 once, hurt uses **uninitialized `tex->hurt` (bug: zero-size)** 1f/2.0s once. Pgas cycles 12 frames every `0.08s`.
- Audio (`sound.c`): `load_audio` maps `assets/music/*.mp3|wav` → `menuMusic(menu_background_music, loop)`, `gameMusic(game_music1.wav, loop, vol .4, pitch .6)`, SFX `hurt/die(pitch1.2)/enemy_die/hit(=enemy_hurt)/jump(pitch1.2)/landing/dash/swing/player_swing/enemy_swing/running(.5)/enemy_run/health_pickup/explosion/menu_click/menu_select/typing`. Must call `updateMusic`每frame. Footsteps: player `running.mp3` every `0.35s` while grounded+fast; enemies same interval while walking. `unloadAudio` only frees 2 musics + 4 sounds (leaks rest).
- Textures (`texture.c`): `LoadPixelTexture` = `LoadTexture + POINT filter`. Fonts: `Pixelmania→font1, StayPixelDEMO→font2, BoldPixels→font3` (`LoadFontEx size 200`). **Two absolute paths break on other machines:** `D:\programming\raylib_practise\assets\PNG\enemy_health_drop.png` and `...platform health.png` (with space). Backslash/space filenames elsewhere are Windows-ok but fragile.

---

## 13. Controls cheat-sheet

| Input | Context | Effect |
|---|---|---|
| `D / A` | Game, grounded | Move ±1000 (700 while attacking) |
| `D / A` | Game, air | Move ±750 |
| `SPACE` | Game, grounded | Jump `vy=-700` |
| `LEFT_SHIFT` | Game, grounded, `cd<=0` | Dash `±2200 + vy=-350`, 0.45s, cd 0.6s |
| `LEFT_CLICK` | Game | Melee (0.54s ground / 0.56s air, hits frames 3–6, 30 dmg, 60px reach) |
| `ENTER / SPACE` | Tutorial | Complete typing / next page |
| Hold `ENTER 2s` | Tutorial pre-game | Skip to GAME |
| Type + `BACKSPACE` + `ENTER` | Name entry | 24-char name → start |
| `↑/W ↓/S + ENTER`, mouse | Menu | Navigate START/TUTORIAL/EXIT |
| `ENTER` | Gameover | Back to MENU |

---

## 14. Known bugs / fragility (do not "fix" silently)

1. Absolute texture paths (`texture.c`) — fails outside author's `D:\` machine.
2. `tex->hurt` never loaded → hurt anim is 0-size; `player_hurt` may render nothing.
3. Dash sheet sliced `/6` but `framecount=4`; attack sheet `/14` but plays 9 looping (should be once).
4. `unloadAudio`/`unloadTexture` leak most assets.
5. Bomb always deals full 30 on fuse expiry, even if player escaped radius.
6. `spawn_healthrect` writes to `gchunk[chunk_index]` that the next `pushgroundchunk` may overwrite — order-dependent.
7. Hint fade at 40k uses dead `4300/6700` constants; tutorial pages 6+7 text concatenated without newline; `drawScoreHUD` mixes font sizes 40/60; `gamescreen` enum non-sequential (`TUTORIAL=4, GAMEOVER=3`).
8. `data/highscore.txt` empty/missing is handled (zeros), but CWD must be workspace root.

---

## 15. File → responsibility map

| File | Owns |
|---|---|
| `main.c` | Window, init, `updateGame + updateMusic`, screen dispatch draw, shutdown |
| `game.c` | Menu/name/gameover logic, `drawGame` order, `updateGameplay` order, `restartGame`, bombs draw |
| `player.c` | Physics, state machine, dash/attack triggers, wall/ceiling/left-clamp/ground checks |
| `enemy.c` | Enemy FSM + spawn/despawn, pgas follow + fog bands/puffs, spike damage |
| `combat.c` | Hit windows (player 3–6, enemy 6–10), pgas tick, bomb fuse |
| `health.c` | HP, invul, UI bar, drops, float texts |
| `score.c` | File I/O top-5, game-over list, difficulty bar |
| `explosion.c` | Timed 8-frame FX |
| `tutorial.c` | Typewriter pages + illustrations |
| `sound.c` | Music/SFX + footsteps |
| `ground.c` | Spawner, difficulty, spikes/bombs/decor |
| `pattern.c` | Tilemap definitions + rasterizer |
| `background.c` | Parallax tiling |
| `camera.c` | Deadzone follow |
| `texture.c` | Loading (see bug §14) |
| `animation.c` | Frame tables + stepper |
| `types.h` | Structs/enums/`GS` |
| `constants.h` | Tuning (see §8–§11 for values) |
