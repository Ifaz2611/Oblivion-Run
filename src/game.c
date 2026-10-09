    #include <math.h>
    #include <string.h>
    #include <ctype.h>
    #include "game.h"
    #include "texture.h"
    #include "animation.h"
    #include "player.h"
    #include "enemy.h"
    #include "ground.h"
    #include "background.h"
    #include "camera.h"
    #include "health.h"
    #include"combat.h"
    #include"types.h"
    #include "score.h"
    #include "sound.h"
    #include "explosion.h"
    #include"tutorial.h"
    void drawGame(GS* gs,tex *textures){
    
    //drawing background elements

    drawBackground(gs);
    drawBushLine(gs, textures);
    drawDetailDecor(gs, textures);  
    DrawRectangle(gs->camera.target.x-s_width,0,2*s_width,s_height,GetColor(0x00000080));

    for(int i=0;i<MaxChunkNum;i++){

        float width = textures->floating_platform.width;
        float height = textures->floating_platform.height;
        Rectangle chunk = gs->gchunk[i].groundChunkRect;
        Rectangle source = (Rectangle){
            .height = height,
            .width = width,
            .x = 0,
            .y = 0
        };
        Rectangle dest = (Rectangle){
            .height = chunk.height,
            .width = chunk.width,
            .x = chunk.x,
            .y = chunk.y
        };
        
        DrawTexturePro(textures->floating_platform,source,dest,(Vector2){0,0},0,WHITE);

        
        // DrawRectangleLinesEx(gs->gchunk[i].groundChunkRect,3,BLACK);
        
        
        // ei chunk e heal item thakle  and seta pick na kore
        //thakle box ta green 
        if(gs->gchunk[i].hasHealthItem && !gs->gchunk[i].healthItemCollected){
            Rectangle source ={0,0,textures->health_item.width, textures->health_item.height};

         float floatOffset =  3.0*sinf(GetTime()* 6.0f)*8.0f;

           Rectangle dest = {
            .x = gs->gchunk[i].healthItemRect.x,
            .y = gs->gchunk[i].healthItemRect.y + floatOffset,
            .width = gs->gchunk[i].healthItemRect.width,
            .height = gs->gchunk[i].healthItemRect.height
        };
        
        DrawTexturePro(textures->health_item,source,dest,(Vector2){0,0},0.0f,WHITE);    
    }      
}    

    drawSpikes(gs, textures);

    drawBombs(gs, textures);
    drawHealthDrops(gs, textures);
    drawExplosions(gs,textures);

    //drawing player sprite

        drawPlayerSprite(gs);
        // DrawRectangleLinesEx(getPlayerRect(gs),10,(gs->player.isattacking)?RED:BLUE);

    //drawing enemy sprites
        for(int i=0;i<max_enemy_num;i++){
           if(gs->enemy[i].isactive) drawEnemy(&gs->enemy[i]);
            // DrawRectangleLinesEx(getEnemyHitbox(&gs->enemy[i]),20,BLACK);
            // DrawRectangleLinesEx(getEnemyRect(&gs->enemy[i]),10,BLUE);
        }
        drawFog(gs);
        drawFogPuffs(gs,(float)GetTime());

        // drawPgasSprite(gs); 
        // drawPgasSprite(gs);
        
      for(int i=0;i<10;i++){
       if(gs->floatTexts[i].active){
        float alpha = gs->floatTexts[i].timer/  (gs->floatTexts[i].maxTime);

        Color textColor =Fade(gs->floatTexts[i].color,alpha);

       // DrawTextEx(gs->cfonts.menu_font1,gs->floatTexts[i].text,gs->floatTexts[i].position,30,0,textColor);
        
       DrawText(gs->floatTexts[i].text, (int)gs->floatTexts[i].position.x, (int)gs->floatTexts[i].position.y, 30, textColor);

       }
      }  
      if(gs->distance_traveled > 40000 && gs->distance_traveled < 41000){
        const char* hint = "!!!! DASH OVER THE GAPS !!!!";
        Vector2 size = MeasureTextEx(gs->cfonts.menu_font3, hint, 60, 0);

        float screenLeft = gs->camera.target.x - gs->camera.offset.x;   // left edge of the screen in world coords
        float x = screenLeft + s_width/2.0f - size.x/2.0f;

        float d = gs->distance_traveled;
        float alpha = 1.0f;
        if(d < 40300)      alpha = (d - 40000.0f) / 300.0f;   // fade in
        else if(d > 40700) alpha = (41000.0f - d) / 300.0f;   // fade out

        DrawTextEx(gs->cfonts.menu_font3, hint, (Vector2){x, 100}, 60, 0, Fade(LIGHTGRAY, alpha));
        }

    }

    void initGame(GS* gs,tex* tex,anim* anim){
        (void)anim;

        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);

    // load textures
        loadTexture(tex,gs);

        gs->logo = LoadTexture("assets/PNG/Logo.png");
        
    // load animations
        loadAnimation(gs,tex);
   
       loadHighScores(gs->highScores);

        //menu 
        gs->currentscreen = MENU;
        gs->menu_selection = 0; //1st e start game e select hoye tahkbe 
        gs->quit_game = false;
        gs->show_tutorial = true;
        load_audio(gs);                      
        PlayMusicStream(gs->audio.menuMusic);
        gs->skip_duration = skip_timer;
        gs->difficulty = DIFF_MEDIUM;
        gs->difficulty_selection = DIFF_MEDIUM;
        //set player
        float scale = SPRITE_SCALE * 1.6f;
        float frameW = gs->player_animations[player_idle].frameWidth;   // 80
        float frameH = gs->player_animations[player_idle].frameHeight;  // 48


        gs->player.width  = player_real_width  * scale;   // 17 * scale
        gs->player.height = player_real_height * scale;   // 32 * scale

        gs->player.collisionOffset.x = 30.0f * scale;
        gs->player.collisionOffset.y = 16.0f * scale;

        gs->player.initial_position.x = (frameW * scale) / 2.0f+200.0f;  
        gs->player.initial_position.y = ground_y - (frameH * scale);    
        
        gs->player.velocity.x = 300.0f;

        gs->player.position = gs->player.initial_position;


    //set camera 
        gs->camera.offset = (Vector2){s_width/2.0f-200.0f,0.0f};
        gs->camera.rotation = 0.0f;
        gs->camera.zoom = 1.0f;
        
        gs->camera.target = (Vector2){0.0f,0.0f};
        gs->last_camera_x = gs->camera.target.x;
        //heath function er variable gulo
        gs->player.maxHealth = PLAYER_MAX_HEALTH;
        gs->player.health = PLAYER_MAX_HEALTH;
        gs->player.isDead = false;
        
        // INITIAL GROUND / PATTERN GENERATION
        gs->chunk_index = 0;

        // Start spawning from the beginning of the world
        gs->next_spawn_point = -s_width;

        // No pattern has been spawned yet
        gs->lastPatternEndX = gs->next_spawn_point;

        // Distance before the first pattern
        gs->gapBetweenTheNextPattern = 2*s_width;

        // Generate the initial world
        updateGround(gs);
        // setup background layers — farthest (slowest apparent motion) to nearest
        float bg_scrollfactors[BG_LAYER_COUNT] = {0.1f, 0.25f, 0.45f,0.65f , 0.85f,.95f};
        
        for(int i=0;i<BG_LAYER_COUNT;i++){
            parallax_layer *l = &gs->bgLayers[i];
            l->scrollfactor = bg_scrollfactors[i];
            l->offsetX = 0.0f;
            l->source = (Rectangle){
                .x = 0, .y = 0,
                .width  = l->tex.width,
                .height = l->tex.height
            };
        }
        // loading all the enemy information at the start of the game 
        for(int i=0;i<max_enemy_num;i++){
            gs->enemy[i] = loadEnemy(tex);
            gs->enemy[i].isactive = false; // spawn_pattern() activates slots as chunks generate
        }

        // setup poison gas cloud
        gs->pgas.position = (Vector2){-800.0f,ground_y-gs->pgas.pgas_anim[0].height+50.0f};
        gs->pgas.pgas_damage = 10.0f; 
        gs->pgas.frameduration = 0.08f;
        gs->pgas.attackcooldown = 2.0f;

        initFogPuffs(gs);   
        

        gs->starting_timer = STARTING_TIMER;

        // all other properties of gs are set to zero by default
        gs->last_bush_x = gs->next_spawn_point;
        gs->last_ground_y = ground_y;
        gs->last_screen_height = (float)s_height;

    }

    void updateCheatCode(GS* gs){
        // hidden cheat: type "godmode" (or "oblivion" / "ididdqd") anytime while
        // running to toggle infinite health. Buffer keeps the last letters typed.
        int k = GetCharPressed();
        while(k > 0){
            char c = (char)k;
            if(c >= 'A' && c <= 'Z') c = (char)(c + ('a' - 'A'));
            if(c >= 'a' && c <= 'z'){
                if(gs->cheatLen < (int)sizeof(gs->cheatBuf) - 1){
                    gs->cheatBuf[gs->cheatLen++] = c;
                }else{
                    memmove(gs->cheatBuf, gs->cheatBuf + 1, sizeof(gs->cheatBuf) - 2);
                    gs->cheatBuf[sizeof(gs->cheatBuf) - 2] = c;
                }
                gs->cheatBuf[gs->cheatLen] = '\0';
                const char* codes[] = {"godmode", "oblivion", "ididdqd"};
                for(int ci = 0; ci < 3; ci++){
                    size_t cl = strlen(codes[ci]);
                    if((size_t)gs->cheatLen >= cl &&
                       strcmp(gs->cheatBuf + gs->cheatLen - cl, codes[ci]) == 0){
                        gs->godmode = !gs->godmode;
                        gs->cheatLen = 0;
                        gs->cheatBuf[0] = '\0';
                        if(gs->godmode){
                            gs->player.health = gs->player.maxHealth;
                            PlaySound(gs->audio.health_pickup);
                        }else{
                            PlaySound(gs->audio.menu_click);
                        }
                        for(int t = 0; t < 10; t++){
                            if(!gs->floatTexts[t].active){
                                gs->floatTexts[t].active = true;
                                gs->floatTexts[t].position = (Vector2){gs->player.position.x, gs->player.position.y - 60.0f};
                                gs->floatTexts[t].timer = 2.0f;
                                gs->floatTexts[t].maxTime = 2.0f;
                                const char* msg = gs->godmode ? "GOD MODE ON" : "GOD MODE OFF";
                                strncpy(gs->floatTexts[t].text, msg, sizeof(gs->floatTexts[t].text) - 1);
                                gs->floatTexts[t].text[sizeof(gs->floatTexts[t].text) - 1] = '\0';
                                gs->floatTexts[t].color = gs->godmode ? GOLD : LIGHTGRAY;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
            k = GetCharPressed();
        }
        if(gs->godmode && !gs->player.isDead){
            gs->player.health = gs->player.maxHealth;
        }
    }

    void updateGameplay(GS* gs,anim* anim,float dt){
        ShowCursor();
        SetMouseCursor(MOUSE_CURSOR_CROSSHAIR);
        bool padPause = IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT);
        if(IsKeyPressed(KEY_ESCAPE) || padPause){
            gs->currentscreen = PAUSED;
            gs->pause_selection = 0;
            return;
        }
        // hit-stop: brief world freeze on heavy impacts (shake still decays).
        if(gs->hitStopTimer > 0.0f){
            gs->hitStopTimer -= dt;
            if(gs->hitStopTimer < 0.0f) gs->hitStopTimer = 0.0f;
            updateScreenShake(gs, dt);
            return;
        }
        updateTouchButtons(gs);
        updateCheatCode(gs);
        Rectangle pr = getPlayerRect(gs);
        gs->player.prevBottom = pr.y + pr.height;
        player_has_fallen(gs);
        playerDashUpdate(gs,dt);
        Gravity(gs,dt);
        hitting(gs,dt);
        playerMovement(gs,anim,dt);
        checkCeilingCollision(gs);
        checkWallCollision(gs); 
        restrict_left_movement(gs);
        groundedCheck(gs);
        setplayerstate(gs);
        updateJumpFrame(gs);
        DamageFromSpikes(gs,dt);
        DamageFromBombs(gs,dt);
        updateExplosions(gs, dt);
        updateAnimation(&gs->player_animations[gs->current_player_anim_name],dt);
        
        playerFootstepUpdate(gs, dt);

        updateHealth(gs,dt);
        checkHealthPickup(gs); // collidrawsion ditect check 

        updateGround(gs);
        cameraMovement(gs);
        updateScreenShake(gs, dt);
        updatescore(gs);
        move_pgas(gs,dt);
        updatePgasAnimation(gs,dt);


        float cameradelta = gs->camera.target.x - gs->last_camera_x;
        updateParallax(gs,cameradelta);
        gs->last_camera_x = gs->camera.target.x;


        //enemy functions
        updateEnemyInvultimer(gs,dt);
        updatePlayerInvulnerability(gs,dt);
        updateCombat(gs,dt);
        updateEnemy(gs,dt);
        updateEnemyAnimations(gs,dt);
        updateHealthDropPickup(gs); 
        for(int i = 0; i < 10; i++){
            if(gs->floatTexts[i].active){
                gs->floatTexts[i].position.y -= 40.0f * dt; 
                //40 px kore per sec ee uthbe 
            gs->floatTexts[i].timer -= dt;              
                
                if(gs->floatTexts[i].timer <= 0) {
                    gs->floatTexts[i].active = false;       
                }
            }
        }
        enemyFootstepUpdate(gs, dt);    
        isGameover(gs,dt);

    }

void updateWorldForResize(GS* gs){
    const float screenHeight = (float)s_height;
    if(screenHeight == gs->last_screen_height) return;

    const float groundDelta = ground_y - gs->last_ground_y;
    const float heightDelta = screenHeight - gs->last_screen_height;

    gs->player.position.y += groundDelta;
    gs->player.initial_position.y += groundDelta;
    gs->player.prevBottom += groundDelta;
    gs->pgas.position.y += groundDelta;

    for(int i = 0; i < MaxChunkNum; i++){
        Rectangle *chunk = &gs->gchunk[i].groundChunkRect;
        if(chunk->width > 0.0f){
            bool reachesBottom = fabsf(chunk->y + chunk->height - gs->last_screen_height) < 1.0f;
            chunk->y += groundDelta;
            if(reachesBottom) chunk->height += heightDelta - groundDelta;
        }
        if(gs->gchunk[i].hasHealthItem) gs->gchunk[i].healthItemRect.y += groundDelta;
    }
    for(int i = 0; i < max_spikes; i++)
        if(gs->spikes[i].isactive) gs->spikes[i].rect.y += groundDelta;
    for(int i = 0; i < max_bombs; i++)
        if(gs->bombs[i].isactive) gs->bombs[i].rect.y += groundDelta;
    for(int i = 0; i < max_enemy_num; i++)
        if(gs->enemy[i].isactive) gs->enemy[i].position.y += groundDelta;
    for(int i = 0; i < max_health_drops; i++)
        if(gs->healthDrops[i].active) gs->healthDrops[i].rect.y += groundDelta;
    for(int i = 0; i < max_explosions; i++)
        if(gs->explosions[i].active) gs->explosions[i].position.y += groundDelta;
    for(int i = 0; i < 10; i++)
        if(gs->floatTexts[i].active) gs->floatTexts[i].position.y += groundDelta;
    for(int i = 0; i < max_bush_decor; i++)
        if(gs->bushDecor[i].active) gs->bushDecor[i].position.y += groundDelta;
    for(int i = 0; i < max_detail_decor; i++)
        if(gs->detailDecor[i].active) gs->detailDecor[i].position.y += groundDelta;
    for(int i = 0; i < 25; i++){
        gs->fogpuffs[i].offset.y += groundDelta;
        if(gs->fogpuffs[i].offset.y < fog_top_gap) gs->fogpuffs[i].offset.y = fog_top_gap;
        if(gs->fogpuffs[i].offset.y > screenHeight - fog_bottom_gap)
            gs->fogpuffs[i].offset.y = screenHeight - fog_bottom_gap;
    }

    gs->last_ground_y = ground_y;
    gs->last_screen_height = screenHeight;
}

void restartGame(GS* gs) {
    // Apply the player-chosen difficulty to this run.
    gs->player.maxHealth = diffPlayerMaxHp(gs->difficulty);
    gs->pgas.pgas_damage = diffPgasDmg(gs->difficulty);
    // Player reset
    gs->player.health = gs->player.maxHealth;
    gs->player.isDead = false;
    gs->player.position = gs->player.initial_position;
    gs->player.velocity = (Vector2){0, 0};
    gs->current_player_anim_name = player_idle;
    gs->player.isattacking = false;
    gs->player.isdashing = false;

//camera
    float player_center_x = gs->player.position.x + gs->player.collisionOffset.x + gs->player.width / 2.0f;
    gs->camera.target = (Vector2){player_center_x - camera_half_deadzone, 0.0f};
    gs->last_camera_x = gs->camera.target.x;

    // World & Chunks reset

    gs->chunk_index = 0;
    gs->next_spawn_point = -s_width;
    gs->lastPatternEndX = gs->next_spawn_point;
    gs->gapBetweenTheNextPattern = 2 * s_width;

    for(int i = 0; i < MaxChunkNum; i++){
        gs->gchunk[i].groundChunkRect = (Rectangle){0,0,0,0};
        gs->gchunk[i].hasHealthItem = false;
        gs->gchunk[i].healthItemCollected = false;
        gs->gchunk[i].healthItemRect = (Rectangle){0,0,0,0};
    }
    gs->spike_index = 0;
    gs->bomb_index = 0;
    gs->healthDrop_index = 0;
    gs->explosion_index = 0;
    //bomb

    for (int i = 0; i < max_bombs; i++) {
        gs->bombs[i].isactive = false;
    }
    for (int i = 0; i < max_spikes; i++) {
        gs->spikes[i].isactive = false;
    }
     for (int i = 0; i < max_enemy_num; i++) {
        gs->enemy[i].isactive = false;
    }
    //
    //
    // Poison gas reset
    gs->pgas.position = (Vector2){-200.0f, ground_y - gs->pgas.pgas_anim[0].height + 50.0f};

    gs->score = 0;
    gs->bonusScore = 0;
    gs->timer = 0.0f;
    
    gs->last_bush_x = gs->next_spawn_point;
    gs->bushDecor_index = 0;
    gs->detailDecor_index = 0;
    
    gs->starting_timer = STARTING_TIMER;   // otherwise the intro run only happens on the first game
    gs->current_player_state = idle_player;
    // death anim must be reset or the next death skips its finish-wait in isGameover()
    gs->player_animations[player_die].currentframe = 0;
    gs->player_animations[player_die].frametimer = 0.0f;
    gs->player_animations[player_die].isfinished = false;
    gs->timer = 0.0f;
    // godmode persists across retries (toggle off by retyping the code);
    // clear the letter buffer + stale touch state so nothing carries over
    gs->cheatLen = 0;
    gs->cheatBuf[0] = '\0';
    for(int i = 0; i < TB_COUNT; i++) gs->touchHeld[i] = gs->touchPrevHeld[i] = false;
    gs->player.invultimer = 0; gs->player.dashcooldowntimer = 0; gs->player.dashduration = 0;
    gs->player.hitduration = 0; gs->player.hashitthiswing = false;
    gs->spike_cooldown = 0; gs->pgas.attacktimer = 0;
    gs->shakeTime = 0; gs->shakeDuration = 0; gs->shakeMagnitude = 0;
    gs->shakeOffset = (Vector2){0, 0}; gs->hitStopTimer = 0;
    
    for(int i=0;i<max_bush_decor;i++)   gs->bushDecor[i].active = false;
    for(int i=0;i<max_detail_decor;i++) gs->detailDecor[i].active = false;
    for(int i=0;i<max_health_drops;i++) gs->healthDrops[i].active = false;
    for(int i=0;i<max_explosions;i++)   gs->explosions[i].active = false;
    for(int i=0;i<10;i++)               gs->floatTexts[i].active = false;
    gs->player.facing_left = false;
    gs->distance_traveled = 0;
    updateGround(gs);
}

static Rectangle pauseOptionRect(int index){
    const float width = 360.0f, height = 64.0f, gap = 18.0f;
    const float totalHeight = 3.0f * height + 2.0f * gap;
    return (Rectangle){(s_width - width) * 0.5f,
                       (s_height - totalHeight) * 0.5f + index * (height + gap),
                       width, height};
}

// ---- gamepad helpers for menus (pad 0 only) ----
static bool padAvail(void){ return IsGamepadAvailable(0); }
static bool padConfirm(void){
    return padAvail() && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}
static bool padBack(void){
    return padAvail() && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT) ||
                          IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT));
}
static bool padUp(void){
    return padAvail() && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP) ||
                          IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_THUMB));
}
static bool padDown(void){
    return padAvail() && (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN) ||
                          IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_THUMB));
}
static bool padLeft(void){
    return padAvail() && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
}
static bool padRight(void){
    return padAvail() && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
}

