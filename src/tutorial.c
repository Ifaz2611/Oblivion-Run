#include "tutorial.h"
#include "background.h"
#include <string.h>
#include <stdio.h>

/* ================================================================== */
/*  Tutorial pages                                                      */
/* ================================================================== */

static const char *tutorialPages[] = {
    "                    THE STORY               \n\n"
    "---------------------------------------------------\n\n"
    "     You wandered too deep into the jungle.    \n\n"
    "         You ANGERED the FOREST!!!!\n\n"
    "       And now it's TAKING REVENGE ON YOU.\n\n"
    "A POISONOUS GAS CLOUD chases you until DEATH!!\n\n"
    " HOW LONG CAN YOU SURVIVE THIS OBLIVION RUN?!\n\n\n",


    "          READ THIS VERY CAREFULLY!!!!\n\n"
    "--------------------------------------------\n\n"
    "                   HOW TO PLAY\n\n"
    "                  -------------  \n\n"
    "A / D or ARROW KEYS  -  Move\n\n"
    "SPACE or UP ARROW  -  Jump\n\n"
    "LEFT / RIGHT SHIFT  -  Dash\n\n"
    "LEFT CLICK or DOWN ARROW / X  -  Attack\n\n"
    "Gamepad: Left stick / D-pad move, A jump,\n\n"
    "RB or B dash, X or RT attack, Start pause\n\n"
    "On-screen buttons (< > ^ DSH ATK) work too!\n\n",

    "                     OBSTACLES\n\n"
    "                    -----------\n\n"
    "SPIKES - Touching it causes damage\n\n"
    "BOMBS - STAYING IN RANGE CAUSES EXPLOSION\n\n",

    "                      ENEMY      \n\n"
    "                    --------\n\n\n\n"
    "ENEMY CHASES YOU AND HITS IF YOU ARE IN RANGE\n\n",

    "                   HEALTH     \n\n"
    "                  --------\n\n\n\n"
    "       collect Health throughout the GAME\n\n\n"
    "       healths randomly spawn on platforms\n\n\n\n\n\n\n\n",

    "                SPOILS OF WAR\n\n"
    "                --------------\n\n\n\n"
    "      Enemies also drop health when killed\n\n\n\n\n",


   "                    POISON GAS\n\n"
   "                   -------------\n\n\n"
   "AVOID THE POISON GAS CHASING YOU\n\n\n"
   "Getting close or staying in that will cause DAMAGE\n\n",

   "                ****TIPS******\n\n"
   "                --------------\n\n\n\n"
   "         USE DASH (LEFT SHIFT BUTTON)\n\n"
   "     WHEN THE GAP IS TOO BIG TO JUMP OVER\n\n",

   "                ****TIPS******\n\n"
   "                --------------\n\n\n\n"
   "       STAY CALM, STAY FAST, STAY ALIVE\n\n"
   "      ONE WRONG JUMP ENDS THE RUN — FOCUS",

   "            USE THE TIPS CORRECTLY\n\n\n\n\n\n\n\n\n"
   "                 !!!!! ENJOY !!!!!\n\n\n\n\n\n"
};
static const int tutorialPageCount = (int)(sizeof tutorialPages / sizeof *tutorialPages);

/* ================================================================== */
/*  Tuning constants                                                    */
/* ================================================================== */

static const float kTextTop      = 150.0f;   /* first line Y            */
static const float kLineHeight   = 34.0f;
static const float kFontSize     = 50.0f;
static const float kPromptSize   = 40.0f;
static const float kPromptMargin = 100.0f;   /* prompt distance from bottom edge */

enum { kVisibleCap = 2048, kLineCap = 256 };

/* ================================================================== */
/*  Page icons                                                          */
/*                                                                      */
/*  One table entry per icon instead of an if-chain per page.           */
/*  dx   : horizontal offset from screen centre                         */
/*  yFrac: vertical position as a fraction of screen height             */
/*  Icons are drawn centred on (cx + dx, s_height * yFrac).             */
/* ================================================================== */

typedef enum {
    ICON_SPIKES = 0,
    ICON_BOMB,
    ICON_ENEMY,
    ICON_HEALTH_PLATFORM,
    ICON_HEALTH_ENEMY,
    ICON_POISON_GAS,
    ICON_LARGE_GAP
} IconId;

typedef struct {
    int     page;
    IconId  icon;
    float   dx;      /* horizontal offset from screen centre */
    float   scale;   /* on-screen size multiplier            */
} PageIcon;

