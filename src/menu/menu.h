#ifndef _menu_h
#define _menu_h
  #include "../utils/util.h"
  #include "../utils/types.h"

  typedef struct{
    MenuButton play;
    MenuButton rank;
    MenuButton exit;
  } MenuHome;

  int menu(AllegroConfig *alConfig, GameConfig *gameConfig, Activity *activity);
  void loadMenuBackground(ALLEGRO_BITMAP **menuBackground);
  void drawMenuBackground(ALLEGRO_BITMAP *menuBackground);
  void loadMenuHome(MenuHome *menuplay);
  void drawMenuHome(MenuHome *menuHome);
  bool isHoverButtonHome(MenuHome menuHome, Axes position);
  int getClickedMenuHome(MenuHome *menuHome, Axes position);
  void handleClickOnMenuHome(Activity *activity, GameConfig *gameConfig, MenuHome *menuHome, Axes position);
#endif