static void activatePauseSelection(GS* gs){
    PlaySound(gs->audio.menu_click);
    if(gs->pause_selection == 0){
        gs->currentscreen = GAME;
    }else if(gs->pause_selection == 1){
        restartGame(gs);
        gs->currentscreen = GAME;
    }else{
        gs->currentscreen = MENU;
        gs->menu_selection = 0;
        PlayMusicStream(gs->audio.menuMusic);
    }
}

void updatePauseMenu(GS* gs){
    ShowCursor();
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    if(IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || padBack()){
        gs->currentscreen = GAME;
        PlaySound(gs->audio.menu_click);
        return;
    }
    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || padUp()){
        gs->pause_selection = (gs->pause_selection + 2) % 3;
        PlaySound(gs->audio.menu_select);
    }
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || padDown()){
        gs->pause_selection = (gs->pause_selection + 1) % 3;
        PlaySound(gs->audio.menu_select);
    }
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE) || padConfirm()){
        activatePauseSelection(gs);
        return;
    }
    Vector2 mouse = GetMousePosition();
    for(int i = 0; i < 3; i++){
        if(!CheckCollisionPointRec(mouse, pauseOptionRect(i))) continue;
        if(gs->pause_selection != i){
            gs->pause_selection = i;
            PlaySound(gs->audio.menu_select);
        }
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) activatePauseSelection(gs);
        break;
    }
}