static const PageIcon kPageIcons[] = {
    { 2, ICON_SPIKES,          -240.0f,  6.0f               },
    { 2, ICON_BOMB,             240.0f,  0.8f               },
    { 3, ICON_ENEMY,              0.0f,  SPRITE_SCALE * 2.5f },
    { 4, ICON_HEALTH_PLATFORM,    0.0f,  0.625f             },
    { 5, ICON_HEALTH_ENEMY,       0.0f,  1.0f               },
    { 6, ICON_POISON_GAS,         0.0f,  0.5f               },
    { 7, ICON_LARGE_GAP,          0.0f,  0.5f               },
};
static const int kPageIconCount = (int)(sizeof kPageIcons / sizeof *kPageIcons);

/* Vertical breathing room between the text block and the icons. */
static const float kIconGap = 48.0f;

/* ================================================================== */
/*  Small state helpers                                                 */
/* ================================================================== */

static Rectangle tutorialSkipRect(void)
{
    const float w = 220.0f, h = 60.0f;
    return (Rectangle){ (float)s_width - w - 30.0f, 30.0f, w, h };
}

static void skipTutorial(GS *gs)
{
    PlaySound(gs->audio.menu_click);
    gs->skip_pressed_timer = 0.0f;
    if (gs->pressed_how_to_play) {
        gs->pressed_how_to_play = false;
        gs->currentscreen = MENU;
    } else {
        gs->show_tutorial = false;
        gs->currentscreen = GAME;
    }
}

static void startPage(GS *gs, int page)
{
    gs->tutorial_page      = page;
    gs->tutorial_charsShown = 0;
    gs->tutorial_charTimer  = 0.0f;
    gs->tutorial_alpha      = 0.0f;
    gs->tutorial_state      = tut_fadein;
    gs->skip_pressed_timer  = 0.0f;
}

/* Advance after the fade-out: next page, or leave the tutorial. */
static void nextPageOrExit(GS *gs)
{
    if (gs->tutorial_page + 1 < tutorialPageCount) {
        startPage(gs, gs->tutorial_page + 1);
        return;
    }
    gs->show_tutorial = false;
    gs->currentscreen = gs->pressed_how_to_play ? MENU : GAME;
    gs->pressed_how_to_play = false;
}

void initTutorial(GS *gs)
{
    startPage(gs, 0);
}

/* ================================================================== */
/*  Update                                                              */
/* ================================================================== */

void updateTutorial(GS *gs, float dt)
{
    ShowCursor();
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    if ((unsigned)gs->tutorial_page >= (unsigned)tutorialPageCount) {
        startPage(gs, 0);
        return;
    }

    const char *pageText = tutorialPages[gs->tutorial_page];
    const int   textLen  = (int)strlen(pageText);

    /* --- skip / back ------------------------------------------------ */
    Rectangle skipRect = tutorialSkipRect();
    Vector2   mouse    = GetMousePosition();

    if ((CheckCollisionPointRec(mouse, skipRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            || IsKeyPressed(KEY_ESCAPE)) {
        skipTutorial(gs);
        return;
    }

    /* --- hold ENTER to skip the whole tutorial ---------------------- */
    if (!gs->pressed_how_to_play) {
        bool holdSkip = IsKeyDown(KEY_ENTER) || IsKeyDown(KEY_KP_ENTER) ||
                        (IsGamepadAvailable(0) && IsGamepadButtonDown(0, GAMEPAD_BUTTON_MIDDLE_RIGHT));
        if (holdSkip) {
            gs->skip_pressed_timer += dt;
            if (gs->skip_pressed_timer >= gs->skip_duration) {
                gs->skip_pressed_timer = 0.0f;
                gs->show_tutorial = false;
                gs->currentscreen = GAME;
                return;
            }
        } else {
            gs->skip_pressed_timer = 0.0f;
        }
    }

    /* --- page state machine ------------------------------------------ */
    switch (gs->tutorial_state) {
    case tut_fadein:
        gs->tutorial_alpha += tutorial_fade_speed * dt;
        if (gs->tutorial_alpha >= 1.0f) {
            gs->tutorial_alpha = 1.0f;
            gs->tutorial_state = tut_typing;
        }
        break;

    case tut_typing:
        gs->tutorial_charTimer += dt;
        while (gs->tutorial_charTimer >= tutorial_char_interval
                && gs->tutorial_charsShown < textLen) {
            gs->tutorial_charTimer -= tutorial_char_interval;
            gs->tutorial_charsShown++;
        }
        if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)
             || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)))
                && gs->tutorial_charsShown < textLen) {
            gs->tutorial_charsShown = textLen;      /* reveal all instantly */
            PlaySound(gs->audio.menu_click);
        } else if (gs->tutorial_charsShown >= textLen) {
            gs->tutorial_state = tut_waiting;
        }
        break;

    case tut_waiting:
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)
            || (IsGamepadAvailable(0) && IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))) {
            gs->tutorial_state = tut_fadeout;
        }
        break;

    case tut_fadeout:
        gs->tutorial_alpha -= tutorial_fade_speed * dt;
        if (gs->tutorial_alpha <= 0.0f) {
            gs->tutorial_alpha = 0.0f;
            nextPageOrExit(gs);
        }
        break;
    }

    updateParallax(gs, 5.0f);
}

