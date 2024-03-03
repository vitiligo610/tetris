compile:./main.cpp
	g++ -c ./main.cpp ./functionality.h ./utils.h ./pieces.h
	g++ main.o -o main -lsfml-graphics -lsfml-window -lsfml-system

run:
	./main
