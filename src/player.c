#include"player.h"
#include "raymath.h"
#include"types.h"
#include"enemy.h"
#include<stdio.h>
#include<string.h>
#include<math.h>
#include"health.h"
void setplayerstate(GS* gs){
    playerstate newstate;
    Player* p = &gs->player;
    anim_name newanim;
    if(p->isDead){
        newanim = player_die;
        newstate = dead_player;
    }else if(p->invultimer>0.0f && gs->current_player_anim_name==player_hurt && !gs->player_animations[gs->current_player_anim_name].isfinished){
        newanim = player_hurt;
        newstate = hurting_player;
    }else if(p->isdashing){
        newanim = player_dash;
        newstate = dashing_player;
    }else if(p->isattacking && !p->isgrounded){
        newanim = airattack;
        newstate = attacking_player;
    }else if(p->isattacking){
        newanim = attack;
        newstate = attacking_player;
    }
    else if(!p->isgrounded){
        newanim = player_jump;
        newstate = jumping_player;
    }
    else if (fabs(p->velocity.x)>10.0f){
        newanim = player_running;
        newstate = running_player;

    }else {
        newanim = player_idle;
        newstate = idle_player;
    }
    if(newanim!=gs->current_player_anim_name){
        gs->current_player_anim_name = newanim;
        gs->player_animations[newanim].currentframe = 0;
        gs->player_animations[newanim].frametimer = 0;
        gs->player_animations[newanim].isfinished =false;
    }
    if(gs->current_player_state!=newstate) gs->current_player_state = newstate;
}

Rectangle getPlayerRect(GS* gs){
    return (Rectangle){
        gs->player.position.x + gs->player.collisionOffset.x,
        gs->player.position.y + gs->player.collisionOffset.y,
        gs->player.width,
        gs->player.height
    };
}
float getPlayerCenterX(GS* gs){
    return gs->player.position.x + gs->player.collisionOffset.x + gs->player.width/2.0f;
}

void drawPlayerSprite(GS* gs){
    anim *a = &gs->player_animations[gs->current_player_anim_name];

    Rectangle source = {
        .x = a->currentframe * a->frameWidth,
        .y = 0,
        .width  = (gs->player.facing_left) ? -a->frameWidth : a->frameWidth,
        .height = a->frameHeight
    };
    Rectangle dest = {
        .x = gs->player.position.x,
        .y = gs->player.position.y,
        .width  = a->frameWidth  * SPRITE_SCALE*1.6f,
        .height = a->frameHeight * SPRITE_SCALE*1.6f
    };
    DrawTexturePro(a->tex, source, dest, (Vector2){0,0}, 0.0f, (gs->current_player_state == hurting_player)?RED:WHITE);
}

Rectangle getTouchBtnRect(touchbtn b){
    float s = 100.0f;
    float y = (float)GetScreenHeight() - s - 30.0f;
    float w = (float)GetScreenWidth();
    switch(b){
        case TB_LEFT:   return (Rectangle){20.0f, y, s, s};
        case TB_RIGHT:  return (Rectangle){140.0f, y, s, s};
        case TB_DASH:   return (Rectangle){w - 360.0f, y, s, s};
        case TB_JUMP:   return (Rectangle){w - 240.0f, y, s, s};
        case TB_ATTACK: return (Rectangle){w - 120.0f, y, s, s};
        default:        return (Rectangle){0, 0, 0, 0};
    }
}

bool isMouseOnAnyTouchBtn(void){
    Vector2 m = GetMousePosition();
    for(int i = 0; i < TB_COUNT; i++){
        if(CheckCollisionPointRec(m, getTouchBtnRect((touchbtn)i))) return true;
    }
    return false;
}

static bool pointerDownOnBtn(touchbtn b){
    Rectangle r = getTouchBtnRect(b);
    Vector2 m = GetMousePosition();
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(m, r)) return true;
    int tc = GetTouchPointCount();
    for(int i = 0; i < tc; i++){
        if(CheckCollisionPointRec(GetTouchPosition(i), r)) return true;
    }
    return false;
}

void updateTouchButtons(GS* gs){
    for(int i = 0; i < TB_COUNT; i++){
        gs->touchPrevHeld[i] = gs->touchHeld[i];
        gs->touchHeld[i] = pointerDownOnBtn((touchbtn)i);
    }
}

static bool touchEdge(GS* gs, touchbtn b){
    return gs->touchHeld[b] && !gs->touchPrevHeld[b];
}

static bool gamepadActive(void){
    return IsGamepadAvailable(0);
}

static float gamepadMoveAxis(void){
    if(!gamepadActive()) return 0.0f;
    float ax = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
    if(fabsf(ax) < GAMEPAD_DEADZONE) ax = 0.0f;
    return ax;
}

