#include"raylib.h"
#include<stdbool.h>
#include<math.h>
#include"types.h"
#include"game.h"
#include"enemy.h"
#include"texture.h"
#include"animation.h"
#include "health.h"
#include "sound.h"
#include"score.h"
#include"tutorial.h"
#include"player.h"
int main(){
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1920,1080,"OBLIVION RUN");
    SetTargetFPS(60);
    // On 1080p (or smaller) screens a decorated 1920x1080 window is taller
    // than the desktop, so switch to borderless to fill the display exactly.
    // On larger monitors keep a centered 1920x1080 window.
    {
        int monitor = GetCurrentMonitor();
        int mw = GetMonitorWidth(monitor);
        int mh = GetMonitorHeight(monitor);
        if (mw <= 1920 && mh <= 1080) {
            if (!IsWindowState(FLAG_BORDERLESS_WINDOWED_MODE)) ToggleBorderlessWindowed();
        } else {
            SetWindowPosition(mw/2 - GetScreenWidth()/2, mh/2 - GetScreenHeight()/2);
        }
    }
    InitAudioDevice();
    GS gs={0};
    tex tex={0};
    anim anim = {0};
    initGame(&gs,&tex,&anim);
    while(!WindowShouldClose() && !gs.quit_game){
        float dt = GetFrameTime();
        updateGame(&gs,&anim,dt);
        updateMusic(&gs);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gs.currentscreen == MENU) {
            drawMenu(&gs,&tex); 
        } 
        else if (gs.currentscreen == NAME_ENTRY) { 
            drawNameEntry(&gs);
        }
        else if(gs.currentscreen==TUTORIAL){
            drawTutorial(&gs,&tex);
        }
        else if (gs.currentscreen == GAME) {
            BeginMode2D(gs.camera); 
            drawGame(&gs, &tex);
            EndMode2D();
            drawHealthUI(&gs);
            drawScoreHUD(&gs);
            drawDifficultyMeter(&gs);
            drawTouchControls(&gs);
            }
        else if (gs.currentscreen == CREDITS) {
             drawCredits(&gs,&tex);
        }

        else if(gs.currentscreen == GAMEOVER){
            drawGameover(&gs);
        }

        EndDrawing();       
    }
    unloadTexture(&tex, &gs);
    unloadenemy(&gs);
    unloadAudio(&gs);
    CloseAudioDevice();
    CloseWindow();
}