#include "game.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "../utils/types.h"
#include "../utils/util.h"
#include "../utils/variables.h"
#include "../scoreboard/scoreboard.h"

#define MENU_PAUSED_HOME 1
#define MENU_PAUSED_RESUME 2
#define MENU_PAUSED_RESET 3

int game(AllegroConfig *alConfig, GameConfig *gameConfig, Activity *activity) {
    Platform platform;
    Ball balls[3];
    Axes mousePosition;
    ALLEGRO_BITMAP *background = NULL;
    ALLEGRO_BITMAP *clockBitmap = NULL;
    ALLEGRO_BITMAP *pauseBitmap = NULL;
    ALLEGRO_BITMAP *gameOverBitmap = NULL;
    ALLEGRO_BITMAP *helpBitmap = NULL;
    ALLEGRO_BITMAP *scoreBitmap = NULL;
    ALLEGRO_BITMAP *dialogScoreBitmap = NULL;
    ALLEGRO_BITMAP *healthBitmap[7];
    MenuPaused menuPaused;
    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE keyboardState;
    bool mouseBottomUp = false;
    bool shouldRedraw = false;
    Sounds sounds;
    Score **scores;
    Score newScore;
    int scoreNameCount = 0;

    scores = allocateScore();
    readScores(scores);
    resetNewScore(&newScore, &scoreNameCount);

    loadMenuPaused(&menuPaused);
    loadClock(&clockBitmap);
    loadGameBackground(&background);
    loadPauseScreen(&pauseBitmap);
    loadGameOverScreen(&gameOverBitmap);
    loadHelpScreen(&helpBitmap);
    loadHealthBar(healthBitmap);
    loadPlataform(&platform);
    loadBalls(balls);
    loadAllSounds(&sounds);
    loadScoreBitmap(&scoreBitmap);
    loadDialogScore(&dialogScoreBitmap);

    resetGame(gameConfig, balls);

    while (!gameConfig->exit && activity->game) {  // Main loop
        al_wait_for_event(alConfig->event_queue, &event);
        controllGameLevel(gameConfig, &platform, balls);
        increaseBallsSpeed(balls, gameConfig->level);
        if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
            mousePosition.x = event.mouse.x;
            mousePosition.y = event.mouse.y;
            if (!gameConfig->pause) {  // Change position only if playing
                platform.position.x = mousePosition.x;
            }
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            shouldRedraw = true;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            al_get_keyboard_state(&keyboardState);
            if(!gameConfig->hasScore){
              if (event.keyboard.keycode == ALLEGRO_KEY_Q) {// Quit game
                  gameConfig->exit = true;
              } else if (event.keyboard.keycode == ALLEGRO_KEY_P || event.keyboard.keycode == ALLEGRO_KEY_H) {
                togglePauseGame(gameConfig);
              }
            }
        } else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            mouseBottomUp = gameConfig->pause ? true : false;
        } else if (event.type == ALLEGRO_EVENT_KEY_CHAR && gameConfig->hasScore) {
            switch(event.keyboard.keycode){
              case ALLEGRO_KEY_ENTER:
                gameConfig->hasScore = false;
                sanitizeScoreBeforeSave(&newScore, gameConfig, scoreNameCount);
                saveScore(scores, newScore);
                resetNewScore(&newScore, &scoreNameCount);
                break;
              case ALLEGRO_KEY_BACKSPACE:
                if(scoreNameCount > 0){
                  scoreNameCount--;
                  newScore.name[scoreNameCount] = '\0';
                  placeCarret(newScore.name, 20, scoreNameCount - 1);
                }
                break;
              default:
                if(scoreNameCount <= 18){//Name size is 20
                  newScore.name[scoreNameCount] = event.keyboard.unichar;
                  newScore.name[scoreNameCount+1] = '\0';
                  placeCarret(newScore.name, 20, scoreNameCount);
                  scoreNameCount++;
                }
                break;
            }
        } else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            gameConfig->exit = true;
        }

        if (shouldRedraw && al_is_event_queue_empty(alConfig->event_queue)) {
            shouldRedraw = false;
            drawBackground(background);
            drawBalls(balls, *gameConfig);
            drawHealthBar(healthBitmap, *gameConfig);
            drawClockInfo(clockBitmap, gameConfig, alConfig->fontSmall);  // TODO: Stop time when it stops
            drawHelpTextHint(alConfig->fontSmall);
            drawScore(scoreBitmap, *gameConfig, alConfig->fontMedium);
            drawPlataform(platform);
            if (gameConfig->pause) {
                al_show_mouse_cursor(alConfig->display);
                al_ungrab_mouse();

                if (mouseBottomUp) {  // Has clicked
                    mouseBottomUp = false;
                    handleClickOnMenu(activity, gameConfig, &menuPaused, balls, mousePosition);
                }

                if (isHoverButton(menuPaused, mousePosition) && !gameConfig->hasScore) {  // Change the cursor when hovering buttons
                    al_set_system_mouse_cursor(alConfig->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
                } else {
                    al_set_system_mouse_cursor(alConfig->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                }

                if (al_key_down(&keyboardState, ALLEGRO_KEY_P) && !gameConfig->hasScore) {
                    drawPauseScreen(pauseBitmap);
                    drawMenuPaused(&menuPaused);
                } else if (gameConfig->gameOver) {
                    drawGameOverScreen(gameOverBitmap);
                    drawMenuPaused(&menuPaused);
                    if(gameConfig->hasScore){
                      drawDialogScore(dialogScoreBitmap, newScore, alConfig->fontBig);
                    }
                } else {
                    drawHelpScreen(helpBitmap);
                }
            } else {
                moveBalls(balls, gameConfig->gravity, gameConfig->level);
                checkBallsNextAction(balls, platform, gameConfig, scores, sounds);
                al_hide_mouse_cursor(alConfig->display);
                al_grab_mouse(alConfig->display);
            }
            al_flip_display();
        }
    }

    freeScores(scores);

    return 0;
}