static bool gamepadLeftHeld(void){
    if(!gamepadActive()) return false;
    if(gamepadMoveAxis() < -GAMEPAD_DEADZONE) return true;
    return IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
}

static bool gamepadRightHeld(void){
    if(!gamepadActive()) return false;
    if(gamepadMoveAxis() > GAMEPAD_DEADZONE) return true;
    return IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
}

static bool gamepadJumpPressed(void){
    if(!gamepadActive()) return false;
    return IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
}

static bool gamepadDashPressed(void){
    if(!gamepadActive()) return false;
    return IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_1) ||
           IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
}

static bool gamepadAttackPressed(void){
    if(!gamepadActive()) return false;
    return IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) ||
           IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_TRIGGER_2) ||
           IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_TRIGGER_2);
}

static bool moveLeftHeld(GS* gs){
    return IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT) || gs->touchHeld[TB_LEFT] || gamepadLeftHeld();
}

static bool moveRightHeld(GS* gs){
    return IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT) || gs->touchHeld[TB_RIGHT] || gamepadRightHeld();
}

static bool jumpPressedNow(GS* gs){
    (void)gs;
    return IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || touchEdge(gs, TB_JUMP) || gamepadJumpPressed();
}

static bool dashPressedNow(GS* gs){
    (void)gs;
    return IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT) || touchEdge(gs, TB_DASH) || gamepadDashPressed();
}

static bool attackPressedNow(GS* gs){
    bool mouseAtk = IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isMouseOnAnyTouchBtn();
    return mouseAtk || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_X) || IsKeyPressed(KEY_J) || touchEdge(gs, TB_ATTACK) || gamepadAttackPressed();
}

void drawTouchControls(GS* gs){
    (void)gs;
    const char* labels[TB_COUNT] = {"<", ">", "^", "DSH", "ATK"};
    const char* hints[TB_COUNT] = {"A/<", "D/>", "SPC/UP", "SHIFT", "CLK/DN"};
    for(int i = 0; i < TB_COUNT; i++){
        Rectangle r = getTouchBtnRect((touchbtn)i);
        bool held = gs->touchHeld[i];
        Color bg = held ? Fade(GOLD, 0.55f) : Fade(RAYWHITE, 0.22f);
        Color border = held ? GOLD : Fade(RAYWHITE, 0.6f);
        DrawRectangleRounded(r, 0.25f, 8, bg);
        DrawRectangleRoundedLines(r, 0.25f, 8, border);
        int fs = (i == TB_DASH || i == TB_ATTACK) ? 28 : 48;
        Vector2 sz = MeasureTextEx(GetFontDefault(), labels[i], (float)fs, 0);
        DrawText(labels[i], (int)(r.x + r.width/2.0f - sz.x/2.0f), (int)(r.y + 12.0f), fs, WHITE);
        Vector2 hz = MeasureTextEx(GetFontDefault(), hints[i], 16.0f, 0);
        DrawText(hints[i], (int)(r.x + r.width/2.0f - hz.x/2.0f), (int)(r.y + r.height - 24.0f), 16, LIGHTGRAY);
    }
}

void updateDashGhosts(GS* gs, float dt){
    for(int i = 0; i < MAX_DASH_GHOSTS; i++){
        if(!gs->dashGhosts[i].active) continue;
        gs->dashGhosts[i].alpha -= DASH_GHOST_FADE_SPEED * dt;
        if(gs->dashGhosts[i].alpha <= 0.0f){
            gs->dashGhosts[i].alpha = 0.0f;
            gs->dashGhosts[i].active = false;
        }
    }
}

void drawDashGhosts(GS* gs){
    Texture2D tex = gs->player_animations[player_dash].tex;
    for(int i = 0; i < MAX_DASH_GHOSTS; i++){
        if(!gs->dashGhosts[i].active) continue;
        DashGhost* g = &gs->dashGhosts[i];
        Rectangle dest = {
            .x = g->position.x,
            .y = g->position.y,
            .width = g->source.width * SPRITE_SCALE * 1.6f,
            .height = g->source.height * SPRITE_SCALE * 1.6f
        };
        Rectangle src = g->source;
        if(g->facing_left) src.width = -src.width;
        Color tint = Fade(SKYBLUE, g->alpha * 0.60f);
        DrawTexturePro(tex, src, dest, (Vector2){0,0}, 0.0f, tint);
    }
}

