te: src/main.c
	cc -o te src/main.c src/grid.c src/renderer.c src/engine.c -I/usr/include/lua5.4 -lraylib -llua5.4
