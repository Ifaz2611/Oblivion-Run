#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"

Rectangle getPlayerRect(GS* gs);
void playerMovement(GS* gs,anim* anim,float dt);
void Gravity(GS* gs,float dt);
void drawPlayerSprite(GS* gs);
void setplayerstate(GS* gs);
void updateJumpFrame(GS* gs);
void playerDashUpdate(GS* gs,float dt);
Rectangle getplayerhitbox(GS* gs);
void hitting(GS* gs,float dt);
void restrict_left_movement(GS* gs);
void checkHealthPickup(GS* gs);
float getPlayerCenterX(GS* gs);
Rectangle getHeadCheckRec(GS* gs);
Rectangle getLeftCheckRec(GS* gs);
Rectangle getRightCheckRec(GS* gs);
void checkCeilingCollision(GS* gs);
void checkWallCollision(GS* gs);
// combined keyboard + on-screen button input
void updateTouchButtons(GS* gs);
Rectangle getTouchBtnRect(touchbtn b);
bool isMouseOnAnyTouchBtn(void);
void drawTouchControls(GS* gs);
#endif