void loadMenuPaused(MenuPaused *menuPaused) {
    int margin = 10, heightButton, widthButton, posX, posY;
    const char *resumePath = "assets/menu/buttons/resumeButton.png";
    const char *resetPath = "assets/menu/buttons/resetButton.png";
    const char *homePath = "assets/menu/buttons/homeButton.png";

    menuPaused->resume.bitmap = al_load_bitmap(resumePath);
    menuPaused->reset.bitmap = al_load_bitmap(resetPath);
    menuPaused->home.bitmap = al_load_bitmap(homePath);

    widthButton = al_get_bitmap_width(menuPaused->resume.bitmap);
    heightButton = al_get_bitmap_height(menuPaused->resume.bitmap);
    posX = (DISPLAY_WIDTH / 2) - (widthButton / 2);
    posY = 250;

    menuPaused->home.position.x = posX;
    menuPaused->home.position.y = posY;
    menuPaused->home.endPosition.x = posX + widthButton;
    menuPaused->home.endPosition.y = menuPaused->home.position.y + heightButton;

    menuPaused->resume.position.x = posX;
    menuPaused->resume.position.y = menuPaused->home.endPosition.y + margin;
    menuPaused->resume.endPosition.x = posX + widthButton;
    menuPaused->resume.endPosition.y =
        menuPaused->resume.position.y + heightButton;

    menuPaused->reset.position.x = posX;
    menuPaused->reset.position.y = menuPaused->resume.endPosition.y + margin;
    menuPaused->reset.endPosition.x = posX + widthButton;
    menuPaused->reset.endPosition.y =
        menuPaused->reset.position.y + heightButton;
}

void drawMenuPaused(MenuPaused *menuPaused) {
    al_draw_bitmap(menuPaused->home.bitmap, menuPaused->home.position.x, menuPaused->home.position.y, 0);
    al_draw_bitmap(menuPaused->resume.bitmap, menuPaused->resume.position.x, menuPaused->resume.position.y, 0);
    al_draw_bitmap(menuPaused->reset.bitmap, menuPaused->reset.position.x, menuPaused->reset.position.y, 0);
}

