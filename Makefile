all: simulation;

Planet.o: src/Planet.cpp src/ui.cpp include/Planet.h include/ui.h
	g++ -std=c++11 -c src/Planet.cpp -lsfml-graphics -lsfml-window -lsfml-system

simulation: simulation.cpp src/ui.cpp Planet.o include/Planet.h include/ui.h
	g++ -std=c++11 -o simulation Planet.o simulation.cpp src/ui.cpp -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system