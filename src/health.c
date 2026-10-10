#include"health.h"
#include"player.h"
#include"enemy.h"
#include"camera.h"
#include<string.h>
#include<stdio.h>
#include<math.h>

void updateHealth(GS* gs, float  dt){
    (void)dt;
    if(gs->godmode && !gs->player.isDead) gs->player.health = gs->player.maxHealth;
    if(gs->player.isDead) return;
    if(gs->player.health <= 0.0f){
        gs->player.health = 0.0f;
        gs->player.isDead = true;
    }
}

void drawHealthUI(GS* gs){
    if(gs->player.isDead) return;

    float posX = 30.0f;
    float posY = 30.0f;


    const char* heroname = TextFormat("%s", gs->playerName); 
 
    DrawTextEx(gs->cfonts.menu_font3, heroname, (Vector2){posX + 3, posY + 3}, 60, 0, Fade(BLACK, 0.6f)); 

    DrawTextEx(gs->cfonts.menu_font3, heroname, (Vector2){posX, posY}, 60, 0, GOLD); 

    float barY = posY + 70.0f;
    float barWidth = 250.0f; 
    float barHeight = 25.0f;
    
    float healthPercentage = gs->player.health / gs->player.maxHealth;
    if(healthPercentage < 0) healthPercentage = 0.0f; 

    DrawRectangleRounded((Rectangle){posX, barY, barWidth, barHeight}, 0.5f, 10, Fade(BLACK, 0.7f));
    
    Color healthColor = LIME;
    if(healthPercentage <= 0.5f) healthColor = YELLOW;
    if(healthPercentage <= 0.25f) healthColor = RED;

    DrawRectangleRounded((Rectangle){posX, barY, barWidth * healthPercentage, barHeight}, 0.5f, 10, healthColor);

    DrawRectangleRoundedLines((Rectangle){posX, barY, barWidth, barHeight}, 0.5f, 10, LIGHTGRAY);

    const char* hpText = TextFormat("HP: %d / %d", (int)gs->player.health, (int)gs->player.maxHealth);

    DrawTextEx(gs->cfonts.menu_font3,hpText, (Vector2){posX + 5, barY + barHeight + 8}, 30, 0,RAYWHITE);
    if(gs->godmode){
        DrawTextEx(gs->cfonts.menu_font3, "GOD MODE", (Vector2){posX + 5, barY + barHeight + 44}, 30, 0, GOLD);
    }
}


void damagePlayer(GS* gs,float amount){
    Player* p = &gs->player;
    if(gs->godmode) return; 
    if(p->isDead || p->invultimer>0) return;
    p->health-=amount;
    spawn_health_update(gs,-amount);
    p->invultimer = player_invul_time;
    triggerScreenShake(gs, SHAKE_HURT_DURATION, SHAKE_HURT_MAGNITUDE);
    triggerHitStop(gs, HITSTOP_HURT_DURATION);

    if(p->health<=0.0f){
        p->isDead = true;
        p->health=0.0f;
        PlaySound(gs->audio.die);
    }
    else {
        gs->current_player_anim_name = player_hurt;
        anim* hurtAnim = &gs->player_animations[player_hurt];
        hurtAnim->currentframe = 0;
        hurtAnim->frametimer = 0.0f;
        hurtAnim->isfinished = false;
        p->velocity.x = 0;
         PlaySound(gs->audio.hurt);
    }
}


void updatePlayerInvulnerability(GS* gs,float dt){
    Player* p = &gs->player;
    if (p->invultimer >= 0.0f) {
        p->invultimer -= dt; 
        if (p->invultimer < 0.0f) p->invultimer = 0.0f;
    }
}

void spawn_health_update(GS* gs,int amount){
    Rectangle playerRect = getPlayerRect(gs);
    for(int t=0;t<10;t++){
        if(!gs->floatTexts[t].active){
            gs->floatTexts[t].active = true;
            
            gs->floatTexts[t].position =(Vector2){playerRect.x,playerRect.y-30.0f};
            gs->floatTexts[t].timer=1.5f;
            gs->floatTexts[t].maxTime=1.5f;
            char show[40];
            sprintf(show,"%+d %s",amount, "HP");
            strcpy(gs->floatTexts[t].text, show);
            gs->floatTexts[t].color =(amount<0)?RED:GOLD;
            break;  
            
        }
    }
}

void spawnHealthDrop(GS* gs, float x, float y){
    int index = gs->healthDrop_index;

    gs->healthDrops[index].rect = (Rectangle){
        .x = x - health_drop_width/2.0f,
        .y = y - health_drop_height/2.0f,
        .width = health_drop_width,
        .height = health_drop_height
    };
    gs->healthDrops[index].healAmount = enemy_health_drop_amount;
    gs->healthDrops[index].active = true;

    gs->healthDrop_index = (gs->healthDrop_index + 1) % max_health_drops;
}

void updateHealthDropPickup(GS* gs){
    if(gs->player.isDead) return;
    Rectangle playerRect = getPlayerRect(gs);

    for(int i=0;i<max_health_drops;i++){
        HealthDrop* d = &gs->healthDrops[i];
        if(!d->active) continue;

        if(CheckCollisionRecs(playerRect, d->rect)){
            gs->player.health += d->healAmount;
            if(gs->player.health > gs->player.maxHealth) gs->player.health = gs->player.maxHealth;

            PlaySound(gs->audio.health_pickup);
            spawn_health_update(gs, (int)d->healAmount);
            d->active = false;
        }
    }
}

void drawHealthDrops(GS* gs, tex* textures){
    for(int i=0;i<max_health_drops;i++){
        HealthDrop* d = &gs->healthDrops[i];
        if(!d->active) continue;

        Texture2D dropTex = textures->enemy_health_drop.id != 0 ? textures->enemy_health_drop : textures->health_item;
        Rectangle source = {0, 0, (float)dropTex.width, (float)dropTex.height};
        float floatOffset = sinf((float)GetTime() * 4.0f) * 6.0f; 

        Rectangle dest = {
            .x = d->rect.x,
            .y = d->rect.y + floatOffset,
            .width = d->rect.width,
            .height = d->rect.height
        };
        DrawTexturePro(dropTex, source, dest, (Vector2){0,0}, 0.0f, WHITE);
    }
}