void drawPauseMenu(GS* gs){
    DrawRectangle(0, 0, s_width, s_height, Fade(BLACK, 0.72f));
    const char *title = "PAUSED";
    Vector2 titleSize = MeasureTextEx(gs->cfonts.menu_font2, title, 72, 0);
    Rectangle firstOption = pauseOptionRect(0);
    DrawTextEx(gs->cfonts.menu_font2, title,
               (Vector2){(s_width - titleSize.x) * 0.5f, firstOption.y - 100.0f},
               72, 0, GOLD);

    const char *labels[3] = {"RESUME", "RESTART RUN", "MAIN MENU"};
    for(int i = 0; i < 3; i++){
        Rectangle rect = pauseOptionRect(i);
        bool selected = (gs->pause_selection == i);
        DrawRectangleRounded(rect, 0.22f, 8, selected ? Fade(GOLD, 0.9f) : Fade(DARKGRAY, 0.9f));
        DrawRectangleRoundedLines(rect, 0.22f, 8, selected ? GOLD : LIGHTGRAY);
        Vector2 labelSize = MeasureTextEx(gs->cfonts.menu_font3, labels[i], 36, 0);
        DrawTextEx(gs->cfonts.menu_font3, labels[i],
                   (Vector2){rect.x + (rect.width - labelSize.x) * 0.5f,
                             rect.y + (rect.height - labelSize.y) * 0.5f},
                   36, 0, selected ? BLACK : RAYWHITE);
    }
    const char *hint = "ESC / START: Resume";
    Vector2 hintSize = MeasureTextEx(gs->cfonts.menu_font3, hint, 24, 0);
    DrawTextEx(gs->cfonts.menu_font3, hint,
               (Vector2){(s_width - hintSize.x) * 0.5f, s_height - 52.0f},
               24, 0, LIGHTGRAY);
}

