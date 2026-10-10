#ifndef CONSTANTS_H
#define CONSTANTS_H

#include"raylib.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

//bg layers
#define BG_LAYER_COUNT 6
//screen er size newa
#define s_height GetScreenHeight()
#define s_width GetScreenWidth()

#define camera_half_deadzone 450.0f // player ke koto tuku cholar jaiga dewa hobe camerar moddhe

//different player speeds
#define pSpeed 1000.0f
#define pAttackMoveSpeed 700.0f
#define pSpeedAir 750.0f
#define jumpSpeed 700.0f
#define gravity 1400.0f
#define dash_speed 2200.0f // jore laaf dewar speed
//different timers
#define dash_duration .45f
#define dash_cooldowntimer .6f
#define attackduration .54f
#define airattackduration .56f
// texture choto boro korar jonno
#define SPRITE_SCALE 3.0f
#define BG_SCALE (SPRITE_SCALE * 1.3f)
// attack koto tuku gulo frame er moddhe hobe 
#define attackstartframe 3
#define attackendframe 6    
//koto gulo ground chunk dekhabe
#define MaxChunkNum 50
#define player_attack_power 30.0f

// enemy er jono
#define enSpeed 900.0f
#define encooldown 0.96f
#define attackrange 120.0f
#define enemy_attack_power 5.0f
#define max_enemy_num 100
#define enemy_max_health 60.0f
#define enemy_invultimer .32f
#define enemy_attack_start_frame 6
#define enemy_attack_end_frame 10
#define spikecooldown .60f
#define spike_damage 25.0f

// health maximum
#define PLAYER_MAX_HEALTH 100.0f
#define player_invul_time .04f
#define player_real_width 17.0f
#define player_real_height 32.0f


// obstacles and traps
#define pattern_tile_width 182.0f
#define platform_width 368.0f
#define patternheight 40.0f
#define gap_height 80.0f
#define max_spikes 30
#define ground_y  (GetScreenHeight()*3.7f/4)
#define spike_width 92.0f
// for score
#define MAX_HIGH_SCORES   5
#define HIGHSCORE_FILE    "data/highscore.txt"
#define SCORE_PER_DISTANCE 0.0025f
#define SCORE_PER_KILL_NORMAL 100
#define SCORE_PER_KILL_BRUTE  250
#define MAX_NAME_LEN 25

//bomb
#define max_bombs 20
#define bomb_damage 30.0f
#define bomb_width 80.0f  
#define bomb_height 80.0f

#define STARTING_TIMER 1.00f

#define enemy_aggro_range    800.0f   // koto dur theke enemy dhorte ashbe
#define enemy_despawn_margin 300.0f   // koto dur gele enemy despawn hoye jabe

// fog visuals (not gameplay hitbox — see getPgasRect/move_pgas)
#define fog_color_r        40
#define fog_color_g        90
#define fog_color_b        40
#define fog_base_alpha     160
#define fog_top_gap        80.0f   // empty space kept clear at the top of the screen
#define fog_bottom_gap     100.0f   // empty space kept clear above the very bottom
#define fog_vfade          80.0f    // how many px the top/bottom edges fade over
#define fog_edge_fade_width 200.0f  // how many px the leading (right) edge fades over
#define fog_edge_strips    12       // smoothness of the leading-edge fade
#define pgas_max_lag      3000.0f   // gap distance that triggers a snap-forward
#define pgas_teleport_lag  900.0f   // distance left of the player the gas snaps to

#define pgas_player_margin 250.0f   // how far past the player's right edge the fog should reach

#define bomb_explosion_range     250.0f   // damage radius in px, independent of the bomb's own rect
#define bomb_explosion_scale     4.0f     // how big the explosion sprite is drawn (tune to your sheet)
#define max_explosions           20
#define explosion_framecount     8        // SET THIS to your sprite sheet's actual frame count
#define explosion_frameduration  0.05f
#define bomb_fuse_time  .6f   // seconds from entering the blast radius to detonation

#define enemy_healthbar_width   60.0f
#define enemy_healthbar_height  8.0f
#define enemy_healthbar_yoffset 14.0f   // gap between the bar and the top of the sprite

