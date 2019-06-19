#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "../utils/util.h"
#include "../utils/types.h"

#define DISPLAY_WIDTH 600
#define DISPLAY_HEIGHT 600

typedef struct{
  int sizeBitmap;
  int position;//Leftmost side
  int level;//Max is 2
  ALLEGRO_BITMAP *bitmap;
  ALLEGRO_BITMAP *bitmapLeft;
  ALLEGRO_BITMAP *bitmapMid;
  ALLEGRO_BITMAP *bitmapRight;
} Platform;

void loadGameBackground(ALLEGRO_BITMAP **backgroundBitmap);
void drawBackground(ALLEGRO_BITMAP *backgroundBitmap);
void loadPlataform(Platform *plataform);
void drawPlataform(Platform plataform);

int main(){
  AllegroConfig alConfig = {DISPLAY_WIDTH, DISPLAY_HEIGHT, NULL, NULL};
  ALLEGRO_BITMAP *background = NULL;
  ALLEGRO_EVENT event;
  Platform plataform;

  bool hasEvent;

  if(!initAllegro(&alConfig)){
    return -1;
  }

  loadGameBackground(&background);
  loadPlataform(&plataform);

  while (true) {//Main loop
    hasEvent = al_wait_for_event_timed(alConfig.event_queue, &event, 0.1);
    if(hasEvent){
      if(event.type == ALLEGRO_EVENT_MOUSE_AXES){
        plataform.position = event.mouse.x;
      }else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
        break;
      }
    }

    drawBackground(background);
    drawPlataform(plataform);

    al_flip_display();
  }

  destroyAllegro(&alConfig);
}

void loadGameBackground(ALLEGRO_BITMAP **backgroundBitmap){
  const char *background = "assets/game/background.png";
  *backgroundBitmap = al_load_bitmap(background);
}

void drawBackground(ALLEGRO_BITMAP *backgroundBitmap){
  al_draw_bitmap(backgroundBitmap, 0, 0, 0);
}

void loadPlataform(Platform *platform){
  const int sizeBitmap = 70;//The image size

  const char *bitmapLeft = "assets/game/platformLeft.png";
  const char *bitmapMid = "assets/game/platformMid.png";
  const char *bitmapRight = "assets/game/platformRight.png";

  platform->level = 1;//Max is 2
  platform->sizeBitmap = sizeBitmap;
  platform->position = DISPLAY_WIDTH / 2;
  platform->bitmapLeft = al_load_bitmap(bitmapLeft);
  platform->bitmapMid = al_load_bitmap(bitmapMid);
  platform->bitmapRight = al_load_bitmap(bitmapRight);
}

void drawPlataform(Platform platform){
  const int elevation = DISPLAY_HEIGHT - 50;
  int drawCounter = 0;
  int positionFraction;//The fraction to calc the right position

  if(platform.level == 1){
    positionFraction = 1.5*platform.sizeBitmap;
  } else {
    positionFraction = platform.sizeBitmap;
  }

  al_draw_bitmap(platform.bitmapLeft, platform.position - positionFraction, elevation, 0);

  if(platform.level == 1){
    drawCounter++;
    al_draw_bitmap(platform.bitmapMid, platform.position - 0.5*platform.sizeBitmap, elevation, 0);
  }

  drawCounter++;
  if(platform.level == 1){
    al_draw_bitmap(platform.bitmapRight, platform.position + 0.5*platform.sizeBitmap, elevation, 0);
  } else {
    al_draw_bitmap(platform.bitmapRight, platform.position, elevation, 0);
  }
}