void updateGame(GS* gs, anim* anim, float dt){
    updateWorldForResize(gs);
    switch(gs->currentscreen){
        case MENU: 
            updateMenu(gs); 
            break;
        case DIFFICULTY:
            updateDifficultySelect(gs);
            break;
        case NAME_ENTRY: 
            updateNameEntry(gs); 
            break;
        case GAME: 
            updateGameplay(gs, anim, dt); 
            break;
        case PAUSED:
            updatePauseMenu(gs);
            break;
        case TUTORIAL:
            updateTutorial(gs, dt);  
            break;
        case GAMEOVER:
            updateGameover(gs);
            break;
        case CREDITS:
            updateCredits(gs);
            break;
    }
}



//menu functions

static void activateMenuSelection(GS* gs) {
    PlaySound(gs->audio.menu_click);
    if (gs->menu_selection == 0) {
        // START -> difficulty picker (name step comes after it).
        gs->difficulty_selection = gs->difficulty;
        gs->currentscreen = DIFFICULTY;
    }
    else if (gs->menu_selection == 1) {
        gs->pressed_how_to_play = true;
        initTutorial(gs);
        gs->currentscreen = TUTORIAL;
    }
    else if (gs->menu_selection == 2) {
        gs->currentscreen = CREDITS;
    }
    else if (gs->menu_selection == 3) {
        gs->quit_game = true;
    }
}