#define max_health_drops       20
#define enemy_health_drop_amount 15.0f   // less than the normal pickup's 25
#define health_drop_width      50.0f
#define health_drop_height     50.0f

#define difficulty_ramp_distance 80000.0f

#define tutorial_char_interval 0.03f   // seconds per revealed character
#define tutorial_fade_speed    1.5f    // alpha change per second

#define bush_sprite_count   6  
#define detail_sprite_count 9   

#define max_bush_decor       400
#define bush_spacing         55.0f    // px between bush pieces — TUNE to your sprite's actual on-screen width so they touch with no gaps
#define bush_min_scale       2.5f
#define bush_max_scale       3.5f

#define max_detail_decor        200
#define detail_cluster_chance   55       // percent chance per plain tile to start a cluster
#define detail_cluster_min      2
#define detail_cluster_max      5
#define detail_cluster_spacing  50.0f
#define detail_min_scale        1.8f
#define detail_max_scale        3.0f
#define skip_timer 2.0f

// screen shake + hit-stop (bomb explosions and player damage)
#define SHAKE_BOMB_DURATION  0.35f
#define SHAKE_BOMB_MAGNITUDE 14.0f
#define HITSTOP_BOMB_DURATION 0.09f
#define SHAKE_HURT_DURATION  0.25f
#define SHAKE_HURT_MAGNITUDE 8.0f
#define HITSTOP_HURT_DURATION 0.06f

// gamepad (controller) tuning
#define GAMEPAD_DEADZONE 0.3f

// advanced platformer feel & combo mechanics
#define COYOTE_TIME_DURATION      0.12f
#define JUMP_BUFFER_DURATION      0.14f
#define COMBO_TIMEOUT_DURATION    2.5f
#define MAX_DASH_GHOSTS           8
#define DASH_GHOST_SPAWN_INTERVAL 0.05f
#define DASH_GHOST_FADE_SPEED     3.0f

// second enemy type (no new structs — parametrized via spawnEnemy type id)
#define ENEMY_TYPE_NORMAL 0
#define ENEMY_TYPE_BRUTE  1
#define ENEMY_TYPE_AUTO   -1   // pick by difficulty/distance
#define ENEMY_BRUTE_HP_MULT    2.2f
#define ENEMY_BRUTE_SPEED_MULT 0.85f
#define ENEMY_BRUTE_DMG_MULT   2.0f
#define ENEMY_BRUTE_SCALE_MULT 1.2f

// player-chosen difficulty (GS.difficulty): picked on the DIFFICULTY screen
// after START, before name entry. MEDIUM == legacy vanilla tuning.
#define DIFF_EASY   0
#define DIFF_MEDIUM 1
#define DIFF_HARD   2

static inline float diffEnemyHpMult(int d){
    if(d == DIFF_EASY) return 0.7f;
    if(d == DIFF_HARD) return 1.6f;
    return 1.0f;
}
static inline float diffEnemySpeedMult(int d){
    if(d == DIFF_EASY) return 0.8f;
    if(d == DIFF_HARD) return 1.25f;
    return 1.0f;
}
static inline float diffEnemyDmg(int d){
    if(d == DIFF_EASY) return 3.0f;
    if(d == DIFF_HARD) return 8.0f;
    return (float)enemy_attack_power;
}
static inline float diffSpikeDmg(int d){
    if(d == DIFF_EASY) return 15.0f;
    if(d == DIFF_HARD) return 35.0f;
    return (float)spike_damage;
}
static inline float diffBombDmg(int d){
    if(d == DIFF_EASY) return 20.0f;
    if(d == DIFF_HARD) return 45.0f;
    return (float)bomb_damage;
}
static inline float diffPgasDmg(int d){
    if(d == DIFF_EASY) return 7.0f;
    if(d == DIFF_HARD) return 15.0f;
    return 10.0f;
}
static inline float diffPgasSpeedMult(int d){
    if(d == DIFF_EASY) return 0.8f;
    if(d == DIFF_HARD) return 1.3f;
    return 1.0f;
}
static inline float diffPlayerMaxHp(int d){
    if(d == DIFF_EASY) return 120.0f;
    if(d == DIFF_HARD) return 80.0f;
    return (float)PLAYER_MAX_HEALTH;
}

#endif  