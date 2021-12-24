default: build run

debug:
	g++ -g src/*.cpp -lSDL2_image -lSDL2 -lGL -lm -o alton
	gdb

build:
	g++ -O3 src/*.cpp -lSDL2_image -lSDL2 -lGL -lm -o alton

run:
	./alton
