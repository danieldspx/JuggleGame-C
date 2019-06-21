#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "util.h"
#include "types.h"

bool initAllegro(AllegroConfig *alConfig){
  srand(time(0));

  if(!al_init()){
    fprintf(stderr, "Erro ao inicilizar o Allegro\n");
    return false;
  }

  alConfig->timer = al_create_timer(1.0 / alConfig->fps);
   if(!alConfig->timer) {
      fprintf(stderr, "Erro ao criar o timer\n");
      return false;
   }


  alConfig->display = al_create_display(alConfig->width, alConfig->height);

  if(!alConfig->display){
    fprintf(stderr, "Erro ao criar o display\n");
    return false;
  }

  if(!al_init_image_addon()){
    fprintf(stderr, "Erro ao iniciar o image_addon\n");
    destroyAllegro(alConfig);
  }

  alConfig->event_queue = al_create_event_queue();
  if(!alConfig->event_queue){
    fprintf(stderr, "Erro ao criar a event_queue\n");
    destroyAllegro(alConfig);
    return false;
  }

  if(!al_install_mouse()){
    fprintf(stderr, "Erro ao instalar o mouse\n");
    destroyAllegro(alConfig);
    return false;
  }

  al_register_event_source(alConfig->event_queue, al_get_mouse_event_source());
  al_register_event_source(alConfig->event_queue, al_get_display_event_source(alConfig->display));
  al_register_event_source(alConfig->event_queue, al_get_timer_event_source(alConfig->timer));
  al_start_timer(alConfig->timer);
  return true;
}

void destroyAllegro(AllegroConfig *alConfig){
  al_destroy_display(alConfig->display);
  al_destroy_event_queue(alConfig->event_queue);
}

void clearScreen(){
  al_clear_to_color(al_map_rgb(0,0,0));
  al_flip_display();
}

bool isIntercepting(Axes pos1, Axes pos2, Axes point){
  float greaterX, lowerX, greaterY, lowerY;
  greaterX = pos1.x;
  greaterY = pos1.y;
  lowerX = pos2.x;
  lowerY = pos2.y;
  makeFirstGreater(&greaterX, &lowerX);
  makeFirstGreater(&greaterY, &lowerY);
  // greaterX = pos2.x;
  // greaterY = pos2.y;
  // lowerX = pos1.x;
  // lowerY = pos1.y;
  if(lowerX <= point.x && point.x <= greaterX){
    if(lowerY <= point.y && point.y <= greaterY){
      return true;
    }
  }
  return false;
}

void makeFirstGreater(float *greater, float *lower){
  float temp = *greater;
  if(greater < lower){
    *greater = *lower;
    *lower = temp;
  }
}

int getRandomNumber(int maxNum){
  return ((rand() % maxNum)+1);
}
