simulation: simulation.cpp src/ui.cpp
	g++ -std=c++11 -o simulation simulation.cpp src/ui.cpp -ljsoncpp -lsfml-graphics -lsfml-window -lsfml-system