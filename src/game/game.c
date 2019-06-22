#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../utils/util.h"
#include "../utils/types.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define FPS 80

typedef struct{
  int height;
  int width;
  Axes position;//Leftmost side
  int level;//Max is 2
  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_BITMAP *bitmapLeft;
  ALLEGRO_BITMAP *bitmapMid;
  ALLEGRO_BITMAP *bitmapRight;
} Platform;

typedef struct{
  ALLEGRO_BITMAP *bitmap;
  Axes position;
  Axes speed;
  int radius;
  bool shouldMove;
  float freezedTime;
  float rotation;
} Ball;

typedef struct{
  bool top;
  bool right;
  bool bottom;
  bool left;
} TouchingWalls;

typedef struct{
  ALLEGRO_SAMPLE *impactBall;
  ALLEGRO_SAMPLE *damage;
} Sounds;

void initGameConfig(GameConfig *gameConfig);
void loadHealthBar(ALLEGRO_BITMAP **healthBitmap);
void drawHealthBar(ALLEGRO_BITMAP **healthBitmap, GameConfig gameConfig);
void loadScoreBitmap(ALLEGRO_BITMAP **scoreBitmap);
void drawScore(ALLEGRO_BITMAP *scoreBitmap, GameConfig gameConfig, ALLEGRO_FONT *font);
void loadClock(ALLEGRO_BITMAP **clockBitmap);
void drawClockInfo(ALLEGRO_BITMAP *clockBitmap, double startTime, ALLEGRO_FONT *font);
void loadGameBackground(ALLEGRO_BITMAP **backgroundBitmap);
void loadPauseScreen(ALLEGRO_BITMAP **pauseBitmap);
void drawPauseScreen(ALLEGRO_BITMAP *pauseBitmap);
void loadAllSounds(Sounds *sounds);
void destroySounds(Sounds *sounds);
void drawBackground(ALLEGRO_BITMAP *backgroundBitmap);
void loadPlataform(Platform *plataform);
void drawPlataform(Platform plataform);
void loadBalls(Ball *balls);
void drawBalls(Ball *balls, GameConfig gameConfig);
void resetBall(Ball *ball, Ball *previousBall);
void initBalls(Ball *balls);
void moveBalls(Ball *balls, int gravity, int currentLevel);
bool isTouchingPlataform(Ball ball, Platform platform);
void checkBallsNextAction(Ball *balls, Platform platform, GameConfig *gameConfig, Sounds sounds);
void handleBallTouchinPlatform(Ball *ball, float platformY);
TouchingWalls getTouchingWall(Ball ball);
bool isTouchingWall(TouchingWalls touchingWalls);
void handleBallTouchingWall(TouchingWalls touchingWalls, Ball *ball);
void decreaseLife(GameConfig *gameConfig);
void tryToUnfreezeBall(Ball *ball);
void resetTime(double *time);
double getTime(double time);

