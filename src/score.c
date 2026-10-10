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
            break; 
        }
        line[strcspn(line, "\r\n")] = '\0';
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
            line[0] = '\0'; 
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
        highScores[i] = highScores[i - 1];   
        i--;
    }

    highScores[i].score = newScore;
    strncpy(highScores[i].name, name, MAX_NAME_LEN - 1);
    highScores[i].name[MAX_NAME_LEN - 1] = '\0';   

    saveHighScores(highScores);
    return 1;
}

void drawGameOverScores(const GS* gs, int isNewHighScore) {
    float centerX = s_width / 2.0f;
    float startY = (s_height < 650) ? 140.0f : 175.0f;

    float scoreFontSize = (s_height < 650) ? 40.0f : 50.0f;
    const char* scoreText = TextFormat("Score: %d", gs->score);
    Vector2 scoreSize = MeasureTextEx(gs->cfonts.menu_font3, scoreText, scoreFontSize, 0);
    DrawTextEx(gs->cfonts.menu_font3, scoreText, (Vector2){centerX - (scoreSize.x / 2.0f), startY}, scoreFontSize, 0, RAYWHITE);

    float currentY = startY + scoreFontSize + 10.0f;

    if (isNewHighScore) {
        float newHsFontSize = (s_height < 650) ? 30.0f : 36.0f;
        const char* newHsText = "New High Score!";
        Vector2 newHsSize = MeasureTextEx(gs->cfonts.menu_font3, newHsText, newHsFontSize, 0);
        DrawTextEx(gs->cfonts.menu_font3, newHsText, (Vector2){centerX - (newHsSize.x / 2.0f), currentY}, newHsFontSize, 0, YELLOW);
        currentY += newHsFontSize + 12.0f;
    }

    float titleFontSize = (s_height < 650) ? 40.0f : 50.0f;
    const char* titleText = "High Scores";
    Vector2 titleSize = MeasureTextEx(gs->cfonts.menu_font3, titleText, titleFontSize, 0);
    DrawTextEx(gs->cfonts.menu_font3, titleText, (Vector2){centerX - (titleSize.x / 2.0f), currentY}, titleFontSize, 0, GOLD);

    currentY += titleFontSize + 14.0f;
    float listFontSize = (s_height < 650) ? 28.0f : 36.0f;
    float listSpacing = (s_height < 650) ? 34.0f : 46.0f;

    for (int i = 0; i < MAX_HIGH_SCORES; i++) {
        const char* listText = TextFormat("%d. %s - %d", i + 1, gs->highScores[i].name, gs->highScores[i].score);
        Vector2 listSize = MeasureTextEx(gs->cfonts.menu_font3, listText, listFontSize, 0);
        DrawTextEx(gs->cfonts.menu_font3, listText, (Vector2){centerX - (listSize.x / 2.0f), currentY + (i * listSpacing)}, listFontSize, 0, LIGHTGRAY);
    }
}
void drawDifficultyMeter(GS* gs){
    float diff = getDifficultyFactor(gs);

    float barWidth = 250.0f;
    float barHeight = 25.0f;
    float posX = s_width - barWidth - 30.0f; 
    float posY = 30.0f;
    unsigned char r, g;
    if(diff < 0.5f){
        float t = diff / 0.5f;         
        r = (unsigned char)(255 * t);
        g = 255;
    } else {
        float t = (diff - 0.5f) / 0.5f; 
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
    const char* diffName = (gs->difficulty == DIFF_EASY) ? "EASY" : (gs->difficulty == DIFF_HARD) ? "HARD" : "MEDIUM";
    Color diffColor = (gs->difficulty == DIFF_EASY) ? LIME : (gs->difficulty == DIFF_HARD) ? RED : GOLD;
    Vector2 diffSize = MeasureTextEx(gs->cfonts.menu_font3, diffName, 40, 0);
    DrawTextEx(gs->cfonts.menu_font3, diffName, (Vector2){posX + barWidth/2.0f - diffSize.x/2.0f, posY + barHeight + 40.0f}, 30, 0, diffColor);
}