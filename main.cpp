#include <iostream>
#include "chess.h"

int main(int argc, char **argv){
  if (argc != 2){
    std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
    return EXIT_FAILURE;
  }
  P2P_chess::Board b;
  b.startGame(atoi(argv[1]));
  return EXIT_FAILURE;
}