int main(){
  AllegroConfig alConfig = {DISPLAY_WIDTH, DISPLAY_HEIGHT, FPS, NULL, NULL, NULL};
  GameConfig gameConfig;
  Platform platform;
  Ball balls[3];
  ALLEGRO_BITMAP *background = NULL;
  ALLEGRO_BITMAP *clockBitmap = NULL;
  ALLEGRO_BITMAP *pauseBitmap = NULL;
  ALLEGRO_BITMAP *scoreBitmap = NULL;
  ALLEGRO_BITMAP *healthBitmap[7];
  ALLEGRO_EVENT event;
  double startTime;
  bool shouldRedraw = false;
  Sounds sounds;

  if(!initAllegro(&alConfig)){
    return -1;
  }

  initGameConfig(&gameConfig);
  loadClock(&clockBitmap);
  loadGameBackground(&background);
  loadPauseScreen(&pauseBitmap);
  loadHealthBar(healthBitmap);
  loadPlataform(&platform);
  loadBalls(balls);
  loadAllSounds(&sounds);
  loadScoreBitmap(&scoreBitmap);
  resetTime(&startTime);

  while (!gameConfig.gameOver) {//Main loop
    al_wait_for_event(alConfig.event_queue, &event);
    if(event.type == ALLEGRO_EVENT_MOUSE_AXES){
      platform.position.x = event.mouse.x;
    } else if(event.type == ALLEGRO_EVENT_TIMER){
      shouldRedraw = true;
    } else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      break;
    }

    if(shouldRedraw && al_is_event_queue_empty(alConfig.event_queue)){
      shouldRedraw = false;
      drawBackground(background);
      drawBalls(balls, gameConfig);
      drawHealthBar(healthBitmap, gameConfig);
      drawClockInfo(clockBitmap, startTime, alConfig.fontSmall);
      drawScore(scoreBitmap, gameConfig, alConfig.fontMedium);
      drawPlataform(platform);
      moveBalls(balls, gameConfig.gravity, gameConfig.level);
      checkBallsNextAction(balls, platform, &gameConfig, sounds);
      // drawPauseScreen(pauseBitmap);
      al_flip_display();
    }
  }

  destroyAllegro(&alConfig);
}

void initGameConfig(GameConfig *gameConfig){
  gameConfig->score = 0;
  gameConfig->level = 1;
  gameConfig->gravity = -60;//-60 m/s²
  gameConfig->life = 6;
  gameConfig->pause = false;
  gameConfig->gameOver = false;
}

void loadHealthBar(ALLEGRO_BITMAP **healthBitmap){
  char healthFile[30];
  for(int i = 0; i <= 6; i++){
    sprintf(healthFile, "assets/game/health/life%d.png", i);
    healthBitmap[i] = al_load_bitmap(healthFile);
  }
}

void drawHealthBar(ALLEGRO_BITMAP **healthBitmap, GameConfig gameConfig){
  int posX = 20;
  int posY = 20;
  al_draw_bitmap(healthBitmap[gameConfig.life], posX, posY, 0);
}

void loadScoreBitmap(ALLEGRO_BITMAP **scoreBitmap){
  const char *scorePath = "assets/game/score.png";
  *scoreBitmap = al_load_bitmap(scorePath);
}

void drawScore(ALLEGRO_BITMAP *scoreBitmap, GameConfig gameConfig, ALLEGRO_FONT *font){
  int scoreWidth = al_get_bitmap_width(scoreBitmap);
  int scoreHeight = al_get_bitmap_height(scoreBitmap);
  int margin = 20;
  Axes scorePos = {DISPLAY_WIDTH - (scoreWidth + margin),  margin/4};
  al_draw_bitmap(scoreBitmap, scorePos.x, scorePos.y, 0);
  Axes scoresTextPos = {scorePos.x + margin, scorePos.y + scoreHeight/2 + margin/4};
  char points[7];
  if(gameConfig.score < 999999){
    sprintf(points, "%06d", gameConfig.score);
    al_draw_text(font, al_map_rgb(0,0,0), scoresTextPos.x, scoresTextPos.y, 0, points);
  } else {
    al_draw_text(font, al_map_rgb(0,0,0), scoresTextPos.x, scoresTextPos.y, 0, "999999");
  }
}

void loadClock(ALLEGRO_BITMAP **clockBitmap){
  const char *clockPath = "assets/game/clock.png";
  *clockBitmap = al_load_bitmap(clockPath);
}

void drawClockInfo(ALLEGRO_BITMAP *clockBitmap, double startTime, ALLEGRO_FONT *font){
  Axes clock = {20, 50};
  al_draw_bitmap(clockBitmap, clock.x, clock.y, 0);

  int clockWidth = al_get_bitmap_width(clockBitmap);
  char timeText[6];
  int currentTime = (int) getTime(startTime);
  int minutes = currentTime/60;
  int seconds = currentTime%60;
  Axes clockTime = {clock.x + clockWidth + 10, clock.y};
  sprintf(timeText, "%02d:%02d", minutes, seconds);
  al_draw_text(font, al_map_rgb(0, 0, 0), clockTime.x, clockTime.y, 0, timeText);
}

