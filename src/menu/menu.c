#include "menu.h"
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

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define FPS 80

#define MENU_PLAY 1
#define MENU_RANK 2
#define MENU_EXIT 3

int menu(AllegroConfig *alConfig, GameConfig *gameConfig, Activity *activity){
  MenuHome menuHome;
  Axes mousePosition;
  ALLEGRO_EVENT event;
  ALLEGRO_BITMAP *menuBackgroundBitmap;
  ALLEGRO_KEYBOARD_STATE keyboardState;
  bool shouldRedraw = false;
  bool mouseBottomUp = false;

  loadMenuBackground(&menuBackgroundBitmap);
  loadMenuHome(&menuHome);

  while(!gameConfig->exit && activity->menu){
    al_wait_for_event(alConfig->event_queue, &event);
    if(event.type == ALLEGRO_EVENT_MOUSE_AXES){
      mousePosition.x = event.mouse.x;
      mousePosition.y = event.mouse.y;
    } else if(event.type == ALLEGRO_EVENT_TIMER){
      shouldRedraw = true;
    } else if(event.type == ALLEGRO_EVENT_KEY_DOWN){
      al_get_keyboard_state(&keyboardState);
      if(event.keyboard.keycode == ALLEGRO_KEY_Q){//Quit game
        gameConfig->exit = true;
      }
    } else if(event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
      mouseBottomUp = true;
    } else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
      gameConfig->exit = true;
    }

    if(shouldRedraw && al_is_event_queue_empty(alConfig->event_queue)){
      if(mouseBottomUp){//Has clicked
        mouseBottomUp = false;
        handleClickOnMenuHome(activity, gameConfig, &menuHome, mousePosition);
      }

      if(isHoverButtonHome(menuHome, mousePosition)){//Change the cursor when hovering buttons
        al_set_system_mouse_cursor(alConfig->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      } else {
        al_set_system_mouse_cursor(alConfig->display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
      }

      drawMenuBackground(menuBackgroundBitmap);
      drawMenuHome(&menuHome);
      al_flip_display();
    }
  }

  return 0;
}

void loadMenuBackground(ALLEGRO_BITMAP **menuBackground){
  const char *menuBackgroundPath = "assets/menu/background.png";
  *menuBackground = al_load_bitmap(menuBackgroundPath);
}

void drawMenuBackground(ALLEGRO_BITMAP *menuBackground){
  Axes from = {0, 0};
  Axes to = {DISPLAY_WIDTH, DISPLAY_HEIGHT};
  drawResized(menuBackground, from, to);
}

void loadMenuHome(MenuHome *menuHome){
  int margin = 10, heightButton, widthButton, posX, posY;
  const char *playPath = "assets/menu/buttons/playButton.png";
  const char *rankPath = "assets/menu/buttons/rankButton.png";
  const char *exitPath = "assets/menu/buttons/exitButton.png";

  menuHome->play.bitmap = al_load_bitmap(playPath);
  menuHome->rank.bitmap = al_load_bitmap(rankPath);
  menuHome->exit.bitmap = al_load_bitmap(exitPath);

  widthButton = al_get_bitmap_width(menuHome->play.bitmap);
  heightButton = al_get_bitmap_height(menuHome->play.bitmap);
  posX = (DISPLAY_WIDTH / 2) - (widthButton / 2);
  posY = 250;

  menuHome->play.position.x = posX;
  menuHome->play.position.y = posY;
  menuHome->play.endPosition.x = posX + widthButton;
  menuHome->play.endPosition.y = menuHome->play.position.y + heightButton;

  menuHome->rank.position.x = posX;
  menuHome->rank.position.y = menuHome->play.endPosition.y + margin;
  menuHome->rank.endPosition.x = posX + widthButton;
  menuHome->rank.endPosition.y = menuHome->rank.position.y + heightButton;

  menuHome->exit.position.x = posX;
  menuHome->exit.position.y = menuHome->rank.endPosition.y + margin;
  menuHome->exit.endPosition.x = posX + widthButton;
  menuHome->exit.endPosition.y = menuHome->exit.position.y + heightButton;
}

void drawMenuHome(MenuHome *menuHome){
  al_draw_bitmap(menuHome->play.bitmap, menuHome->play.position.x, menuHome->play.position.y, 0);
  al_draw_bitmap(menuHome->rank.bitmap, menuHome->rank.position.x, menuHome->rank.position.y, 0);
  al_draw_bitmap(menuHome->exit.bitmap, menuHome->exit.position.x, menuHome->exit.position.y, 0);
}

bool isHoverButtonHome(MenuHome menuHome, Axes position){
  return isIntercepting(menuHome.play.position, menuHome.play.endPosition, position)
        || isIntercepting(menuHome.rank.position, menuHome.rank.endPosition, position)
        || isIntercepting(menuHome.exit.position, menuHome.exit.endPosition, position);
}

int getClickedMenuHome(MenuHome *menuHome, Axes position){
  if(isIntercepting(menuHome->play.position, menuHome->play.endPosition, position)){
    return MENU_PLAY;
  } else if (isIntercepting(menuHome->rank.position, menuHome->rank.endPosition, position)){
    return MENU_RANK;
  } else if(isIntercepting(menuHome->exit.position, menuHome->exit.endPosition, position)){
    return MENU_EXIT;
  } else {
    return -1;//None
  }
}

void handleClickOnMenuHome(Activity *activity, GameConfig *gameConfig, MenuHome *menuHome, Axes position){
  switch (getClickedMenuHome(menuHome, position)) {
    case MENU_PLAY:
      activity->menu = false;
      activity->game = true;
      break;
    case MENU_RANK:
      //GO TO RANK
      break;
    case MENU_EXIT:
      activity->menu = false;
      gameConfig->exit = true;
  }
}
