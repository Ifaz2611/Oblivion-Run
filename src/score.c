#include <stdio.h>
#include <string.h>
#include "score.h"
#include "constants.h"
#include"ground.h"

void loadHighScores(HighScoreEntry highScores[MAX_HIGH_SCORES]) {
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        highScores[i].name[0] = '\0';
        highScores[i].score = 0;
    }
    FILE* f = fopen(HIGHSCORE_FILE, "r");
    if (!f) {
        return;
    }
    char line[128];
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        if (!fgets(line, sizeof(line), f)) {
            break; // remaining slots keep the zero defaults above
        }
        line[strcspn(line, "\r\n")] = '\0';
        // Split on the LAST space so hero names may contain spaces.
        char* lastSpace = strrchr(line, ' ');
        if (!lastSpace || lastSpace == line) {
            continue;
        }
        *lastSpace = '\0';
        int sc = 0;
        if (sscanf(lastSpace + 1, "%d", &sc) != 1) {
            continue;
        }
        if (strcmp(line, "---") == 0) {
            line[0] = '\0'; // placeholder written for empty slots
        }
        strncpy(highScores[i].name, line, MAX_NAME_LEN - 1);
        highScores[i].name[MAX_NAME_LEN - 1] = '\0';
        highScores[i].score = sc;
    }
    fclose(f);
}

void saveHighScores(const HighScoreEntry highScores[MAX_HIGH_SCORES]) {
    FILE* f = fopen(HIGHSCORE_FILE, "w");
    if (!f) return;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        const char* name = highScores[i].name[0] ? highScores[i].name : "---";
        fprintf(f, "%s %d\n", name, highScores[i].score);
    }
    fclose(f);
}

int tryAddHighScore(HighScoreEntry highScores[MAX_HIGH_SCORES], const char* name, int newScore) {
    if (newScore <= highScores[MAX_HIGH_SCORES - 1].score) return 0;

    int i = MAX_HIGH_SCORES - 1;
    while (i > 0 && highScores[i - 1].score < newScore) {
        highScores[i] = highScores[i - 1];   // struct assignment copies name + score together
        i--;
    }

    highScores[i].score = newScore;
    strncpy(highScores[i].name, name, MAX_NAME_LEN - 1);
    highScores[i].name[MAX_NAME_LEN - 1] = '\0';   // strncpy doesn't guarantee this on truncation

    saveHighScores(highScores);
    return 1;
}

void drawGameOverScores(const GS* gs, int isNewHighScore) {
    float centerX = s_width / 2.0f;
    float centerY = s_height / 2.0f;

    
    float scoreFontSize = 50.0f;
    const char* scoreText = TextFormat("Score: %d", gs->score);
    Vector2 scoreSize = MeasureTextEx(gs->cfonts.menu_font3, scoreText, scoreFontSize, 0);
    DrawTextEx(gs->cfonts.menu_font3, scoreText, (Vector2){centerX - (scoreSize.x / 2.0f), centerY - 350}, scoreFontSize, 0, RAYWHITE);

    
    if (isNewHighScore) {
        float newHsFontSize = 40.0f;
        const char* newHsText = "New High Score!";
        Vector2 newHsSize = MeasureTextEx(gs->cfonts.menu_font3, newHsText, newHsFontSize, 0);
        DrawTextEx(gs->cfonts.menu_font3, newHsText, (Vector2){centerX - (newHsSize.x / 2.0f), centerY - 120}, newHsFontSize, 0, YELLOW);
    }

    
    float titleFontSize = 60.0f;
    const char* titleText = "High Scores";
    Vector2 titleSize = MeasureTextEx(gs->cfonts.menu_font3, titleText, titleFontSize, 0);
    DrawTextEx(gs->cfonts.menu_font3, titleText, (Vector2){centerX - (titleSize.x / 2.0f), centerY - 40}, titleFontSize, 0, GOLD);

    
    float listFontSize = 45.0f;
    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        const char* listText = TextFormat("%d. %s - %d", i + 1, gs->highScores[i].name, gs->highScores[i].score);
        Vector2 listSize = MeasureTextEx(gs->cfonts.menu_font3, listText, listFontSize, 0);
        DrawTextEx(gs->cfonts.menu_font3, listText, (Vector2){centerX - (listSize.x / 2.0f), centerY + 40 + (i * 60)}, listFontSize, 0, LIGHTGRAY);
    }
}
void drawDifficultyMeter(GS* gs){
    float diff = getDifficultyFactor(gs);

    float barWidth = 250.0f;
    float barHeight = 25.0f;
    float posX = s_width - barWidth - 30.0f;   // top-right corner, mirrors the health bar's top-left placement
    float posY = 30.0f;

    // green -> yellow -> red as diff goes 0 -> 1, same two-stop blend feel as the health bar's stepped colors
    // but smooth here since difficulty is continuous rather than stepped
    unsigned char r, g;
    if(diff < 0.5f){
        float t = diff / 0.5f;          // 0..1 across the green->yellow half
        r = (unsigned char)(255 * t);
        g = 255;
    } else {
        float t = (diff - 0.5f) / 0.5f; // 0..1 across the yellow->red half
        r = 255;
        g = (unsigned char)(255 * (1.0f - t));
    }
    Color meterColor = (Color){r, g, 0, 255};

    DrawRectangleRounded((Rectangle){posX, posY, barWidth, barHeight}, 0.5f, 10, Fade(BLACK, 0.7f));
    DrawRectangleRounded((Rectangle){posX, posY, barWidth * diff, barHeight}, 0.5f, 10, meterColor);
    DrawRectangleRoundedLines((Rectangle){posX, posY, barWidth, barHeight}, 0.5f, 10, LIGHTGRAY);

    const char* label = "DIFFICULTY";
    Vector2 labelSize = MeasureTextEx(gs->cfonts.menu_font3, label, 40, 0);
    DrawTextEx(gs->cfonts.menu_font3, label, (Vector2){posX + barWidth/2.0f - labelSize.x/2.0f, posY + barHeight + 5.0f}, 30, 0, RAYWHITE);
}