/* ================================================================== */
/*  Drawing helpers                                                     */
/* ================================================================== */

/* Typewriter text: every line is measured and centred horizontally. */
static void drawTypewriterText(GS *gs, const char *visible, float alpha)
{
    const Color textColor = Fade(WHITE, alpha);
    float y = kTextTop;
    char  lineBuf[kLineCap];

    const int len = (int)strlen(visible);
    for (int i = 0, lineStart = 0; i <= len; i++) {
        if (visible[i] != '\n' && visible[i] != '\0') continue;

        int lineLen = i - lineStart;
        if (lineLen >= kLineCap) lineLen = kLineCap - 1;
        memcpy(lineBuf, visible + lineStart, (size_t)lineLen);
        lineBuf[lineLen] = '\0';

        Vector2 size = MeasureTextEx(gs->cfonts.menu_font3, lineBuf, kFontSize, 0);
        DrawTextEx(gs->cfonts.menu_font3, lineBuf,
                   (Vector2){ (s_width - size.x) * 0.5f, y },
                   kFontSize, 0, textColor);
        y += kLineHeight;
        lineStart = i + 1;
    }
}

/* Blinking "Press ENTER ..." prompt, only while a page is fully shown. */
static void drawContinuePrompt(GS *gs)
{
    if (gs->tutorial_state != tut_waiting) return;

    const bool  lastPage = (gs->tutorial_page + 1 >= tutorialPageCount);
    const char *prompt   = lastPage ? "Press ENTER to start" : "Press ENTER to continue";

    if ((int)(GetTime() * 2.0) % 2 == 0) {
        Vector2 size = MeasureTextEx(gs->cfonts.menu_font3, prompt, kPromptSize, 0);
        DrawTextEx(gs->cfonts.menu_font3, prompt,
                   (Vector2){ (s_width - size.x) * 0.5f, s_height - kPromptMargin },
                   kPromptSize, 0, Fade(LIGHTGRAY, gs->tutorial_alpha));
    }
}

/* Bottom edge of the fully-typed text block (used to place icons under it). */
static float textBlockBottom(const char *visible)
{
    int lines = 1;
    for (const char *p = visible; *p; p++) {
        if (*p == '\n') lines++;
    }
    return kTextTop + lines * kLineHeight;
}

/* Draw every icon registered for the current page. The icons are centred
   horizontally on (cx + dx) and vertically inside the free band between
   the bottom of the text block and the "Press ENTER" prompt, so they can
   never overlap the text regardless of how many lines a page has. */
