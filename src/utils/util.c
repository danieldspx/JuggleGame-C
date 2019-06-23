#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "util.h"
#include "types.h"

bool initAllegro(AllegroConfig *alConfig){
  srand(time(0));

  al_init_font_addon();
  al_init_ttf_addon();

  if(!al_init()){
    fprintf(stderr, "Erro ao inicilizar o Allegro\n");
    return false;
  }

  const char *robotoFont = "assets/fonts/RobotoMono-Bold.ttf";
  alConfig->fontSmall = al_load_ttf_font(robotoFont, 12, 0);
  alConfig->fontMedium = al_load_ttf_font(robotoFont, 15, 0);

  if(!alConfig->fontSmall || !alConfig->fontMedium){
    fprintf(stderr, "Erro ao carregar a fonte\n");
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

  if(!al_install_audio()){
      fprintf(stderr, "Erro ao inicializar audio\n");
      destroyAllegro(alConfig);
      return false;
   }

   if(!al_init_acodec_addon()){
      fprintf(stderr, "Erro ao inicializar audio codecs\n");
      destroyAllegro(alConfig);
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

  if(!al_install_keyboard()){
    fprintf(stderr, "Erro ao instalar o teclado\n");
    destroyAllegro(alConfig);
    return false;
  }

  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MIPMAP | ALLEGRO_MAG_LINEAR);//Make rotations smooth

  al_register_event_source(alConfig->event_queue, al_get_keyboard_event_source());
  al_register_event_source(alConfig->event_queue, al_get_mouse_event_source());
  al_register_event_source(alConfig->event_queue, al_get_display_event_source(alConfig->display));
  al_register_event_source(alConfig->event_queue, al_get_timer_event_source(alConfig->timer));
  al_start_timer(alConfig->timer);
  return true;
}

void destroyAllegro(AllegroConfig *alConfig){
  al_destroy_font(alConfig->fontSmall);
  al_destroy_timer(alConfig->timer);
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

void drawResized(ALLEGRO_BITMAP *bitmap, Axes from, Axes to){
  int sourceW = al_get_bitmap_width(bitmap);
  int sourceH = al_get_bitmap_height(bitmap);
  al_draw_scaled_bitmap(bitmap, from.x, from.y, sourceW, sourceH, from.x, from.x, to.x, to.y, 0);
}
