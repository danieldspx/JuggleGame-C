#ifndef _util_h
#define _util_h
  #include <stdbool.h>
  #include <stdio.h>
  #include <allegro5/allegro.h>
  #include "types.h"

  bool initAllegro(AllegroConfig *alConfig);
  void destroyAllegro(AllegroConfig *alConfig);
  void clearScreen();
  bool isIntercepting(Axes pos1, Axes pos2, Axes point);
  void makeFirstGreater(float *greater, float *lower);
  int getRandomNumber(int maxNum);
  void drawResized(ALLEGRO_BITMAP *bitmap, Axes from, Axes to);
  double absolute(double num);
  void initGameConfig(GameConfig *gameConfig);
  void resetTime(double *time);
  double getTime(double time);
  long int getFileSize(FILE *file);

#endif
