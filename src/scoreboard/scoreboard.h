#ifndef _scoreboard_h
#define _scoreboard_h
  #include <stdio.h>
  #include "../utils/types.h"

  FILE *loadScoreFile();
  Score **allocateScore();
  void errorOnReadScores();
  void readScores(FILE *scoreFile, Score **scores);
  int getScorePosition(Score **scores, int points);
  void saveScore(Score **scores, Score newScore);

#endif
