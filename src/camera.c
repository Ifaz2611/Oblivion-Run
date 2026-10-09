#include"camera.h"

void cameraMovement(GS* gs){

    if(gs->starting_timer>0){
        gs->camera.target.x = 0.0f;
        gs->camera.target.y = 0.0f;
    }
    // gs->camera.target = (Vector2){gs->player.position.x,0.0f};
    float player_center_x = gs->player.position.x  + gs->player.collisionOffset.x + gs->player.width / 2.0f;
    float camera_right_pos = gs->camera.target.x + camera_half_deadzone;

    if(player_center_x>camera_right_pos){
        gs->camera.target = (Vector2){player_center_x-camera_half_deadzone,0.0f};
    }
}

void triggerScreenShake(GS* gs, float duration, float magnitude){
    if(duration <= 0.0f || magnitude <= 0.0f) return;
    if(gs->shakeTime <= 0.0f || magnitude >= gs->shakeMagnitude){
        gs->shakeTime = duration;
        gs->shakeDuration = duration;
        gs->shakeMagnitude = magnitude;
    }else if(duration > gs->shakeTime){
        gs->shakeTime = duration;
        gs->shakeDuration = duration > gs->shakeDuration ? duration : gs->shakeDuration;
    }
}

void triggerHitStop(GS* gs, float duration){
    if(duration <= 0.0f) return;
    if(duration > gs->hitStopTimer) gs->hitStopTimer = duration;
}

void updateScreenShake(GS* gs, float dt){
    if(gs->shakeTime > 0.0f){
        gs->shakeTime -= dt;
        if(gs->shakeTime <= 0.0f){
            gs->shakeTime = 0.0f;
            gs->shakeOffset = (Vector2){0.0f, 0.0f};
            return;
        }
        float frac = (gs->shakeDuration > 0.0f) ? (gs->shakeTime / gs->shakeDuration) : 0.0f;
        float mag = gs->shakeMagnitude * frac;
        float ox = ((float)GetRandomValue(-1000, 1000) / 1000.0f) * mag;
        float oy = ((float)GetRandomValue(-1000, 1000) / 1000.0f) * mag;
        gs->shakeOffset = (Vector2){ox, oy};
    }else{
        gs->shakeOffset = (Vector2){0.0f, 0.0f};
    }
}
