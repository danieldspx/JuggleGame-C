#ifndef _util_h
#define _util_h
  #include <stdbool.h>
  #include <allegro5/allegro.h>
  #include "types.h"

  bool initAllegro(AllegroConfig *alConfig);
  void destroyAllegro(AllegroConfig *alConfig);
  void clearScreen();
  bool isIntercepting(Axes pos1, Axes pos2, Axes point);
  void makeFirstGreater(float *greater, float *lower);
  int getRandomNumber(int maxNum);

#endif
