default: build run

debug:
	g++ -g src/*.cpp -lSDL2_image -lSDL2 -lGL -lm -lGLEW -o alton
	gdb alton

build:
	g++ -O3 src/*.cpp -lSDL2_image -lSDL2 -lGL -lm -lGLEW -o alton

map:
	python editor/main.py assets/tiledict.json assets/map_alton.json assets/map.map

win_x64:
	build.bat
	bin/win_x64/alton.exe

run:
	./alton