void drawHelpTextHint(ALLEGRO_FONT *font){
  al_draw_text(font, al_map_rgb(0, 0, 0), 20, 80, 0, "Pressione H");
  al_draw_text(font, al_map_rgb(0, 0, 0), 20, 100, 0, "para obter ajudar");
}

void loadDialogScore(ALLEGRO_BITMAP **dialogScoreBitmap){
  const char *dialogPath = "assets/game/dialog.png";
  *dialogScoreBitmap = al_load_bitmap(dialogPath);
}

void drawDialogScore(ALLEGRO_BITMAP *dialogScoreBitmap, Score newScore, ALLEGRO_FONT *font){
  Axes from = {0, 0};
  Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
  drawResized(dialogScoreBitmap, from, to);
  al_draw_text(font, al_map_rgb(0, 0, 0), 210, 375, 0, newScore.name);
}

void loadHealthBar(ALLEGRO_BITMAP **healthBitmap) {
    char healthFile[30];
    for (int i = 0; i <= 6; i++) {
        sprintf(healthFile, "assets/game/health/life%d.png", i);
        healthBitmap[i] = al_load_bitmap(healthFile);
    }
}

void drawHealthBar(ALLEGRO_BITMAP **healthBitmap, GameConfig gameConfig) {
    int posX = 20;
    int posY = 20;
    al_draw_bitmap(healthBitmap[gameConfig.life], posX, posY, 0);
}

void loadScoreBitmap(ALLEGRO_BITMAP **scoreBitmap) {
    const char *scorePath = "assets/game/score.png";
    *scoreBitmap = al_load_bitmap(scorePath);
}

void drawScore(ALLEGRO_BITMAP *scoreBitmap, GameConfig gameConfig, ALLEGRO_FONT *font) {
    int scoreWidth = al_get_bitmap_width(scoreBitmap);
    int scoreHeight = al_get_bitmap_height(scoreBitmap);
    int margin = 20;
    Axes scorePos = {DISPLAY_WIDTH - (scoreWidth + margin), margin / 4};
    al_draw_bitmap(scoreBitmap, scorePos.x, scorePos.y, 0);
    Axes scoresTextPos = {scorePos.x + margin, scorePos.y + scoreHeight / 2 + margin / 4};
    char points[7];
    if (gameConfig.score < 999999) {//Max score
        sprintf(points, "%06d", gameConfig.score);
        al_draw_text(font, al_map_rgb(0, 0, 0), scoresTextPos.x, scoresTextPos.y, 0, points);
    } else {
        al_draw_text(font, al_map_rgb(0, 0, 0), scoresTextPos.x, scoresTextPos.y, 0, "999999");
    }
}

void loadClock(ALLEGRO_BITMAP **clockBitmap) {
    const char *clockPath = "assets/game/clock.png";
    *clockBitmap = al_load_bitmap(clockPath);
}

void drawClockInfo(ALLEGRO_BITMAP *clockBitmap, GameConfig *gameConfig, ALLEGRO_FONT *font) {
    Axes clock = {20, 50};
    al_draw_bitmap(clockBitmap, clock.x, clock.y, 0);
    int clockWidth = al_get_bitmap_width(clockBitmap);
    char timeText[6];
    int currentTime;
    if(gameConfig->pause == false){
      currentTime = (int)getTime(gameConfig->timeStart);
    } else {
      currentTime = gameConfig->timePassed;
    }
    int minutes = currentTime / 60;
    int seconds = currentTime % 60;
    Axes clockTime = {clock.x + clockWidth + 10, clock.y};
    sprintf(timeText, "%02d:%02d", minutes, seconds);
    al_draw_text(font, al_map_rgb(0, 0, 0), clockTime.x, clockTime.y, 0, timeText);
}

void loadGameBackground(ALLEGRO_BITMAP **backgroundBitmap) {
    const char *background = "assets/game/background.png";
    *backgroundBitmap = al_load_bitmap(background);
}

void loadPauseScreen(ALLEGRO_BITMAP **pauseBitmap) {
    const char *pausePath = "assets/game/pause.png";
    *pauseBitmap = al_load_bitmap(pausePath);
}

