GAME=src/game/game.c
MENU=src/menu/menu.c
MAIN=src/main/main.c
UTIL=src/utils/util.c

#game: $(GAME) $(UTIL)
#	gcc -o bin/game $(GAME) $(UTIL) -I. -lallegro -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf

main: $(MAIN) $(MENU) $(GAME) $(UTIL)
	gcc -o bin/main $(MAIN) $(MENU) $(GAME) $(UTIL) -I. -lallegro -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf
