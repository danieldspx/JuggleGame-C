#include <stdio.h>
#include <stdlib.h>
#include "../utils/util.h"

#define SCORE_SIZE 10

FILE *loadScoreFile(){
  const char *filePath = "data/juggler.score";
  FILE *scoreFile = fopen(filePath, "w+b");
  return scoreFile;
}

Score **allocateScore(){
  return (Score **) malloc(SCORE_SIZE*sizeof(Score *));
}

void errorOnReadScores(){
  fprintf(stderr, "Erro ao ler scores: ponteiro de ponteiro nulo\n");
}

void readScores(FILE *scoreFile, Score **scores){
  if(scores == NULL){//No memory allocated
    errorOnReadScores();
    return;
  }

  if(getFileSize(scoreFile) != 0){
    fread(scores, sizeof(Score), SCORE_SIZE, scoreFile);
  } else {
    for(int i = 0; i < SCORE_SIZE; i++){
      scores[i] = NULL;
    }
  }
}

int getScorePosition(Score **scores, int points){
  if(scores == NULL){
    errorOnReadScores();
    return -1;
  }

  for(int i = 0; i < SCORE_SIZE; i++){
    if(scores[i] == NULL || points >= scores[i]->points){
      return i;
    }
  }

  return -1;
}

void saveScore(Score **scores, Score newScore){
  FILE *scoreFile= loadScoreFile();
  int savePosition = getScorePosition(scores, newScore.points);
  if(savePosition != -1){
    if(scores[savePosition] == NULL){
      scores[savePosition] = (Score *) malloc(sizeof(Score));
    } else {
      for(int i = (SCORE_SIZE - 1); i > savePosition; i--){
        *scores[i] = *scores[i - 1];
      }
    }

    *scores[savePosition] = newScore;
  }

  fwrite(scores, sizeof(Score *), SCORE_SIZE, scoreFile);
}
