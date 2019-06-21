#ifndef _types_h
#define _types_h
  #include <allegro5/allegro.h>
  //Shared types
  typedef struct{
    float x;
    float y;
  } Axes;

  typedef struct {
    int width;
    int height;
    int fps;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *timer;
  } AllegroConfig;

  typedef struct{
    int score;
    int level;
    int gravity;
    int life;
    bool gameOver;
    bool pause;
  } GameConfig;
#endif
