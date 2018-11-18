compile:
	g++ -std=c++11 -o game ./main.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2
pgcompile:
	g++ -std=c++11 -o game ./main.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2 -pg -ggdb
compile-landscape:
	g++ -std=c++11 -o simu ./landscape_gen.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2
pgcompile-landscape:
	g++ -std=c++11 -o simu ./landscape_gen.cpp ./worldmath.cpp ./graphics.cpp ./data_io.cpp -lm -lSDL2 -pg -ggdb
run-landscape:
	./simu
run:
	./game
crun: compile
	make run
crun-landscape: compile-landscape
	make run-landscape
profile: pgcompile
	make run