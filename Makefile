compile:
	g++ -std=c++11 -o game ./main.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2
run:
	./game
crun: compile
	make run