void drawPauseScreen(ALLEGRO_BITMAP *pauseBitmap) {
    Axes from = {0, 0};
    Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
    drawResized(pauseBitmap, from, to);
}

void loadGameOverScreen(ALLEGRO_BITMAP **gameOverBitmap) {
    const char *gameOverPath = "assets/game/gameOver.png";
    *gameOverBitmap = al_load_bitmap(gameOverPath);
}

void drawGameOverScreen(ALLEGRO_BITMAP *gameOverBitmap) {
    Axes from = {0, 0};
    Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
    drawResized(gameOverBitmap, from, to);
}

void loadHelpScreen(ALLEGRO_BITMAP **helpBitmap) {
    const char *helpPath = "assets/game/help.png";
    *helpBitmap = al_load_bitmap(helpPath);
}

void drawHelpScreen(ALLEGRO_BITMAP *helpBitmap) {
    Axes from = {0, 0};
    Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
    drawResized(helpBitmap, from, to);
}

void loadAllSounds(Sounds *sounds) {
    const int totalSounds = 2;

    al_reserve_samples(totalSounds);

    const char *impactSound = "assets/game/sounds/impact_ball.wav";
    const char *damageSound = "assets/game/sounds/damage.wav";

    sounds->impactBall = al_load_sample(impactSound);
    sounds->damage = al_load_sample(damageSound);
}

void destroySounds(Sounds *sounds) { al_destroy_sample(sounds->impactBall); }

