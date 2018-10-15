compile:
	g++ -std=c++11 -o game ./main.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2
remove:
	rm -rf test
test: remove
	g++ -std=c++11 -o test ./test.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2
	./test
run:
	./game
crun: compile
	make run