void updateMenu(GS* gs) {
    ShowCursor();
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
   //down key niche toggle korar jonno
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || padDown()) {
        gs->menu_selection++;
        PlaySound(gs->audio.menu_select);
        if (gs->menu_selection > 3) gs->menu_selection = 0; // four menu options: start, tutorial, credits, exit
    }
    //up key te vice versa
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || padUp()) {
        gs->menu_selection--;
        PlaySound(gs->audio.menu_select);
        if (gs->menu_selection < 0) gs->menu_selection = 3;
    }

    //enter key (keyboard)
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE) || padConfirm()) {
        activateMenuSelection(gs);
        updateParallax(gs,5.0f);
        return;
    }

    // ---- mouse support: hover to select, left-click to confirm ----
    // Positions/sizes must match drawMenu() below.
    Vector2 mouse = GetMousePosition();
    const char* labels[4] = {
        (gs->menu_selection == 0) ? "> START GAME <" : "  START GAME  ",
        (gs->menu_selection == 1) ? "> TUTORIAL <" : "  TUTORIAL ",
        (gs->menu_selection == 2) ? "> CREDITS <" : "  CREDITS  ",
        (gs->menu_selection == 3) ? "> EXIT <" : "  EXIT  "
    };
    Vector2 positions[4] = {
        (Vector2){700.0f, s_height / 2 - 50.0f},
        (Vector2){760.0f, s_height / 2 + 25.0f},
        (Vector2){820.0f, s_height / 2 + 100.0f},
        (Vector2){880.0f, s_height / 2 + 175.0f}
    };
    for (int i = 0; i < 4; i++) {
        Vector2 size = MeasureTextEx(gs->cfonts.menu_font2, labels[i], 60, 0);
        Rectangle bounds = {positions[i].x, positions[i].y, size.x, size.y};
        if (CheckCollisionPointRec(mouse, bounds)) {
            if (gs->menu_selection != i) {
                gs->menu_selection = i;
                PlaySound(gs->audio.menu_select);
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                activateMenuSelection(gs);
                return;
            }
            break;
        }
    }
    updateParallax(gs,5.0f);
}

void drawMenu(GS* gs, tex* tex) {
    drawBackgroundMenu(gs);
    
    DrawRectangle(0,0,s_width,s_height,GetColor(0x000000AA));

    float width = tex->air_attack1.width/7.0f;
    float height = tex->air_attack1.height;
    float player_posx = s_width/2.0f-250.0f;
    float player_posy = s_height/2.0f+200.0f;
    DrawTexturePro(
        tex->air_attack1,(Rectangle){.x = 2*width,.y=0,.height = height, .width = width}, 
        (Rectangle){.x = player_posx+80.0f,.y = player_posy+50.0f,.width = width*SPRITE_SCALE*1.3f, .height = height*SPRITE_SCALE*1.3f},
        (Vector2){0,0},0,WHITE
    );

    float width2 = tex->enemy_hurt.width/8.0f;
    float height2  = tex->enemy_hurt.height;
    DrawTexturePro(
        tex->enemy_hurt,(Rectangle){.x = 2*width2,.y=0,.height = height2, .width = -width2}, 
        (Rectangle){.x = player_posx+475.0f,.y = player_posy+120.0f,.width = width2*SPRITE_SCALE*1.6f, .height = height2*SPRITE_SCALE*1.6f},
        (Vector2){0,0},0,WHITE
    );

    float width3 = tex->enemy_attack.width/18.0f;
    float height3  = tex->enemy_attack.height;
    DrawTexturePro(
        tex->enemy_attack,(Rectangle){.x = 5*width3,.y=0,.height = height3, .width = width3}, 
        (Rectangle){.x = player_posx-100.0f,.y = player_posy+90.0f,.width = width3*SPRITE_SCALE*1.6f, .height = height3*SPRITE_SCALE*1.6f},
        (Vector2){0,0},0,WHITE
    );

    float position = 0;
    float widthG = tex->floating_platform.width;
    float heightG = tex->floating_platform.height;
    while(position<=s_width){
        Rectangle source = (Rectangle){
            .height = heightG,
            .width = widthG,
            .x = 0,
            .y = 0
        };
        Rectangle dest = (Rectangle){
            .height = heightG*2.0f+40.0f,
            .width = widthG*4.0f,
            .x = position,
            .y = player_posy+270.0f
        };
        
        DrawTexturePro(tex->floating_platform,source,dest,(Vector2){0,0},0,WHITE);
        position+=widthG*4.0f;
    }
    DrawRectangle(0,0,s_width,s_height,GetColor(0x00000022));
    DrawRectangle(s_width/2.0f-gs->logo.width/2.0f,150.0f,gs->logo.width,gs->logo.height,GetColor(0x00000044));
    DrawTexture(gs->logo,s_width/2.0f-gs->logo.width/2.0f,150.0f,WHITE);

    // float widthB = tex->health_item.width;
    // float heightB = tex->health_item.height;
    // Rectangle source2 = (Rectangle){
    //     .height = heightB,
    //     .width = widthB,
    //     .x = 2*widthB,
    //     .y = 0
    // };
    // Rectangle dest2 = (Rectangle){
    //     .height = heightB/2.0f-30,
    //     .width = widthB/2.0f-30,
    //     .x = player_posx+320.0f,
    //     .y = player_posy+height*SPRITE_SCALE*1.8f-60
    // };
    
    // DrawTexturePro(tex->health_item,source2,dest2,(Vector2){0,0},0,WHITE);

    Color startColor   = (gs->menu_selection == 0) ? WHITE : DARKGRAY;
    Color tutorial_color = (gs->menu_selection == 1)? WHITE:DARKGRAY;
    Color creditsColor = (gs->menu_selection == 2) ? WHITE : DARKGRAY;
    Color exitColor    = (gs->menu_selection == 3) ? WHITE : DARKGRAY;

    const char* startText = (gs->menu_selection == 0) ? "> START GAME <" : "  START GAME  ";
    DrawTextEx(gs->cfonts.menu_font2, startText, (Vector2){700.0f, s_height / 2 - 50.0f}, 60, 0, startColor);

    const char* tutorialText = (gs->menu_selection == 1) ? "> TUTORIAL <" : "  TUTORIAL ";
    DrawTextEx(gs->cfonts.menu_font2, tutorialText, (Vector2){760.0f, s_height / 2 + 25.0f}, 60, 0, tutorial_color);

    const char* creditsText = (gs->menu_selection == 2) ? "> CREDITS <" : "  CREDITS  ";
    DrawTextEx(gs->cfonts.menu_font2, creditsText, (Vector2){820.0f, s_height / 2 + 100.0f}, 60, 0, creditsColor);

    const char* exitText = (gs->menu_selection == 3) ? "> EXIT <" : "  EXIT  ";
    DrawTextEx(gs->cfonts.menu_font2, exitText, (Vector2){880.0f, s_height / 2 + 175.0f}, 60, 0, exitColor);
}