void loadGameBackground(ALLEGRO_BITMAP **backgroundBitmap){
  const char *background = "assets/game/background.png";
  *backgroundBitmap = al_load_bitmap(background);
}

void loadPauseScreen(ALLEGRO_BITMAP **pauseBitmap){
  const char *pausePath = "assets/game/pause.png";
  *pauseBitmap = al_load_bitmap(pausePath);
}

void drawPauseScreen(ALLEGRO_BITMAP *pauseBitmap){
  Axes from = {0, 0};
  Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
  drawResized(pauseBitmap, from, to);
}

void loadAllSounds(Sounds *sounds){
  const int totalSounds = 2;

  al_reserve_samples(totalSounds);

  const char *impactSound = "assets/game/sounds/impact_ball.wav";
  const char *damageSound = "assets/game/sounds/damage.wav";

  sounds->impactBall = al_load_sample(impactSound);
  sounds->damage = al_load_sample(damageSound);
}

void destroySounds(Sounds *sounds){
  al_destroy_sample(sounds->impactBall);
}

void playSound(ALLEGRO_SAMPLE *sound){
  al_play_sample(sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void drawBackground(ALLEGRO_BITMAP *backgroundBitmap){
  al_draw_bitmap(backgroundBitmap, 0, 0, 0);
}

void loadPlataform(Platform *platform){
  const char *bitmapLeft = "assets/game/platform/platformLeft.png";
  const char *bitmapMid = "assets/game/platform/platformMid.png";
  const char *bitmapRight = "assets/game/platform/platformRight.png";

  platform->level = 1;//Max is 2
  platform->position.x = DISPLAY_WIDTH / 2;
  platform->position.y = DISPLAY_HEIGHT - 50;
  platform->bitmapLeft = al_load_bitmap(bitmapLeft);
  platform->bitmapMid = al_load_bitmap(bitmapMid);
  platform->bitmapRight = al_load_bitmap(bitmapRight);

  const int widthBitmap = al_get_bitmap_width(platform->bitmapMid);//The image size
  const int heightBitmap = widthBitmap/2;//The image has the other half empty

  platform->width = widthBitmap;
  platform->height = heightBitmap;
}

void drawPlataform(Platform platform){
  const int elevation = DISPLAY_HEIGHT - 50;
  int drawCounter = 0;
  int positionFraction;//The fraction to calc the right position

  if(platform.level == 1){
    positionFraction = 1.5*platform.width;
  } else {
    positionFraction = platform.width;
  }

  al_draw_bitmap(platform.bitmapLeft, platform.position.x - positionFraction, elevation, 0);

  if(platform.level == 1){
    drawCounter++;
    al_draw_bitmap(platform.bitmapMid, platform.position.x - 0.5*platform.width, elevation, 0);
  }

  drawCounter++;
  if(platform.level == 1){
    al_draw_bitmap(platform.bitmapRight, platform.position.x + 0.5*platform.width, elevation, 0);
  } else {
    al_draw_bitmap(platform.bitmapRight, platform.position.x, elevation, 0);
  }
}

void loadBalls(Ball *balls){
  const char *ballBlue = "assets/game/balls/35x35/ballBlue.png";
  const char *ballPink = "assets/game/balls/35x35/ballPink.png";
  const char *ballRed = "assets/game/balls/35x35/ballRed.png";

  balls[0].bitmap = al_load_bitmap(ballBlue);
  balls[1].bitmap = al_load_bitmap(ballPink);
  balls[2].bitmap = al_load_bitmap(ballRed);

  initBalls(balls);
}

void drawBalls(Ball *balls, GameConfig gameConfig){
  const int totalBalls = gameConfig.level == 1 ? 2 : 3;
  int posCornerX, posCornerY;
  for(int i = 0; i < totalBalls; i++){
    posCornerX = balls[i].position.x - balls[i].radius;
    posCornerY = balls[i].position.y - balls[i].radius;
    al_draw_bitmap(balls[i].bitmap, posCornerX, posCornerY, 0);
  }
}

void resetBall(Ball *ball, Ball *previousBall){
  const int radius = (int) al_get_bitmap_width(ball->bitmap) / 2;
  ball->radius = radius;
  ball->speed.x = 1 + getRandomNumber(2);
  if(previousBall != NULL){
    do{
      ball->speed.y = -(5 + getRandomNumber(5));
    }while(previousBall->speed.y == ball->speed.y);
  } else {
    ball->speed.y = -(5 + getRandomNumber(5));
  }
  ball->position.x = 50;
  ball->position.y = DISPLAY_HEIGHT - radius - 10;
  ball->shouldMove = true;
  ball->freezedTime = 0;
  ball->rotation = 0;
}

void initBalls(Ball *balls){
  for(int i = 0; i < 3; i++){
    if(i == 0){
      resetBall(&balls[i], NULL);
    } else {
      resetBall(&balls[i], &balls[i-1]);
    }
  }
}

void tryToUnfreezeBall(Ball *ball){
  const int waitingTime = 2;
  if(getTime(ball->freezedTime) >= waitingTime){
    ball->shouldMove = true;
  }
}

void moveBalls(Ball *balls, int gravity, int currentLevel){
  const int totalBalls = currentLevel == 1 ? 2 : 3;
  const float rotationSpeed = 0.1;
  float completeRound = 2*ALLEGRO_PI;
  for(int i = 0; i < totalBalls; i++){
    if(balls[i].shouldMove){

      balls[i].position.x += balls[i].speed.x;
      balls[i].position.y += balls[i].speed.y;
      balls[i].speed.y -= ((float)gravity/1000);
      balls[i].rotation += rotationSpeed;
      if(balls[i].rotation >= completeRound){
        balls[i].rotation = balls[i].rotation - completeRound;
      }
    } else {
      tryToUnfreezeBall(&balls[i]);
    }
  }
}

bool isTouchingPlataform(Ball ball, Platform platform){
  Axes leftTop, rightBottom, cloneBall;
  int shiftAmount;

  if(platform.level == 1){
    shiftAmount = platform.width*1.5;
  } else {
    shiftAmount = platform.width;
  }
  leftTop.x = platform.position.x - shiftAmount;
  leftTop.y = platform.position.y;

  rightBottom.x = platform.position.x + shiftAmount;
  rightBottom.y = platform.position.y + platform.height;

  cloneBall = ball.position;
  cloneBall.x += ball.radius;//Test right
  if(isIntercepting(leftTop, rightBottom, ball.position)){
    return true;
  }

  cloneBall = ball.position;
  cloneBall.x -= ball.radius;//Test left
  if(isIntercepting(leftTop, rightBottom, ball.position)){
    return true;
  }

  cloneBall = ball.position;
  cloneBall.y += ball.radius;//Test bottom
  if(isIntercepting(leftTop, rightBottom, ball.position)){
    return true;
  }

  cloneBall = ball.position;//Test center
  if(isIntercepting(leftTop, rightBottom, cloneBall)){
    return true;
  }

  return false;
}

void checkBallsNextAction(Ball *balls, Platform platform, GameConfig *gameConfig, Sounds sounds){
  TouchingWalls touchingWalls;
  const int totalBalls = gameConfig->level == 1 ? 2 : 3;
  for(int i = 0; i < totalBalls; i++){
    if(isTouchingPlataform(balls[i], platform)){
      if(balls[i].shouldMove && balls[i].speed.y > 0){
        playSound(sounds.impactBall);
        handleBallTouchinPlatform(&balls[i], platform.position.y);
        gameConfig->score += 20;//Add score
      }
    }
    touchingWalls = getTouchingWall(balls[i]);
    if(isTouchingWall(touchingWalls)){
      handleBallTouchingWall(touchingWalls, &balls[i]);
      if(touchingWalls.bottom){
        decreaseLife(gameConfig);
        playSound(sounds.damage);
      } else {
        playSound(sounds.impactBall);
      }
    }
  }
}

void handleBallTouchinPlatform(Ball *ball, float platformY){
  if(ball->speed.y > 0){//If Going down
    ball->speed.y = -ball->speed.y;//Throw ball up
    ball->position.y = platformY-10;//Move ball up (not touching platform)
  }
}

TouchingWalls getTouchingWall(Ball ball){
  int ballDiameter = ball.radius*2;
  TouchingWalls touchingWalls = {false, false, false, false};
  Axes cloneBall;

  Axes topWall_topLeft = {0, -ballDiameter};
  Axes topWall_bottomRight = {DISPLAY_WIDTH, 0};

  Axes rightWall_topLeft = {DISPLAY_WIDTH, 0};
  Axes rightWall_bottomRight = {DISPLAY_WIDTH+ballDiameter, DISPLAY_HEIGHT};

  Axes bottomWall_topLeft = {0, DISPLAY_HEIGHT};
  Axes bottomWall_bottomRight = {DISPLAY_WIDTH, DISPLAY_HEIGHT+ballDiameter};

  Axes leftWall_topLeft = {-ballDiameter, 0};
  Axes leftWall_bottomRight = {0, DISPLAY_HEIGHT};

  cloneBall = ball.position;
  cloneBall.y -= ball.radius;//TOP SHIFT
  if(isIntercepting(topWall_topLeft, topWall_bottomRight, cloneBall)){
    touchingWalls.top = true;
  }

  cloneBall = ball.position;
  cloneBall.x += ball.radius;//RIGHT SHIFT
  if(isIntercepting(rightWall_topLeft, rightWall_bottomRight, cloneBall)){
    touchingWalls.right = true;
  }

  cloneBall = ball.position;
  cloneBall.y += ball.radius;//BOTTOM SHIFT
  if(isIntercepting(bottomWall_topLeft, bottomWall_bottomRight, cloneBall)){
    touchingWalls.bottom = true;
  }

  cloneBall = ball.position;
  cloneBall.x -= ball.radius;//LEFT SHIFT
  if(isIntercepting(leftWall_topLeft, leftWall_bottomRight, cloneBall)){
    touchingWalls.left = true;
  }

  return touchingWalls;
}

bool isTouchingWall(TouchingWalls touchingWalls){
  return touchingWalls.top || touchingWalls.right || touchingWalls.bottom || touchingWalls.left;
}

void handleBallTouchingWall(TouchingWalls touchingWalls, Ball *ball){
  if(touchingWalls.top){
    ball->speed.y = -ball->speed.y;
    ball->position.y = ball->radius + 1;
  }

  if(touchingWalls.right || touchingWalls.left){
    ball->speed.x = -ball->speed.x;
    if(touchingWalls.right){
      ball->position.x = DISPLAY_WIDTH - (ball->radius + 1);
    }else{
      ball->position.x = ball->radius + 1;
    }
  }

  if(touchingWalls.bottom){
    resetBall(ball, NULL);
    ball->shouldMove = false;
    ball->freezedTime = al_get_time();
  }
}

void decreaseLife(GameConfig *gameConfig){
  if(gameConfig->life > 0){
    gameConfig->life--;
    if (gameConfig->life == 0){
      gameConfig->gameOver = true;
    }
  }
}

void resetTime(double *time){
  *time = al_get_time();
}

double getTime(double time){
  return al_get_time() - time;
}
