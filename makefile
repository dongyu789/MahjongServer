main : main.cpp src/Controller.cpp src/Desk.cpp src/MySocket.cpp src/Mahjong.cpp
	g++ -std=c++11 main.cpp src/Controller.cpp src/Desk.cpp src/MySocket.cpp src/Mahjong.cpp -o main -I headers