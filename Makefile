GAME=src/game/game.c
UTIL=src/utils/util.c

game: $(GAME) $(UTIL)
	gcc -o bin/game $(GAME) $(UTIL) -I. -lallegro -lallegro_image -lallegro_audio -lallegro_acodec -lallegro_font -lallegro_ttf
