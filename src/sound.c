#include "sound.h"
#include "player.h"
#include <math.h>

#define footstep_interval 0.35f  

void load_audio(GS* gs){
    gs->audio.menuMusic = LoadMusicStream("assets/music/menu_background_music.mp3");
    gs->audio.menuMusic.looping = true;
    gs->audio.gameMusic = LoadMusicStream("assets/music/game_music1.wav");
    gs->audio.gameMusic.looping = true;
    SetMusicVolume(gs->audio.gameMusic, .3f);
    SetMusicPitch(gs->audio.gameMusic, 1.0f);

    gs->audio.hurt = LoadSound("assets/music/hurt.mp3");
    gs->audio.die = LoadSound("assets/music/die.mp3");
    SetSoundPitch(gs->audio.die, 1.2f);
    gs->audio.enemyDie = LoadSound("assets/music/enemy_die.mp3");
    gs->audio.hit = LoadSound("assets/music/enemy_hurt.mp3");
    gs->audio.gameOverSting = LoadSound("assets/music/game_over_sound.mp3");
    gs->audio.player_run = LoadSound("assets/music/running.mp3");
    SetSoundVolume(gs->audio.player_run, .5f);

    gs->audio.swing = LoadSound("assets/music/player_swing.mp3");
    gs->audio.enemy_swing = LoadSound("assets/music/enemy_swing.mp3");
    gs->audio.dash = LoadSound("assets/music/dash.mp3");
    gs->audio.jump = LoadSound("assets/music/jump.mp3");
    SetSoundPitch(gs->audio.jump, 1.2f);
    gs->audio.landing = LoadSound("assets/music/landing.mp3");
    gs->audio.enemy_run = LoadSound("assets/music/enemy_run.mp3");
    gs->audio.health_pickup = LoadSound("assets/music/health_pickup.mp3");
    gs->audio.explosion = LoadSound("assets/music/explosion.mp3");
    gs->audio.menu_click = LoadSound("assets/music/menu_click.mp3");
    gs->audio.menu_select = LoadSound("assets/music/menu_select.mp3");
    gs->audio.typing = LoadSound("assets/music/typing.mp3");


    gs->audio.footstepTimer = footstep_interval;   
    for (int i = 0; i < max_enemy_num; i++) {
        gs->audio.enemyFootstepTimer[i] = 0.0f;
    }
}

void updateMusic(GS* gs){
    // Only update streams that are actually playing to avoid any weird overlap
    if (IsMusicStreamPlaying(gs->audio.menuMusic)) UpdateMusicStream(gs->audio.menuMusic);
    if (IsMusicStreamPlaying(gs->audio.gameMusic)) UpdateMusicStream(gs->audio.gameMusic);
}

void unloadAudio(GS* gs){
    UnloadMusicStream(gs->audio.menuMusic);
    UnloadMusicStream(gs->audio.gameMusic);

    UnloadSound(gs->audio.hurt);
    UnloadSound(gs->audio.die);
    UnloadSound(gs->audio.enemyDie);
    UnloadSound(gs->audio.hit);
    UnloadSound(gs->audio.gameOverSting);
    UnloadSound(gs->audio.player_run);
    UnloadSound(gs->audio.enemy_run);
    UnloadSound(gs->audio.swing);
    UnloadSound(gs->audio.enemy_swing);
    UnloadSound(gs->audio.dash);
    UnloadSound(gs->audio.jump);
    UnloadSound(gs->audio.landing);
    UnloadSound(gs->audio.health_pickup);
    UnloadSound(gs->audio.explosion);
    UnloadSound(gs->audio.menu_click);
    UnloadSound(gs->audio.menu_select);
    UnloadSound(gs->audio.typing);
}


void playerFootstepUpdate(GS* gs, float dt){
    Player* p = &gs->player;
    bool isRunning = p->isgrounded && !p->isDead && !p->isdashing
                     && fabsf(p->velocity.x) > 10.0f;

    if (!isRunning) {
        gs->audio.footstepTimer = footstep_interval; 
        return;
    }

    gs->audio.footstepTimer += dt;
    if (gs->audio.footstepTimer >= footstep_interval) {
        gs->audio.footstepTimer = 0.0f;   
        PlaySound(gs->audio.player_run);
    }
}


void enemyFootstepUpdate(GS* gs, float dt){
    float playerCenter = getPlayerCenterX(gs);
    for (int i = 0; i < max_enemy_num; i++) {
        Enemy* e = &gs->enemy[i];
        if (!e->isactive || e->state != walking_enemy) {
            gs->audio.enemyFootstepTimer[i] = 0.0f;
            continue;
        }
        if (fabsf((e->position.x + e->width * 0.5f) - playerCenter) > s_width * 0.75f) {
            continue;
        }
        gs->audio.enemyFootstepTimer[i] += dt;
        if (gs->audio.enemyFootstepTimer[i] >= footstep_interval) {
            gs->audio.enemyFootstepTimer[i] = 0.0f;   
            PlaySound(gs->audio.enemy_run);
        }
    }
}