static void drawPageIcons(GS *gs, tex *textures, unsigned char alpha, float textBottom)
{
    if (gs->tutorial_state != tut_waiting) return;

    const float bandTop    = textBottom + kIconGap;
    const float bandBottom = (float)s_height - kPromptMargin - kPromptSize - 20.0f;
    const float bandCenter = (bandTop + (bandBottom > bandTop ? bandBottom : bandTop)) * 0.5f;

    for (int i = 0; i < kPageIconCount; i++) {
        const PageIcon *icon = &kPageIcons[i];
        if (icon->page != gs->tutorial_page) continue;

        /* Resolve texture + source rect (the enemy is a sprite sheet). */
        Texture2D tex;
        Rectangle src;
        switch (icon->icon) {
        case ICON_ENEMY: {
            tex = textures->enemy_attack;
            const float frameW = tex.width / 18.0f;
            src = (Rectangle){ 5.0f * frameW, 0.0f, frameW, (float)tex.height };
            break;
        }
        case ICON_SPIKES:          tex = textures->spike_sprite;         break;
        case ICON_BOMB:            tex = textures->bomb_sprite;          break;
        case ICON_HEALTH_PLATFORM: tex = textures->platform_health_drop; break;
        case ICON_HEALTH_ENEMY:    tex = textures->enemy_health_drop;    break;
        case ICON_POISON_GAS:      tex = textures->pgas;                 break;
        case ICON_LARGE_GAP:       tex = textures->Large_gap;            break;
        }
        if (icon->icon != ICON_ENEMY) {
            src = (Rectangle){ 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        }

        const Vector2 size = { src.width * icon->scale, src.height * icon->scale };
        const Rectangle dest = {
            s_width * 0.5f + icon->dx - size.x * 0.5f,
            bandCenter - size.y * 0.5f,
            size.x, size.y
        };
        DrawTexturePro(tex, src, dest, (Vector2){ 0.0f, 0.0f }, 0.0f,
                       Fade(WHITE, alpha / 255.0f));
    }
}

/* SKIP / BACK button with hover effect + hold-ENTER progress bar. */
static void drawSkipButton(GS *gs)
{
    const Rectangle r    = tutorialSkipRect();
    const bool      hover = CheckCollisionPointRec(GetMousePosition(), r);
    const Color     bg     = hover ? Fade(GOLD, 0.85f) : Fade(LIGHTGRAY, 0.55f);
    const Color     border = hover ? GOLD : LIGHTGRAY;

    DrawRectangleRounded(r, 0.25f, 8, bg);
    DrawRectangleRoundedLines(r, 0.25f, 8, border);

    const char *label = gs->pressed_how_to_play ? "BACK" : "SKIP >>";
    Vector2 lsize = MeasureTextEx(gs->cfonts.menu_font3, label, 36, 0);
    DrawTextEx(gs->cfonts.menu_font3, label,
               (Vector2){ r.x + (r.width  - lsize.x) * 0.5f,
                          r.y + (r.height - lsize.y) * 0.5f },
               36, 0, BLACK);

    /* hold-ENTER progress bar */
    if (gs->pressed_how_to_play || gs->skip_duration <= 0.0f) return;

    float frac = gs->skip_pressed_timer / gs->skip_duration;
    frac = frac < 0.0f ? 0.0f : (frac > 1.0f ? 1.0f : frac);

    const Rectangle bar = { r.x, r.y + r.height + 8.0f, r.width, 12.0f };
    DrawRectangleRounded(bar, 0.5f, 6, Fade(BLACK, 0.6f));
    if (frac > 0.0f) {
        DrawRectangleRounded((Rectangle){ bar.x, bar.y, bar.width * frac, bar.height },
                             0.5f, 6, GOLD);
    }
    DrawRectangleRoundedLines(bar, 0.5f, 6, LIGHTGRAY);

    const char *hint = "Hold ENTER to skip";
    Vector2 hsize = MeasureTextEx(gs->cfonts.menu_font3, hint, 22, 0);
    DrawTextEx(gs->cfonts.menu_font3, hint,
               (Vector2){ r.x + (r.width - hsize.x) * 0.5f, bar.y + bar.height + 6.0f },
               22, 0, LIGHTGRAY);
}

static void drawPageCounter(GS *gs)
{
    char buf[32];
    snprintf(buf, sizeof buf, "%d / %d", gs->tutorial_page + 1, tutorialPageCount);
    Vector2 size = MeasureTextEx(gs->cfonts.menu_font3, buf, 26, 0);
    DrawTextEx(gs->cfonts.menu_font3, buf,
               (Vector2){ (s_width - size.x) * 0.5f, 36.0f }, 26, 0, LIGHTGRAY);
}

/* ================================================================== */
/*  Draw                                                                */
/* ================================================================== */

void drawTutorial(GS *gs, tex *textures)
{
    drawBackgroundMenu(gs);
    /* two stacked 0xAA / 0x88 overlays ≈ a single 0xD7 overlay */
    DrawRectangle(0, 0, s_width, s_height, GetColor(0x000000D7));

    /* build the visible (typewriter) portion of the page text */
    int n = gs->tutorial_charsShown;
    if (n >= kVisibleCap) n = kVisibleCap - 1;
    char visible[kVisibleCap];
    memcpy(visible, tutorialPages[gs->tutorial_page], (size_t)n);
    visible[n] = '\0';

    const unsigned char alpha = (unsigned char)(gs->tutorial_alpha * 255.0f);

    drawTypewriterText(gs, visible, gs->tutorial_alpha);
    drawContinuePrompt(gs);
    drawPageIcons(gs, textures, alpha, textBlockBottom(visible));
    drawSkipButton(gs);
    drawPageCounter(gs);
}