void playerDashUpdate(GS* gs,float dt){
    if(gs->player.isDead || gs->current_player_state>dashing_player) return;
    Player* a = &gs->player;

    if(a->dashcooldowntimer>0) a->dashcooldowntimer-=dt;

    if(dashPressedNow(gs) && !a->isdashing && a->dashcooldowntimer<=0 && a->isgrounded){
        a->isdashing = true;
        PlaySound(gs->audio.dash);
        a->dashcooldowntimer = dash_cooldowntimer;
        a->dashduration = dash_duration;
        a->velocity.x = (a->facing_left)? -dash_speed : dash_speed;
        a->velocity.y = -350.0f;
        gs->dashGhostSpawnTimer = 0.0f;
    }
    if(a->isdashing && a->dashduration>=0){
        a->dashduration-=dt;

        gs->dashGhostSpawnTimer += dt;
        if(gs->dashGhostSpawnTimer >= DASH_GHOST_SPAWN_INTERVAL){
            gs->dashGhostSpawnTimer = 0.0f;
            anim* dashAnim = &gs->player_animations[player_dash];
            for(int gi = 0; gi < MAX_DASH_GHOSTS; gi++){
                if(!gs->dashGhosts[gi].active){
                    gs->dashGhosts[gi].active = true;
                    gs->dashGhosts[gi].position = a->position;
                    gs->dashGhosts[gi].source = (Rectangle){
                        (float)(dashAnim->currentframe * dashAnim->frameWidth),
                        0.0f,
                        (float)dashAnim->frameWidth,
                        (float)dashAnim->frameHeight
                    };
                    gs->dashGhosts[gi].alpha = 0.75f;
                    gs->dashGhosts[gi].facing_left = a->facing_left;
                    break;
                }
            }
        }
        
        if(a->dashduration<=0){
            a->isdashing = false;
            a->velocity.x = 0;
        }
    }
}
Rectangle getplayerhitbox(GS* gs){
    Player* p = &gs->player;
    Rectangle body = getPlayerRect(gs);
    float reach = 60.0f;
    float x = p->facing_left ?body.x - reach: body.x + body.width;
    return (Rectangle){ x, body.y, reach, body.height };
}

void hitting(GS* gs,float dt){
    if(gs->player.isDead || gs->current_player_state>attacking_player) return;

    Player* p = &gs->player;
    (void)dt;
    if(attackPressedNow(gs) && !p->isattacking && !p->isdashing && p->isgrounded){
        p->isattacking = true;
        PlaySound(gs->audio.swing);
        p->hitduration = attackduration;
    }else if(attackPressedNow(gs) && !p->isattacking && !p->isdashing && !p->isgrounded){
        p->isattacking = true;
        PlaySound(gs->audio.swing);
        p->hitduration = airattackduration;

    }
}

void updateJumpFrame(GS* gs){
    if(gs->current_player_state>jumping_player) return;
    if(gs->player.isgrounded && gs->current_player_anim_name != player_jump) return;
    else {
        if(gs->player.velocity.y<0) gs->player_animations[player_jump].currentframe=0;
        else if(gs->player.velocity.y>250.0f) gs->player_animations[player_jump].currentframe=1;
    }
}

void restrict_left_movement(GS* gs){
    if(gs->player.position.x<gs->camera.target.x-s_width/2.0f){
        gs->player.position.x = gs->camera.target.x-s_width/2.0f;
    }
}


void Gravity(GS* gs,float dt){
    gs->player.velocity.y += gravity*dt;
}
void playerMovement(GS* gs,anim* anim,float dt){
    (void)anim;

    if(gs->starting_timer!=0){
        gs->player.velocity.x = 1000.0f;
        gs->player.position = (Vector2)Vector2Add(gs->player.position,Vector2Scale(gs->player.velocity,dt));
        gs->starting_timer -= dt;
        if(gs->starting_timer<=0) gs->starting_timer = 0.0f;
        return;
    }

    if(gs->player.isDead || gs->current_player_state==hurting_player){
        gs->player.position.y = gs->player.position.y + gs->player.velocity.y*dt;
        return;
    }

    if(gs->player.isdashing){ 
        gs->player.position = (Vector2)Vector2Add(gs->player.position,Vector2Scale(gs->player.velocity,dt));
        return;
    }
    if(moveRightHeld(gs) && gs->player.isgrounded){

        if(gs->player.isattacking) gs->player.velocity.x = pAttackMoveSpeed;
        else gs->player.velocity.x = pSpeed;

        gs->player.facing_left=false;

    }
    else if(moveRightHeld(gs) && !gs->player.isgrounded){
        gs->player.velocity.x = pSpeedAir;
        gs->player.facing_left=false;
    }

    else if(moveLeftHeld(gs)&& gs->player.isgrounded ) {
        if(gs->player.isattacking) gs->player.velocity.x = -pAttackMoveSpeed;
        else gs->player.velocity.x = -pSpeed;
        gs->player.facing_left=true;
    }
    else if(moveLeftHeld(gs) && !gs->player.isgrounded){
        gs->player.velocity.x = -pSpeedAir;
        gs->player.facing_left=true;
    }

    else{ 
        gs->player.velocity.x=0;
    }


    // coyote time update
    if(gs->player.isgrounded){
        gs->player.coyoteTimer = COYOTE_TIME_DURATION;
    }else if(gs->player.coyoteTimer > 0.0f){
        gs->player.coyoteTimer -= dt;
    }

    // jump buffer update
    if(jumpPressedNow(gs)){
        gs->player.jumpBufferTimer = JUMP_BUFFER_DURATION;
    }else if(gs->player.jumpBufferTimer > 0.0f){
        gs->player.jumpBufferTimer -= dt;
    }

    if(gs->player.jumpBufferTimer > 0.0f && (gs->player.isgrounded || gs->player.coyoteTimer > 0.0f)){
        PlaySound(gs->audio.jump);
        gs->player.velocity.y = -jumpSpeed;
        gs->player.isgrounded = false; 
        gs->player.coyoteTimer = 0.0f;
        gs->player.jumpBufferTimer = 0.0f;
    }
    gs->player.position = (Vector2) Vector2Add(gs->player.position,Vector2Scale(gs->player.velocity,dt));

}