// ---- difficulty select: picked right after START, before the run starts ----
static void beginRunAfterDifficulty(GS* gs) {
    // Difficulty is already stored in gs->difficulty; start (or continue to
    // name entry) from here so every run honors the fresh choice.
    if (gs->playerName[0] == '\0') {
        gs->currentscreen = NAME_ENTRY;
        gs->nameLetterCount = 0;
        gs->playerName[0] = '\0';
        return;
    }
    restartGame(gs);
    StopMusicStream(gs->audio.menuMusic);
    // no background music during the run — SFX only
    if (gs->show_tutorial) {
        initTutorial(gs);
        gs->currentscreen = TUTORIAL;
    } else {
        gs->currentscreen = GAME;
    }
}

static void confirmDifficultySelection(GS* gs) {
    PlaySound(gs->audio.menu_click);
    gs->difficulty = gs->difficulty_selection;
    beginRunAfterDifficulty(gs);
}

void updateDifficultySelect(GS* gs) {
    ShowCursor();
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A) || padLeft()) {
        gs->difficulty_selection--;
        if (gs->difficulty_selection < DIFF_EASY) gs->difficulty_selection = DIFF_HARD;
        PlaySound(gs->audio.menu_select);
    }
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D) || padRight()) {
        gs->difficulty_selection++;
        if (gs->difficulty_selection > DIFF_HARD) gs->difficulty_selection = DIFF_EASY;
        PlaySound(gs->audio.menu_select);
    }
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || padUp()) {
        gs->difficulty_selection--;
        if (gs->difficulty_selection < DIFF_EASY) gs->difficulty_selection = DIFF_HARD;
        PlaySound(gs->audio.menu_select);
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S) || padDown()) {
        gs->difficulty_selection++;
        if (gs->difficulty_selection > DIFF_HARD) gs->difficulty_selection = DIFF_EASY;
        PlaySound(gs->audio.menu_select);
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_SPACE) || padConfirm()) {
        confirmDifficultySelection(gs);
        updateParallax(gs, 5.0f);
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || padBack()) {
        gs->currentscreen = MENU;
        gs->menu_selection = 0;
        updateParallax(gs, 5.0f);
        return;
    }
    // mouse: hover highlights, left-click confirms
    Vector2 mouse = GetMousePosition();
    for (int i = DIFF_EASY; i <= DIFF_HARD; i++) {
        float cw = 360.0f, ch = 300.0f, gap = 60.0f;
        float totalW = 3 * cw + 2 * gap;
        float x = s_width / 2.0f - totalW / 2.0f + (float)i * (cw + gap);
        Rectangle bounds = {x, s_height / 2.0f - 80.0f, cw, ch};
        if (CheckCollisionPointRec(mouse, bounds)) {
            if (gs->difficulty_selection != i) {
                gs->difficulty_selection = i;
                PlaySound(gs->audio.menu_select);
            }
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                confirmDifficultySelection(gs);
                return;
            }
            break;
        }
    }
    updateParallax(gs, 5.0f);
}

void drawDifficultySelect(GS* gs) {
    drawBackgroundMenu(gs);
    DrawRectangle(0, 0, s_width, s_height, GetColor(0x000000AA));

    float centerX = s_width / 2.0f;
    const char* title = "CHOOSE DIFFICULTY";
    Vector2 tsize = MeasureTextEx(gs->cfonts.menu_font1, title, 90, 0);
    DrawTextEx(gs->cfonts.menu_font1, title, (Vector2){centerX - tsize.x / 2.0f, 180.0f}, 90, 0, RAYWHITE);

    const char* names[3] = {"EASY", "MEDIUM", "HARD"};
    const char* descs[3] = {
        "Chill run\n+120 HP\nWeaker foes\nSlow gas",
        "Classic run\n100 HP\nNormal foes\nNormal gas",
        "Brutal run\n80 HP\nDeadly foes\nFast gas"
    };
    Color accents[3] = {LIME, GOLD, RED};

    float cw = 360.0f, ch = 300.0f, gap = 60.0f;
    float totalW = 3 * cw + 2 * gap;
    for (int i = DIFF_EASY; i <= DIFF_HARD; i++) {
        float x = centerX - totalW / 2.0f + (float)i * (cw + gap);
        float y = s_height / 2.0f - 80.0f;
        Rectangle card = {x, y, cw, ch};
        bool sel = (gs->difficulty_selection == i);
        DrawRectangleRounded(card, 0.08f, 10, sel ? Fade(accents[i], 0.35f) : Fade(DARKGRAY, 0.85f));
        DrawRectangleRoundedLines(card, 0.08f, 10, sel ? accents[i] : LIGHTGRAY);
        if (sel) {
            Rectangle glow = {x - 6.0f, y - 6.0f, cw + 12.0f, ch + 12.0f};
            DrawRectangleRoundedLines(glow, 0.08f, 10, Fade(accents[i], 0.5f));
        }
        Vector2 nsize = MeasureTextEx(gs->cfonts.menu_font2, names[i], 60, 0);
        DrawTextEx(gs->cfonts.menu_font2, names[i],
                   (Vector2){x + cw / 2.0f - nsize.x / 2.0f, y + 25.0f}, 60, 0, sel ? WHITE : LIGHTGRAY);
        // multi-line description, centered per line
        float dy = y + 120.0f;
        char buf[128];
        strncpy(buf, descs[i], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        char* line = strtok(buf, "\n");
        while (line) {
            Vector2 lsize = MeasureTextEx(gs->cfonts.menu_font3, line, 28, 0);
            DrawTextEx(gs->cfonts.menu_font3, line,
                       (Vector2){x + cw / 2.0f - lsize.x / 2.0f, dy}, 28, 0, RAYWHITE);
            dy += 40.0f;
            line = strtok(NULL, "\n");
        }
    }

    const char* hint = "A/D or UP/DOWN: choose   |   ENTER: confirm   |   ESC: back";
    Vector2 hsize = MeasureTextEx(gs->cfonts.menu_font3, hint, 28, 0);
    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawTextEx(gs->cfonts.menu_font3, hint,
                   (Vector2){centerX - hsize.x / 2.0f, s_height - 120.0f}, 28, 0, LIGHTGRAY);
    }
}




