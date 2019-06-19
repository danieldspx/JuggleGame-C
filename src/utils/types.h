#ifndef _types_h
#define _types_h
  #include <allegro5/allegro.h>
  //Type definitions
  typedef struct {
    int width;
    int height;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
  } AllegroConfig;
#endif
