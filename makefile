dir = ./common/

all : main

main : main.cpp $(dir)GL_utilities.c $(dir)LoadTGA.c $(dir)Linux/MicroGlut.c
	gcc -Wall -o main -I$(dir) -I./common/Linux -DGL_GLEXT_PROTOTYPES main.cpp $(dir)GL_utilities.c $(dir)LoadTGA.c $(dir)Linux/MicroGlut.c -lXt -lX11 -lGL -lm -lstdc++

clean: main