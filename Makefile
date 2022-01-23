all: chess client

chess:
	g++ chess.cpp main.cpp -std=c++2a -o server

client:
	g++ -O3 -Wall client.cpp -o client