void updateNameEntry(GS* gs) {
    //name input nibe
    int key = GetCharPressed();
    while (key > 0) {
        // only A-Z, a-z ,0-9 ,24 er letter er besi noy 
        if ((key >= 32) && (key <= 125) && (gs->nameLetterCount < 24)) {
            PlaySound(gs->audio.typing);
            gs->playerName[gs->nameLetterCount] = (char)key;
            gs->playerName[gs->nameLetterCount + 1] = '\0';
            gs->nameLetterCount++;
        }
        key = GetCharPressed();
    }

    // Backspace chaple okkhor muche  jabe 
    if (IsKeyPressed(KEY_BACKSPACE)) {
        PlaySound(gs->audio.menu_select);
        gs->nameLetterCount--;
        if (gs->nameLetterCount < 0) gs->nameLetterCount = 0;
        gs->playerName[gs->nameLetterCount] = '\0';
    }

    // ESC chaple menu te ferot (nam na likhei ber হওয়া jabe)
    if (IsKeyPressed(KEY_ESCAPE)) {
        gs->currentscreen = MENU;
        gs->menu_selection = 0;
        updateParallax(gs, 5.0f);
        return;
    }

    // ENTER chaple game start at least 1 ta letter likhtei hobe
    if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) && gs->nameLetterCount > 0) {
        PlaySound(gs->audio.menu_click); 
        restartGame(gs);
        StopMusicStream(gs->audio.menuMusic);
        // no background music during the run — SFX only
        if (gs->show_tutorial) {
            initTutorial(gs);
            gs->currentscreen = TUTORIAL;
        } else {
            gs->currentscreen = GAME;
        }
        EnableCursor();
    }

    updateParallax(gs,5.0f);
}

void drawNameEntry(GS* gs) {
    drawBackgroundMenu(gs);
    DrawRectangle(0,0,s_width,s_height,GetColor(0x000000AA));
   


    // rounded ekta box majhe 
    float boxWidth = 600.0f;
    float boxHeight = 300.0f;
    float boxX = (s_width / 2) - (boxWidth / 2);
    float boxY = (s_height / 2) - (boxHeight / 2);
    
    Texture2D tex = gs->player_animations[player_idle].tex;
    float player_posx = boxX + boxWidth/2.0f - tex.width/2.0f-150.0f;
    float player_posy = boxY-tex.height-80.0f;
    DrawTexturePro(
        tex,(Rectangle){.x = 0,.y=0,.height = tex.height, .width = tex.width}, 
        (Rectangle){.x = player_posx,.y = player_posy,.width = tex.width*SPRITE_SCALE*1.6f, .height = tex.height*SPRITE_SCALE*1.6f},
        (Vector2){0,0},0,WHITE
    );
    // DrawRectangleRounded((Rectangle){boxX, boxY, boxWidth, boxHeight}, 0.1f, 10, Fade(DARKGRAY, 0.9f));
    // DrawRectangleRoundedLines((Rectangle){boxX, boxY, boxWidth, boxHeight}, 0.1f, 10, GOLD);

    // title text 
    const char* title = "ENTER YOUR HERO NAME";
    Vector2 titleSize = MeasureTextEx(gs->cfonts.menu_font2, title, 30, 0);
    DrawTextEx(gs->cfonts.menu_font2,title, (Vector2){(s_width / 2.0f) - (titleSize.x / 2.0f), boxY + 40},30, 0, GOLD);

    // white color er name input deyar box
    DrawRectangle(boxX + 50, boxY + 120, boxWidth - 100, 60, Fade(LIGHTGRAY,.6f));
    DrawRectangleLines(boxX + 50, boxY + 120, boxWidth - 100, 60, Fade(LIGHTGRAY,.6f));

    // type kora player name 
    DrawTextEx(gs->cfonts.menu_font3,gs->playerName, (Vector2){boxX + 70, boxY + 135,}, 40,0, BLACK);

    Vector2 nameSize = MeasureTextEx(gs->cfonts.menu_font3, gs->playerName, 40, 0);
    DrawTextEx(gs->cfonts.menu_font3,gs->playerName, (Vector2){player_posx+tex.width/2.0f-nameSize.x/2.0f+160.0f, player_posy,}, 40,0, YELLOW);


    // cursor blink 
    if ((int)(GetTime() * 3) % 2 == 0 && gs->nameLetterCount < 24) {
        int textW = (int)MeasureTextEx(GetFontDefault(), gs->playerName, 40, 0).x;
        DrawText(" _", boxX + 75 + textW, boxY + 135, 40, BLACK);
    }

    // instruction text 
    const char* instruction = "Press ENTER to Begin";
    Vector2 instructionSize = MeasureTextEx(gs->cfonts.menu_font2, instruction, 20, 0);
    DrawTextEx(gs->cfonts.menu_font2,instruction, (Vector2){(s_width / 2.0f) - (instructionSize.x / 2.0f), boxY + 230}, 20, 0,LIGHTGRAY);

    // show the difficulty picked on the previous screen
    {
        const char* dname = (gs->difficulty == DIFF_EASY) ? "EASY" : (gs->difficulty == DIFF_HARD) ? "HARD" : "MEDIUM";
        Color dcolor = (gs->difficulty == DIFF_EASY) ? LIME : (gs->difficulty == DIFF_HARD) ? RED : GOLD;
        const char* dtext = TextFormat("DIFFICULTY: %s", dname);
        Vector2 dsize = MeasureTextEx(gs->cfonts.menu_font3, dtext, 26, 0);
        DrawTextEx(gs->cfonts.menu_font3, dtext, (Vector2){(s_width / 2) - dsize.x / 2.0f, boxY + boxHeight + 12.0f}, 26, 0, dcolor);
    }
}





//game over functions
void isGameover(GS* gs, float dt){
    if(gs->player.isDead && gs->currentscreen != GAMEOVER && gs->player_animations[gs->current_player_anim_name].isfinished){
        gs->timer += dt;
        if(gs->timer >= 1.0f) {
            PlayMusicStream(gs->audio.menuMusic);
            gs->currentscreen = GAMEOVER; 
            gs->isNewHighScore = tryAddHighScore(gs->highScores, gs->playerName, gs->score);
            PlaySound(gs->audio.gameOverSting);
        }
    }
}

// void drawGameover(GS* gs){
//     drawBackgroundMenu(gs);
//     DrawRectangle(0,0,s_width,s_height,Fade(BLACK,.6f));
//     anim* a = &gs->player_animations[player_die];
//     DrawTexturePro(
//         a->tex,
//         (Rectangle){
//             .x = 3*a->frameWidth,
//             .y = 0,
//             .width = a->frameWidth,
//             .height = a->frameHeight
//         },
//         (Rectangle){
//             .x = s_width/2.0f-a->frameWidth/2.0f-160,
//             .y = s_height/2.0f-a->frameHeight-380,
//             .height = a->frameHeight*SPRITE_SCALE*1.8f,
//             .width = a->frameWidth*SPRITE_SCALE*1.8f
//         },
//         (Vector2){0,0},
//         0,WHITE
//     );
//     const char* gameover = "GAME OVER";
//     int text_width = MeasureText(gameover,80);
//     DrawTextEx(gs->cfonts.menu_font2,gameover,(Vector2){s_width/2.0f-text_width/2.0f,s_height/2.0f},80,0,RED);

