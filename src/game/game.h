#ifndef _game_h
#define _game_h
  #include <allegro5/allegro.h>
  #include <allegro5/allegro_audio.h>
  #include <allegro5/allegro_acodec.h>
  #include "../utils/util.h"
  #include "../utils/types.h"

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

  typedef struct{
    MenuButton resume;
    MenuButton reset;
    MenuButton home;
  } MenuPaused;

  int game(AllegroConfig *alConfig, GameConfig *gameConfig, Activity *activity);
  void loadMenuPaused(MenuPaused *menuPaused);
  void drawMenuPaused(MenuPaused *menuPaused);
  void drawHelpTextHint(ALLEGRO_FONT *font);
  void loadDialogScore(ALLEGRO_BITMAP **dialogScoreBitmap);
  void drawDialogScore(ALLEGRO_BITMAP *dialogScoreBitmap, Score newScore, ALLEGRO_FONT *font);
  void loadHealthBar(ALLEGRO_BITMAP **healthBitmap);
  void drawHealthBar(ALLEGRO_BITMAP **healthBitmap, GameConfig gameConfig);
  void loadScoreBitmap(ALLEGRO_BITMAP **scoreBitmap);
  void drawScore(ALLEGRO_BITMAP *scoreBitmap, GameConfig gameConfig, ALLEGRO_FONT *font);
  void loadClock(ALLEGRO_BITMAP **clockBitmap);
  void drawClockInfo(ALLEGRO_BITMAP *clockBitmap, GameConfig *gameConfig, ALLEGRO_FONT *font);
  void loadGameBackground(ALLEGRO_BITMAP **backgroundBitmap);
  void loadPauseScreen(ALLEGRO_BITMAP **pauseBitmap);
  void drawPauseScreen(ALLEGRO_BITMAP *pauseBitmap);
  void loadGameOverScreen(ALLEGRO_BITMAP **gameOverBitmap);
  void drawGameOverScreen(ALLEGRO_BITMAP *gameOverBitmap);
  void loadHelpScreen(ALLEGRO_BITMAP **helpBitmap);
  void drawHelpScreen(ALLEGRO_BITMAP *helpBitmap);
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
  void checkBallsNextAction(Ball *balls, Platform platform, GameConfig *gameConfig, Score **scores, Sounds sounds);
  void handleBallTouchinPlatform(Ball *ball, Platform platform);
  TouchingWalls getTouchingWall(Ball ball);
  bool isTouchingWall(TouchingWalls touchingWalls);
  void handleBallTouchingWall(TouchingWalls touchingWalls, Ball *ball);
  void decreaseLife(GameConfig *gameConfig, Score **scores);
  void tryToUnfreezeBall(Ball *ball);
  void togglePauseGame(GameConfig *gameConfig);
  void resetGame(GameConfig *gameConfig, Ball *balls);
  void controllGameLevel(GameConfig *gameConfig, Platform *platform, Ball *balls);
  void increaseBallsSpeed(Ball *balls, int level);
  bool isHoverButton(MenuPaused menuPaused, Axes position);
  int getClickedMenu(MenuPaused *menuPaused, Axes position);
  void handleClickOnMenu(Activity *activity, GameConfig *gameConfig, MenuPaused *menuPaused, Ball *balls, Axes position);
  void placeCarret(char *name, int maxSize, int currentPos);
  void resetNewScore(Score *newScore, int *counter);
  void sanitizeScoreBeforeSave(Score *newScore, GameConfig *gameConfig, int counter);

#endif
