g++ -g -c build/*.cpp
g++ -no-pie *.o -o software-installer -ltgui -lsfml-graphics -lsfml-window -lsfml-system -std=c++11 -pthread
