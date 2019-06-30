#ifndef _scoreboard_h
#define _scoreboard_h
  #include <stdio.h>
  #include "../utils/types.h"

  int scoreboard(AllegroConfig *alConfig, GameConfig *gameConfig, Activity *activity);
  void loadScoreBackground(ALLEGRO_BITMAP **scoreBackgroundBitmap);
  void drawScoreBackground(ALLEGRO_BITMAP *scoreBackgroundBitmap, Score **scores, ALLEGRO_FONT *font);
  FILE *loadScoreFileWrite();
  FILE *loadScoreFileRead();
  Score **allocateScore();
  void errorOnReadScores();
  void readScores(Score **scores);
  int getScorePosition(Score **scores, int points);
  void saveScore(Score **scores, Score newScore);
  void freeScores(Score **scores);

#endif
