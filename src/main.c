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
    InitWindow(1280, 720, "OBLIVION RUN");
    SetTargetFPS(60);
    {
        int monitor = GetCurrentMonitor();
        int mw = GetMonitorWidth(monitor);
        int mh = GetMonitorHeight(monitor);
        int ww = mw - 80;
        if (ww > 1920) ww = 1920;
        int wh = (ww * 9) / 16;
        if (wh > mh - 120) {
            wh = mh - 120;
            ww = (wh * 16) / 9;
        }
        if (ww < 960) ww = 960;
        if (wh < 540) wh = 540;
        SetWindowMinSize(960, 540);
        SetWindowSize(ww, wh);
        SetWindowPosition(mw / 2 - ww / 2, mh / 2 - wh / 2);
    }
    InitAudioDevice();
    GS gs={0};
    tex tex={0};
    anim anim = {0};
    initGame(&gs,&tex,&anim);
    while(!WindowShouldClose() && !gs.quit_game){
        float dt = GetFrameTime();
        if(dt > 1.0f/30.0f) dt = 1.0f/30.0f;
        updateGame(&gs,&anim,dt);
        updateMusic(&gs);
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gs.currentscreen == MENU) {
            drawMenu(&gs,&tex); 
        } 
        else if (gs.currentscreen == DIFFICULTY) {
            drawDifficultySelect(&gs);
        }
        else if (gs.currentscreen == NAME_ENTRY) { 
            drawNameEntry(&gs);
        }
        else if(gs.currentscreen==TUTORIAL){
            drawTutorial(&gs,&tex);
        }
        else if (gs.currentscreen == GAME || gs.currentscreen == PAUSED) {
            Camera2D renderCam = gs.camera;
            renderCam.offset.x += gs.shakeOffset.x;
            renderCam.offset.y += gs.shakeOffset.y;
            BeginMode2D(renderCam); 
            drawGame(&gs, &tex);
            EndMode2D();
            drawHealthUI(&gs);
            drawScoreHUD(&gs);
            drawDifficultyMeter(&gs);
            if(gs.currentscreen == GAME) drawTouchControls(&gs);
            else drawPauseMenu(&gs);
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
    unloadAudio(&gs);
    CloseAudioDevice();
    CloseWindow();
}


// Main Run code by ifaz 