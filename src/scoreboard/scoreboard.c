#include "scoreboard.h"
#include <stdio.h>
#include <stdlib.h>
#include "../utils/types.h"
#include "../utils/util.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600

#define SCORE_SIZE 10

int scoreboard(AllegroConfig *alConfig, GameConfig *gameConfig, Activity *activity){
  ALLEGRO_EVENT event;
  ALLEGRO_BITMAP *scoreBackgroundBitmap;
  ALLEGRO_BITMAP *backButtonBitmap;
  Axes mousePosition;
  Score **scores = allocateScore();
  readScores(scores);
  bool shouldRedraw = false;

  loadScoreBackground(&scoreBackgroundBitmap);
  loadBackButton(&backButtonBitmap);

  int imageSize = al_get_bitmap_width(backButtonBitmap);
  int marginButton = 30;
  Axes positionBackButton = {DISPLAY_WIDTH - (imageSize+marginButton), DISPLAY_HEIGHT - (imageSize+marginButton)};
  Axes positionEndBackButton = {DISPLAY_WIDTH - marginButton, DISPLAY_HEIGHT - marginButton};

  while (!gameConfig->exit && activity->rank) {
    al_wait_for_event(alConfig->event_queue, &event);
    if(event.type == ALLEGRO_EVENT_MOUSE_AXES){
      mousePosition.x = event.mouse.x;
      mousePosition.y = event.mouse.y;
    } else if(event.type == ALLEGRO_EVENT_TIMER){
      shouldRedraw = true;
    } else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
      if(isIntercepting(positionBackButton, positionEndBackButton, mousePosition)){
        activity->rank = false;
        activity->menu = true;
      }
    } else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      gameConfig->exit = true;
    }

    // printf("(%f %f) (%f %f) (%f %f)\n", positionBackButton.x, positionBackButton.y, positionEndBackButton.x, positionEndBackButton.y, mousePosition.x, mousePosition.y);

    if(isIntercepting(positionBackButton, positionEndBackButton, mousePosition)){  // Change the cursor when hovering buttons
        al_set_system_mouse_cursor(alConfig->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    } else {
        al_set_system_mouse_cursor(alConfig->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }

    if(shouldRedraw && al_is_event_queue_empty(alConfig->event_queue)){
      drawScoreBackground(scoreBackgroundBitmap, scores, alConfig->fontBig);
      drawBackButton(backButtonBitmap, positionBackButton);
      al_flip_display();
    }
  }

  freeScores(scores);

  return 0;
}

void loadScoreBackground(ALLEGRO_BITMAP **scoreBackgroundBitmap){
  const char *scorePath = "assets/scoreboard/background.png";
  *scoreBackgroundBitmap = al_load_bitmap(scorePath);
}

void drawScoreBackground(ALLEGRO_BITMAP *scoreBackgroundBitmap, Score **scores, ALLEGRO_FONT *font){
  Axes from = {0, 0};
  Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
  Axes textPosition = {140, 150};
  int marginY = 40;
  char textScore[50];
  drawResized(scoreBackgroundBitmap, from, to);
  for(int i = 0; i < SCORE_SIZE; i++){
    if(scores[i] == NULL){
      if(i == 0){
        al_draw_text(font, al_map_rgb(255, 255, 255), textPosition.x, textPosition.y, 0, "Nenhum recorde ainda :(");
      }
      break;
    }
    sprintf(textScore, "#%-2d %-20s %10d", i+1, scores[i]->name, scores[i]->points);
    al_draw_text(font, al_map_rgb(255, 255, 255), textPosition.x, textPosition.y + (marginY*i), 0, textScore);
  }
}

void loadBackButton(ALLEGRO_BITMAP **backButtonBitmap){
  const char *scorePath = "assets/scoreboard/next-page.png";
  *backButtonBitmap = al_load_bitmap(scorePath);
}

void drawBackButton(ALLEGRO_BITMAP *backButtonBitmap, Axes position){
  al_draw_bitmap(backButtonBitmap, position.x, position.y, 0);
}

FILE *loadScoreFileWrite(){
  const char *filePath = "data/juggler.score";
  FILE *scoreFile = fopen(filePath, "w+b");
  return scoreFile;
}

FILE *loadScoreFileRead(){
  const char *filePath = "data/juggler.score";
  FILE *scoreFile = fopen(filePath, "r+b");
  if(scoreFile == NULL){
    fclose(loadScoreFileWrite());//Creates the file
  }
  scoreFile = fopen(filePath, "r+b");
  return scoreFile;
}

Score **allocateScore(){
  return (Score **) malloc(SCORE_SIZE*sizeof(Score *));
}

void errorOnReadScores(){
  fprintf(stderr, "Erro ao ler scores: ponteiro de ponteiro nulo\n");
}

void readScores(Score **scores){
  FILE *scoreFile = loadScoreFileRead();
  int totalScores = 0;

  if(scores == NULL){//No memory allocated
    errorOnReadScores();
    return;
  }

  if(getFileSize(scoreFile) != 0){
    fread(&totalScores, sizeof(int), 1, scoreFile);

    for(int i = 0; i < totalScores; i++){
      scores[i] = (Score *) malloc(sizeof(Score));
      fread(scores[i], sizeof(Score), 1, scoreFile);
    }

    for(int i = totalScores; i < SCORE_SIZE; i++){
      scores[i] = NULL;
    }
  } else {
    for(int i = 0; i < SCORE_SIZE; i++){
      scores[i] = NULL;
    }
  }

  fclose(scoreFile);
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

int getTotalScores(Score **scores){
  int count = 0;
  for(int i = 0; i < SCORE_SIZE; i++){
    count++;
    if(scores[i] == NULL){
      break;
    }
  }
  return count;
}

void saveScore(Score **scores, Score newScore){
  FILE *scoreFile= loadScoreFileWrite();
  int savePosition = getScorePosition(scores, newScore.points);
  int totalScores = getTotalScores(scores);
  if(savePosition != -1){
    if(scores[savePosition] == NULL){
      scores[savePosition] = (Score *) malloc(sizeof(Score));
    } else {
      for(int i = (SCORE_SIZE - 1); i > savePosition; i--){
        if(scores[i - 1] != NULL){
          scores[i] = scores[i] == NULL ? (Score *) malloc(sizeof(Score)) : scores[i];
          *scores[i] = *scores[i - 1];
        }
      }
    }

    *scores[savePosition] = newScore;
  }

  fwrite(&totalScores, sizeof(int), 1, scoreFile);
  for(int i = 0; i < totalScores; i++){
    fwrite(scores[i], sizeof(Score), 1, scoreFile);
  }

  fclose(scoreFile);
}

void freeScores(Score **scores){
  for(int i = 0; i < SCORE_SIZE; i++){
    free(scores[i]);
  }

  free(scores);
}
