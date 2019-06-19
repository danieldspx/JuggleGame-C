GAME=src/game/game.c
TYPES=src/utils/types.c
UTIL=src/utils/util.c

game: $(GAME) $(TYPES) $(UTIL)
	gcc -o bin/game $(GAME) $(TYPES) $(UTIL) -I. -lallegro -lallegro_image
