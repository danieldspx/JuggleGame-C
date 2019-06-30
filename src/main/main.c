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
#include "../menu/menu.h"
#include "../game/game.h"

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define FPS 80

int main(){
  AllegroConfig alConfig = {DISPLAY_WIDTH, DISPLAY_HEIGHT, FPS, NULL, NULL, NULL};
  GameConfig gameConfig;
  Activity activity = {false, false, false};

  if(!initAllegro(&alConfig)){
    return -1;
  }

  initGameConfig(&gameConfig);
  activity.menu = true;

  while(!gameConfig.exit){
    al_show_mouse_cursor(alConfig.display);
    al_ungrab_mouse();
    if(activity.menu){
      menu(&alConfig, &gameConfig, &activity);
    } else  if(activity.game){
      game(&alConfig, &gameConfig, &activity);
    }
  }

  return 0;
}
