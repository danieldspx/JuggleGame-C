#ifndef _types_h
#define _types_h
  #include <allegro5/allegro.h>
  #include <allegro5/allegro_font.h>
  //Shared types
  typedef struct{
    float x;
    float y;
  } Axes;

  typedef struct{
    bool game;
    bool menu;
    bool rank;
  } Activity;

  typedef struct {
    int width;
    int height;
    int fps;
    ALLEGRO_DISPLAY *display;
    ALLEGRO_EVENT_QUEUE *event_queue;
    ALLEGRO_TIMER *timer;
    ALLEGRO_FONT *fontSmall;
    ALLEGRO_FONT *fontMedium;
    ALLEGRO_FONT *fontBig;
  } AllegroConfig;

  typedef struct{
    double timeStart;
    double timePassed;
    int score;
    int level;
    int gravity;
    int life;
    bool gameOver;
    bool pause;
    bool exit;
    bool hasScore;
  } GameConfig;

  typedef struct{
    ALLEGRO_BITMAP *bitmap;
    Axes position;
    Axes endPosition;//The oposite point of the image
  } MenuButton;

  typedef struct{
    char name[20];
    int points;
  } Score;
#endif