//     drawGameOverScores(gs,gs->isNewHighScore);
// }

void player_has_fallen(GS* gs){
    if(gs->player.isDead) return;
    if(getPlayerRect(gs).y>=ground_y+gs->player.height/2.0f){
        PlaySound(gs->audio.die);
        gs->player.health = 0.0f;
        gs->player.isDead = true;
    }
}

void updatescore(GS* gs){
    gs->distance_traveled = gs->player.position.x -gs->player.initial_position.x;
    gs->score = (int)(gs->distance_traveled*SCORE_PER_DISTANCE) + gs->bonusScore;
}

void drawScoreHUD(const GS* gs) {
    const char* scoreText = TextFormat("SCORE: %06d", gs->score);

    Vector2 textSize = MeasureTextEx(gs->cfonts.menu_font3, scoreText, 40, 0);
    
    float scoreX = s_width/2.0f-textSize.x/2.0f+200.0f; 
    float scoreY = 30.0f; 


    DrawTextEx(gs->cfonts.menu_font3, scoreText, (Vector2){scoreX + 3, scoreY + 3}, 40, 0, Fade(BLACK, 0.6f));
    DrawTextEx(gs->cfonts.menu_font3, scoreText, (Vector2){scoreX, scoreY}, 40, 0, WHITE);
}



void updateGameover(GS* gs){
    // ENTER / R / pad-A = instant retry with the same hero name (no menu, no retyping).
    // ESC / BACKSPACE / M / pad-B = back to menu.
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER) || IsKeyPressed(KEY_R) || padConfirm()) {
        PlaySound(gs->audio.menu_click);
        restartGame(gs);
        StopMusicStream(gs->audio.menuMusic);
        gs->currentscreen = GAME;
        return;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_M) || padBack()) {
        gs->currentscreen = MENU;
        gs->menu_selection = 0;
        PlayMusicStream(gs->audio.menuMusic);
    }
    updateParallax(gs, 1.5f);
}


void drawGameover(GS* gs){
    drawBackgroundMenu(gs);
    DrawRectangle(0,0,s_width,s_height,Fade(BLACK,.8f)); 
    
    anim* a = &gs->player_animations[player_die];
    DrawTexturePro(
        a->tex,
        (Rectangle){
            .x = 3*a->frameWidth,
            .y = 0,
            .width = a->frameWidth,
            .height = a->frameHeight
        },
        (Rectangle){
            .x = s_width/2.0f-a->frameWidth/2.0f-160,
            .y = s_height/2.0f-a->frameHeight-380,
            .height = a->frameHeight*SPRITE_SCALE*1.8f,
            .width = a->frameWidth*SPRITE_SCALE*1.8f
        },
        (Vector2){0,0},
        0,WHITE
    );
    
    
    const char* gameover = "GAME OVER";
    Vector2 goSize = MeasureTextEx(gs->cfonts.menu_font3, gameover, 80, 0);
    DrawTextEx(gs->cfonts.menu_font3, gameover, (Vector2){(s_width/2.0f) - (goSize.x/2.0f), 80}, 80, 0, RED);

    drawGameOverScores(gs, gs->isNewHighScore);

  const char* instruction = "ENTER/R: Retry   |   ESC: Menu (name is saved)";
    Vector2 instSize = MeasureTextEx(gs->cfonts.menu_font3, instruction, 30, 0);
    
  
    if ((int)(GetTime() * 2) % 2 == 0) {
        DrawTextEx(gs->cfonts.menu_font3, instruction, (Vector2){(s_width/2.0f) - (instSize.x/2.0f), s_height - 100}, 30, 0, LIGHTGRAY);
    }
}



void updateCredits(GS* gs) {
    // esc ba enter chaple abar menu te ferot jabe
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_BACKSPACE) || padConfirm() || padBack()) {
        gs->currentscreen = MENU;
        gs->menu_selection = 0;
    }
    updateParallax(gs, 5.0f); // background scroll korar jonno
}

void drawCredits(GS* gs, tex* textures) {
    drawBackgroundMenu(gs);
    DrawRectangle(0, 0, s_width, s_height, Fade(BLACK, 0.85f));

    float centerX = s_width / 2.0f;

    const char* title = "DEVELOPED BY";
    Vector2 titleSize = MeasureTextEx(gs->cfonts.menu_font1, title, 90, 0);
    DrawTextEx(gs->cfonts.menu_font1, title, (Vector2){centerX - (titleSize.x / 2.0f), 200}, 90, 0, RAYWHITE);

    float photoY = 380.0f;
    float photoSize = 280.0f;
    float photoX = centerX - (photoSize / 2.0f);

    if (textures->creditor_photo.id != 0) {
        Rectangle photoSource = { 0.0f, 0.0f, (float)textures->creditor_photo.width, (float)textures->creditor_photo.height };
        Rectangle photoDest = { photoX, photoY, photoSize, photoSize };
        DrawTexturePro(textures->creditor_photo, photoSource, photoDest, (Vector2){0,0}, 0.0f, WHITE);
    }
    Rectangle photoBox = { photoX, photoY, photoSize, photoSize };
    DrawRectangleLinesEx(photoBox, 4.0f, LIGHTGRAY);

    const char* name = "IFAZ MD ZAHIN";
    Vector2 nSize = MeasureTextEx(gs->cfonts.menu_font2, name, 40, 0);
    DrawTextEx(gs->cfonts.menu_font2, name, (Vector2){centerX - (nSize.x / 2.0f), photoY + photoSize + 20}, 40, 0, RAYWHITE);

    float bottomY = s_height - 180.0f;

    const char* soundCredit = "Sound Credit : pixabay";
    Vector2 scSize = MeasureTextEx(gs->cfonts.menu_font2, soundCredit, 28, 0);
    DrawTextEx(gs->cfonts.menu_font2, soundCredit, (Vector2){centerX - (scSize.x / 2.0f), bottomY}, 28, 0, LIGHTGRAY);

    const char* spriteCredit = "Sprite Credit : itch.io";
    Vector2 spSize = MeasureTextEx(gs->cfonts.menu_font2, spriteCredit, 28, 0);
    DrawTextEx(gs->cfonts.menu_font2, spriteCredit, (Vector2){centerX - (spSize.x / 2.0f), bottomY + 35}, 28, 0, LIGHTGRAY);

    const char* back = "Press ESC or ENTER to return";
    Vector2 backSize = MeasureTextEx(gs->cfonts.menu_font2, back, 22, 0);
    DrawTextEx(gs->cfonts.menu_font2, back, (Vector2){centerX - (backSize.x / 2.0f), s_height - 50}, 22, 0, DARKGRAY);
}