void playSound(ALLEGRO_SAMPLE *sound) {
    al_play_sample(sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void drawBackground(ALLEGRO_BITMAP *backgroundBitmap) {
    al_draw_bitmap(backgroundBitmap, 0, 0, 0);
}

void loadPlataform(Platform *platform) {
    const char *bitmapLeft = "assets/game/platform/platformLeft.png";
    const char *bitmapMid = "assets/game/platform/platformMid.png";
    const char *bitmapRight = "assets/game/platform/platformRight.png";

    platform->level = 1;  // Max is 2
    platform->position.x = DISPLAY_WIDTH / 2;
    platform->position.y = DISPLAY_HEIGHT - 50;
    platform->bitmapLeft = al_load_bitmap(bitmapLeft);
    platform->bitmapMid = al_load_bitmap(bitmapMid);
    platform->bitmapRight = al_load_bitmap(bitmapRight);

    const int widthBitmap =
        al_get_bitmap_width(platform->bitmapMid);  // The image size
    const int heightBitmap =
        widthBitmap / 2;  // The image has the other half empty

    platform->width = widthBitmap;
    platform->height = heightBitmap;
}

void drawPlataform(Platform platform) {
    const int elevation = DISPLAY_HEIGHT - 50;
    int drawCounter = 0;
    int positionFraction;  // The fraction to calc the right position

    if (platform.level == 1) {
        positionFraction = 1.5 * platform.width;
    } else {
        positionFraction = platform.width;
    }

    al_draw_bitmap(platform.bitmapLeft, platform.position.x - positionFraction, elevation, 0);

    if (platform.level == 1) {
        drawCounter++;
        al_draw_bitmap(platform.bitmapMid, platform.position.x - 0.5 * platform.width, elevation, 0);
    } else {
        al_draw_bitmap(platform.bitmapRight, platform.position.x, elevation, 0);
    }

    drawCounter++;
    if (platform.level == 1) {
        al_draw_bitmap(platform.bitmapRight, platform.position.x + 0.5 * platform.width, elevation, 0);
    }
}

void loadBalls(Ball *balls) {
    const char *ballBlue = "assets/game/balls/32x32/ballBlue.png";
    const char *ballPink = "assets/game/balls/32x32/ballPink.png";
    const char *ballRed = "assets/game/balls/32x32/ballRed.png";

    balls[0].bitmap = al_load_bitmap(ballBlue);
    balls[1].bitmap = al_load_bitmap(ballPink);
    balls[2].bitmap = al_load_bitmap(ballRed);

    initBalls(balls);
}

void drawBalls(Ball *balls, GameConfig gameConfig) {
    const int totalBalls = gameConfig.level == 1 ? 2 : 3;
    int posCornerX, posCornerY, imgWidth, imgHeight;
    for (int i = 0; i < totalBalls; i++) {
        posCornerX = balls[i].position.x - balls[i].radius;
        posCornerY = balls[i].position.y - balls[i].radius;
        imgWidth = al_get_bitmap_width(balls[i].bitmap);
        imgHeight = al_get_bitmap_height(balls[i].bitmap);
        al_draw_rotated_bitmap(balls[i].bitmap, imgWidth / 2, imgHeight / 2,
                               posCornerX, posCornerY, balls[i].rotation, 0);
    }
}

void resetBall(Ball *ball, Ball *previousBall) {
    const int radius = (int)al_get_bitmap_width(ball->bitmap) / 2;
    ball->radius = radius;
    ball->speed.x = 1 + getRandomNumber(2);
    if (previousBall != NULL) {
        do {
            ball->speed.y = -(5 + getRandomNumber(5));
        } while (previousBall->speed.y ==
                 ball->speed.y);  // Makes sure that the speed is different
    } else {
        ball->speed.y = -(5);
    }
    ball->position.x = 50;
    ball->position.y = DISPLAY_HEIGHT - radius - 10;
    ball->shouldMove = true;
    ball->freezedTime = 0;
    ball->rotation = 0;
}

void initBalls(Ball *balls) {
    for (int i = 0; i < 3; i++) {
        if (i == 0) {
            resetBall(&balls[i], NULL);
        } else {
            resetBall(&balls[i], &balls[i - 1]);
        }
    }
}

void tryToUnfreezeBall(Ball *ball) {
    const int waitingTime = 2;
    if (getTime(ball->freezedTime) >= waitingTime) {
        ball->shouldMove = true;
    }
}

void moveBalls(Ball *balls, int gravity, int currentLevel) {
    const int totalBalls = currentLevel == 1 ? 2 : 3;
    const float rotationSpeed = 0.1;
    float completeRound = 2 * ALLEGRO_PI;
    for (int i = 0; i < totalBalls; i++) {
        if (balls[i].shouldMove) {
            balls[i].position.x += balls[i].speed.x;
            balls[i].position.y += balls[i].speed.y;
            balls[i].speed.y -= ((float)gravity / 1000);
            balls[i].rotation += rotationSpeed;
            if (balls[i].rotation >= completeRound) {
                balls[i].rotation = balls[i].rotation - completeRound;
            }
        } else {
            tryToUnfreezeBall(&balls[i]);
        }
    }
}

bool isTouchingPlataform(Ball ball, Platform platform) {
    Axes leftTop, rightBottom, cloneBall;
    int shiftAmount;

    if (platform.level == 1) {
        shiftAmount = platform.width * 1.5;
    } else {
        shiftAmount = platform.width;
    }
    leftTop.x = platform.position.x - shiftAmount;
    leftTop.y = platform.position.y;

    rightBottom.x = platform.position.x + shiftAmount;
    rightBottom.y = platform.position.y + platform.height;

    cloneBall = ball.position;
    cloneBall.x += ball.radius;  // Test right
    if (isIntercepting(leftTop, rightBottom, ball.position)) {
        return true;
    }

    cloneBall = ball.position;
    cloneBall.x -= ball.radius;  // Test left
    if (isIntercepting(leftTop, rightBottom, ball.position)) {
        return true;
    }

    cloneBall = ball.position;
    cloneBall.y += ball.radius;  // Test bottom
    if (isIntercepting(leftTop, rightBottom, ball.position)) {
        return true;
    }

    cloneBall = ball.position;  // Test center
    if (isIntercepting(leftTop, rightBottom, cloneBall)) {
        return true;
    }

    return false;
}

void checkBallsNextAction(Ball *balls, Platform platform, GameConfig *gameConfig, Score **scores, Sounds sounds) {
    TouchingWalls touchingWalls;
    const int totalBalls = gameConfig->level == 1 ? 2 : 3;
    for (int i = 0; i < totalBalls; i++) {
        if (isTouchingPlataform(balls[i], platform)) {
            if (balls[i].shouldMove && balls[i].speed.y > 0) {
                playSound(sounds.impactBall);
                handleBallTouchinPlatform(&balls[i], platform);
                gameConfig->score += 20;  // Add score
            }
        }
        touchingWalls = getTouchingWall(balls[i]);
        if (isTouchingWall(touchingWalls)) {
            handleBallTouchingWall(touchingWalls, &balls[i]);
            if (touchingWalls.bottom) {
                decreaseLife(gameConfig, scores);
                playSound(sounds.damage);
            } else {
                playSound(sounds.impactBall);
            }
        }
    }
}

void handleBallTouchinPlatform(Ball *ball, Platform platform) {
    const float maxSpeedX = 7;
    const float shiftAmount = platform.level == 1 ? platform.width * 1.5 : platform.width;
    const float newSpeedX = maxSpeedX * (absolute(platform.position.x - ball->position.x) / shiftAmount);  // More distant from the platform center
                                                                                                          // means higher velocity
    if (ball->speed.y > 0) { // If Going down
        ball->speed.y = -ball->speed.y;  // Throw ball up
        ball->position.y =
            platform.position.y;  // Move ball up (not touching platform)
        if (ball->position.x > platform.position.x) {
            ball->speed.x = +newSpeedX;
        } else {
            ball->speed.x = -newSpeedX;
        }
    }
}

TouchingWalls getTouchingWall(Ball ball) {
    int ballDiameter = ball.radius * 2;
    TouchingWalls touchingWalls = {false, false, false, false};
    Axes cloneBall;

    Axes topWall_topLeft = {0, -ballDiameter};
    Axes topWall_bottomRight = {DISPLAY_WIDTH, 0};

    Axes rightWall_topLeft = {DISPLAY_WIDTH, 0};
    Axes rightWall_bottomRight = {DISPLAY_WIDTH + ballDiameter, DISPLAY_HEIGHT};

    Axes bottomWall_topLeft = {0, DISPLAY_HEIGHT};
    Axes bottomWall_bottomRight = {DISPLAY_WIDTH,
                                   DISPLAY_HEIGHT + ballDiameter};

    Axes leftWall_topLeft = {-ballDiameter, 0};
    Axes leftWall_bottomRight = {0, DISPLAY_HEIGHT};

    cloneBall = ball.position;
    cloneBall.y -= ball.radius;  // TOP SHIFT
    if (isIntercepting(topWall_topLeft, topWall_bottomRight, cloneBall)) {
        touchingWalls.top = true;
    }

    cloneBall = ball.position;
    cloneBall.x += ball.radius;  // RIGHT SHIFT
    if (isIntercepting(rightWall_topLeft, rightWall_bottomRight, cloneBall)) {
        touchingWalls.right = true;
    }

    cloneBall = ball.position;
    cloneBall.y += ball.radius;  // BOTTOM SHIFT
    if (isIntercepting(bottomWall_topLeft, bottomWall_bottomRight, cloneBall)) {
        touchingWalls.bottom = true;
    }

    cloneBall = ball.position;
    cloneBall.x -= ball.radius;  // LEFT SHIFT
    if (isIntercepting(leftWall_topLeft, leftWall_bottomRight, cloneBall)) {
        touchingWalls.left = true;
    }

    return touchingWalls;
}

bool isTouchingWall(TouchingWalls touchingWalls) {
    return touchingWalls.top || touchingWalls.right || touchingWalls.bottom ||
           touchingWalls.left;
}

void handleBallTouchingWall(TouchingWalls touchingWalls, Ball *ball) {
    if (touchingWalls.top) {
        ball->speed.y = -ball->speed.y;
        ball->position.y = ball->radius + 1;
    }

    if (touchingWalls.right || touchingWalls.left) {
        ball->speed.x = -ball->speed.x;
        if (touchingWalls.right) {
            ball->position.x = DISPLAY_WIDTH - (ball->radius + 1);
        } else {
            ball->position.x = ball->radius + 1;
        }
    }

    if (touchingWalls.bottom) {
        resetBall(ball, NULL);
        ball->shouldMove = false;
        ball->freezedTime = al_get_time();
    }
}

void decreaseLife(GameConfig *gameConfig, Score **scores) {
    if (gameConfig->life > 0) {
        gameConfig->life--;
        if (gameConfig->life == 0) {
          gameConfig->gameOver = true;
          gameConfig->pause = true;
          gameConfig->hasScore = getScorePosition(scores, gameConfig->score) != -1 ? true : false;
        }
    }
}

bool isHoverButton(MenuPaused menuPaused, Axes position) {
    return isIntercepting(menuPaused.home.position, menuPaused.home.endPosition,
                          position) ||
           isIntercepting(menuPaused.resume.position,
                          menuPaused.resume.endPosition, position) ||
           isIntercepting(menuPaused.reset.position,
                          menuPaused.reset.endPosition, position);
}

int getClickedMenu(MenuPaused *menuPaused, Axes position) {
    if (isIntercepting(menuPaused->home.position, menuPaused->home.endPosition, position)) {
        return MENU_PAUSED_HOME;
    } else if (isIntercepting(menuPaused->resume.position, menuPaused->resume.endPosition, position)) {
        return MENU_PAUSED_RESUME;
    } else if (isIntercepting(menuPaused->reset.position, menuPaused->reset.endPosition, position)) {
        return MENU_PAUSED_RESET;
    } else {
        return -1;  // None
    }
}

void handleClickOnMenu(Activity *activity, GameConfig *gameConfig, MenuPaused *menuPaused, Ball *balls, Axes position) {
    if(gameConfig->hasScore == false){//Not showing the ScoreDialog
      switch (getClickedMenu(menuPaused, position)) {
          case MENU_PAUSED_HOME:
              activity->game = false;
              activity->menu = true;
              break;
          case MENU_PAUSED_RESUME:
              togglePauseGame(gameConfig);
              break;
          case MENU_PAUSED_RESET:
              resetGame(gameConfig, balls);  // Same as reset
      }
    }
}

void togglePauseGame(GameConfig *gameConfig) {
    if (!gameConfig->gameOver) {
        gameConfig->pause = !gameConfig->pause;
        if(gameConfig->pause == false){//Is resuming game
          gameConfig->timeStart = al_get_time() - gameConfig->timePassed;//Go back in time
          gameConfig->timePassed = 0;
        } else {//Is pausing game
          gameConfig->timePassed = getTime(gameConfig->timeStart);
        }
    }
}

void resetGame(GameConfig *gameConfig, Ball *balls) {
    initGameConfig(gameConfig);
    initBalls(balls);
}

void controllGameLevel(GameConfig *gameConfig, Platform *platform, Ball *balls) {
    if (gameConfig->score >= 200) {
        if (gameConfig->level == 1) {  // Coming from level 1
            resetBall(&balls[2], &balls[1]);
        }
        if (gameConfig->score >= 700) {  // Level 4
            gameConfig->level = 4;
            platform->level = 2;
        } else if (gameConfig->score >= 500) {  // Level 3
            gameConfig->level = 3;
        } else {  // Level 2
            gameConfig->level = 2;
        }
    } else {
        gameConfig->level = 1;
        platform->level = 1;
    }
}

void increaseBallsSpeed(Ball *balls, int level) {
    if (level >= 3) {
        for (int i = 0; i < 3; i++) {
            if (balls[i].speed.y > 0 && balls[i].speed.y < 7) {
                balls[i].speed.y += 3;
            } else if (balls[i].speed.y > -7 && balls[i].speed.y < 0) {
                balls[i].speed.y += -3;
            }
        }
    }
}

void placeCarret(char *name, int maxSize, int currentPos){
  if(currentPos < (maxSize - 2)){
    name[currentPos+1] = '|';
    name[currentPos+2] = '\0';
  }
}

void resetNewScore(Score *newScore, int *counter){
  newScore->name[0] = '|';
  newScore->name[1] = '\0';
  *counter = 0;
}

void sanitizeScoreBeforeSave(Score *newScore, GameConfig *gameConfig, int counter){
  newScore->points = gameConfig->score;
  newScore->name[counter] = '\0';
}
