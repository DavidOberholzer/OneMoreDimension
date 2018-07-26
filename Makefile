compile:
	g++ -std=c++11 -o game ./main.cpp ./worldmath.cpp ./graphics.cpp -lm -lSDL2
run:
	./game