void checkHealthPickup(GS* gs){
    if(gs->player.isDead) return;
    Rectangle playerRect = getPlayerRect(gs);

    for(int i = 0; i < MaxChunkNum; i++){
        if(gs->gchunk[i].hasHealthItem && !gs->gchunk[i].healthItemCollected){
            if(CheckCollisionRecs(playerRect, gs->gchunk[i].healthItemRect)){
                gs->player.health += 25.0f; 
                

                PlaySound(gs->audio.health_pickup);
                if(gs->player.health > gs->player.maxHealth){
                    gs->player.health = gs->player.maxHealth;
                }
                gs->gchunk[i].healthItemCollected = true; 
                spawn_health_update(gs,25);
            }

        }
    }
}
Rectangle getHeadCheckRec(GS* gs){
    Rectangle body = getPlayerRect(gs);
    return (Rectangle){
        .x = body.x + body.width*0.2f,
        .y = body.y - 4.0f,
        .width = body.width*0.6f,
        .height = 4.0f
    };
}

Rectangle getLeftCheckRec(GS* gs){
    Rectangle body = getPlayerRect(gs);
    return (Rectangle){
        .x = body.x - 8.0f,
        .y = body.y + 6.0f,           
        .width = 8.0f,
        .height = body.height - 12.0f
    };
}

Rectangle getRightCheckRec(GS* gs){
    Rectangle body = getPlayerRect(gs);
    return (Rectangle){
        .x = body.x + body.width,
        .y = body.y + 6.0f,
        .width = 8.0f,
        .height = body.height - 12.0f
    };
}
void checkCeilingCollision(GS* gs){
    if(gs->player.velocity.y >= 0) return;   
    Rectangle headRect = getHeadCheckRec(gs);
    for(int i=0;i<MaxChunkNum;i++){
        Rectangle chunk = gs->gchunk[i].groundChunkRect;
        if(chunk.width <= 0.0f) continue;
        if(CheckCollisionRecs(headRect, chunk)){
            gs->player.velocity.y = 0;
            gs->player.position.y = chunk.y + chunk.height - gs->player.collisionOffset.y;
            break;
        }
    }
}

void checkWallCollision(GS* gs){
    const float direction = gs->player.velocity.x;
    if(direction == 0.0f) return;

    bool collided = false;
    for(int pass = 0; pass < MaxChunkNum; pass++){
        Rectangle sideRect = direction < 0.0f ? getLeftCheckRec(gs) : getRightCheckRec(gs);
        bool pushed = false;
        float resolvedX = gs->player.position.x;
        for(int i = 0; i < MaxChunkNum; i++){
            Rectangle chunk = gs->gchunk[i].groundChunkRect;
            if(chunk.width <= 0.0f || gs->player.prevBottom <= chunk.y + 1.0f) continue;
            if(!CheckCollisionRecs(sideRect, chunk)) continue;

            pushed = true;
            float candidateX = direction < 0.0f
                ? chunk.x + chunk.width - gs->player.collisionOffset.x
                : chunk.x - gs->player.width - gs->player.collisionOffset.x;
            if(direction < 0.0f){
                if(candidateX > resolvedX) resolvedX = candidateX;
            }else if(candidateX < resolvedX){
                resolvedX = candidateX;
            }
        }
        if(!pushed) break;
        collided = true;
        if(fabsf(resolvedX - gs->player.position.x) < 0.01f) break;
        gs->player.position.x = resolvedX;
    }
    if(collided) gs->player.velocity.x = 0.0f;
}