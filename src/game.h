#ifndef GAME_H
#define GAME_H
#include"types.h"
#include"health.h"
void drawGame(GS *gs,tex *textures);
void initGame(GS* gs, tex* tex, anim* anim);
void updateGame(GS* gs,anim* anim, float dt);
void updateGameplay(GS* gs,anim* anim,float dt);
//menu
void updateMenu(GS* gs);
void drawMenu(GS* gs,tex* tex);

//difficulty select (after START, before name entry)
void updateDifficultySelect(GS* gs);
void drawDifficultySelect(GS* gs);

//game 
void updateNameEntry(GS* gs);
void drawNameEntry(GS* gs);
void updateGameover(GS* gs);
void player_has_fallen(GS* gs);
void isGameover(GS* gs,float dt);
void drawGameover(GS* gs);
void updatescore(GS* gs);
void drawScoreHUD(const GS* gs);
void updateCredits(GS* gs);
void drawCredits(GS* gs, tex* textures);
void updatePauseMenu(GS* gs);
void drawPauseMenu(GS* gs);
void updateWorldForResize(GS* gs);


void addbomb(GS* gs, float x, float y, float width, float height);
void drawBombs(GS* gs, tex* textures);




void restartGame(GS* gs);
void updateCheatCode(GS* gs);

#endif 