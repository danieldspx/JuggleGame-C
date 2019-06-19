#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "util.h"
#include "types.h"

bool initAllegro(AllegroConfig *alConfig){
  if(!al_init()){
    fprintf(stderr, "Erro ao inicilizar o Allegro\n");
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
  if(!alConfig->event_queue) {
    fprintf(stderr, "Erro ao criar a event_queue!\n");
    destroyAllegro(alConfig);
    return false;
  }

  if(!al_install_mouse()){
    fprintf(stderr, "Erro ao instalar o mouse!\n");
    destroyAllegro(alConfig);
    return false;
  }

  al_register_event_source(alConfig->event_queue, al_get_mouse_event_source());
  al_register_event_source(alConfig->event_queue, al_get_display_event_source(alConfig->display));

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
