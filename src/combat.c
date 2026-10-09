#include"combat.h"
#include"enemy.h"
#include"player.h"
#include"health.h"
#include"explosion.h"
#include"camera.h"
#include"raymath.h"
#include<math.h>

void updateCombat(GS *gs, float dt){
    Player* p =  &gs->player;
    anim* a = &gs->player_animations[gs->current_player_anim_name];

    // ==---player er jono--===
    if(p->isattacking){
        p->hitduration-=dt;
        if(!p->hashitthiswing && attackstartframe<= a->currentframe && a->currentframe <= attackendframe){
            Rectangle player_hitbox = getplayerhitbox(gs);
            int target = -1;
            float nearest = INFINITY;
            for(int i=0;i<max_enemy_num;i++){
                Enemy* en = &gs->enemy[i];
                if(!en->isactive || en->isdead) continue;
                Rectangle enemyRect = getEnemyRect(en);
                if(!CheckCollisionRecs(player_hitbox, enemyRect)) continue;
                float distance = fabsf((enemyRect.x + enemyRect.width * 0.5f) -
                                       (player_hitbox.x + player_hitbox.width * 0.5f));
                if(distance < nearest){
                    nearest = distance;
                    target = i;
                }
            }
            if(target >= 0){
                damageEnemy(gs, &gs->enemy[target], player_attack_power);
                PlaySound(gs->audio.hit);
                p->hashitthiswing = true;
            }
        }
        if(p->hitduration<=0){
            p->isattacking = false;
            p->hashitthiswing = false; 
        }
    } 
    
    // --enemy er jonno--
    for(int i=0;i<max_enemy_num;i++){
        Enemy* en = &gs->enemy[i];
        anim* e = &en->enemy_animations[en->current_enemy_anim_name];
        if(en->isdead || !en->isactive) continue;
        if(en->state == attacking_enemy && en->current_enemy_anim_name == enemy_attack){
            if(enemy_attack_start_frame <= e->currentframe && e->currentframe <= enemy_attack_end_frame){
                Rectangle enemy_hitbox = getEnemyHitbox(en);
                if(!en->hashitplayerthisswing && CheckCollisionRecs(getPlayerRect(gs),enemy_hitbox)){
                    float mult = (en->dmgMult > 0.0f) ? en->dmgMult : 1.0f;
                    damagePlayer(gs, diffEnemyDmg(gs->difficulty) * mult);                   
                    en->hashitplayerthisswing=true;    
                }          
            }
        }
    }
    // pgas er jonno
    gs->pgas.attacktimer-=dt;
    if(gs->pgas.attacktimer<=0) gs->pgas.attacktimer=0;
    if(CheckCollisionRecs(getPlayerRect(gs),getPgasRect(gs)) && gs->pgas.attacktimer==0){
        damagePlayer(gs,gs->pgas.pgas_damage);
        gs->pgas.attacktimer = gs->pgas.attackcooldown;
    }
}


void DamageFromBombs(GS* gs, float dt) {
    Rectangle playerRect = getPlayerRect(gs);
    Vector2 playerCenter = {
        playerRect.x + playerRect.width/2.0f,
        playerRect.y + playerRect.height/2.0f
    };

    for (int i = 0; i < max_bombs; i++) {
        bomb* b = &gs->bombs[i];
        if (!b->isactive) continue;

        Vector2 bombCenter = {
            b->rect.x + b->rect.width/2.0f,
            b->rect.y + b->rect.height/2.0f
        };
        bool playerInRange = Vector2Distance(playerCenter, bombCenter) <= bomb_explosion_range;

        // Arm on proximity, but once armed the fuse keeps ticking even if the
        // player escapes. Damage is range-checked at detonation time only.
        if (playerInRange && !b->armed) {
            b->armed = true;
            b->fuseTimer = bomb_fuse_time;
        }

        if (b->armed) {
            b->fuseTimer -= dt;

            if (b->fuseTimer <= 0.0f) {
                b->isactive = false;
                b->armed = false;
                spawnExplosion(gs, bombCenter);
                PlaySound(gs->audio.explosion);
                // Re-check range at the blast moment so escaping negates damage.
                float distNow = Vector2Distance(playerCenter, bombCenter);
                if (distNow <= bomb_explosion_range) {
                    damagePlayer(gs, diffBombDmg(gs->difficulty));
                }
                // bomb blasts always shake (even on a clean dodge); the stronger
                // bomb shake wins over the hurt shake via triggerScreenShake().
                triggerScreenShake(gs, SHAKE_BOMB_DURATION, SHAKE_BOMB_MAGNITUDE);
                triggerHitStop(gs, HITSTOP_BOMB_DURATION);
            }
        }
    }
}