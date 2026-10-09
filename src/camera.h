#ifndef CAMERA_H
#define CAMERA_H
#include"types.h"

void cameraMovement(GS* gs);
void triggerScreenShake(GS* gs, float duration, float magnitude);
void triggerHitStop(GS* gs, float duration);
void updateScreenShake(GS* gs, float dt);
#endif