#include <allegro5/allegro.h>

typedef struct {
  int width;
  int height;
  ALLEGRO_DISPLAY *display;
  ALLEGRO_EVENT_QUEUE *event_queue;
} AllegroConfig;
