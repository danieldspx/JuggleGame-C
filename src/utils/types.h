#ifndef _types_h
#define _types_h
  #include <allegro5/allegro.h>
  #include <allegro5/allegro_font.h>
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
    ALLEGRO_FONT *fontSmall;
    ALLEGRO_FONT *fontMedium;
  } AllegroConfig;

  typedef struct{
    double timeStart;
    int score;
    int level;
    int gravity;
    int life;
    bool gameOver;
    bool pause;
    bool exit;
  } GameConfig;

  typedef struct{
    ALLEGRO_BITMAP *bitmap;
    Axes position;
    Axes endPosition;//The oposite point of the image
  } MenuButton;
#endif
