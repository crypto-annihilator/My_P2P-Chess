#include <iostream>
#include <cmath>
#include <vector>
#include "chess.h"


static unsigned int cli_count = 0;
static int uid = 1;
const char *ip = "127.0.0.1";

P2P_chess::Piece::~Piece() {
  delete m_piece;
  m_piece = nullptr;
}

void P2P_chess::Piece::setPieceColorAndSymbol(Piece* pic,
    PieceType type, Color color, const std::string& symbol) {
  m_type = type;
  m_color = color;
  m_symbol = symbol;
  m_piece = pic;
  m_moved = false;
}

P2P_chess::PieceType P2P_chess::Piece::getPieceType() const {
  return m_type;
}


P2P_chess::Color P2P_chess::Piece::getColor() const {
  return m_color;
}


std::string P2P_chess::Piece::getSymbol() const {
  return m_symbol;
}

P2P_chess::Piece* P2P_chess::Piece::getPiece() const {
  return m_piece;
}

bool P2P_chess::Piece::getState() const {
  return m_moved;
}

P2P_chess::Piece& P2P_chess::Piece::operator=(Piece&& rhs) {
  m_type = rhs.m_type;
  m_color = rhs.m_color;
  m_symbol = rhs.m_symbol;
  m_piece = rhs.m_piece;
  m_moved = true;
  rhs.m_type = PieceType::None;
  rhs.m_color = Color::None;
  rhs.m_moved = false;
  rhs.m_symbol = "   |";
  rhs.m_piece = nullptr;
  return *this;
}

void P2P_chess::Piece::removePiece(Piece**& board) {
  m_type = PieceType::None;
  m_color = Color::None;
  m_symbol = "   |";
  m_piece = nullptr;
}

void P2P_chess::Piece::changePieceState() {
  m_moved = true;
}

void P2P_chess::Piece::changePawnToQueen() {
  m_type = PieceType::Queen;
  m_piece = new Queen();
  if (m_color == Color::White) {
    m_symbol = " \u2655 |";
  } else {
    m_symbol = " \u265B |";
  }
}

bool P2P_chess::Piece::isOpenCheckForKingVertical(Piece**& board, int x, int y, int x1, int y1) {
  if (x == 0 || x == 7) return true; 
  OpenCheckKing checkingPiece = OpenCheckKing::None;
  OpenCheckKing current = OpenCheckKing::None;
  Color color;
  if (x1 == -1) {
    color = board[y][x].getColor();
  } else {
    color = board[y1][x1].getColor();
  } 
  for (int i = x + 1; i < 8; ++i) {
    if (i == x1) continue; 
    current = kingOrQueenOrRookOrBishop(board, color, PieceType::Rook, x, y, i, y);
    if (current == OpenCheckKing::Other) break ;
    if (current != OpenCheckKing::None) {
      checkingPiece = current;		
      break;
    }
  }
  if (current != OpenCheckKing::Other && checkingPiece != OpenCheckKing::None) {
    for (int i = x - 1; i >= 0; --i) {
      if (i == x1) continue; 
      current = kingOrQueenOrRookOrBishop(board, color, PieceType::Rook, x, y, i, y);
      if (current == OpenCheckKing::Other) {
        checkingPiece = OpenCheckKing::None;
        break;
      }
      if (current != OpenCheckKing::None) {
        if (checkingPiece != current) return false;		
        break;
      }	
    }
  }
  for (int i = y + 1; i < 8; ++i) { 
    if (i == y1) continue; 
    current = kingOrQueenOrRookOrBishop(board, color, PieceType::Rook, x, y, x, i);
    if (current == OpenCheckKing::Other) break;
    if (current != OpenCheckKing::None) {
      checkingPiece = current;		
      break;
    }
  }
  if (current != OpenCheckKing::Other && checkingPiece != OpenCheckKing::None) {
    for (int i = y - 1; i >= 0; --i) {
      if (i == y1) continue; 
      current = kingOrQueenOrRookOrBishop(board, color, PieceType::Rook, x, y, x, i);
      if (current == OpenCheckKing::Other) return true;
      if (current != OpenCheckKing::None) {
        if (checkingPiece != current) return false;		
        break;
      }	
    }
  }
  return true;
}

bool P2P_chess::Piece::isOpenCheckForKingDiagonal(Piece**& board, int x, int y, int x1, int y1) {
  OpenCheckKing checkingPiece = OpenCheckKing::None;
  OpenCheckKing current = OpenCheckKing::None;
  Color color;
  if (x1 == -1) {
    color = board[y][x].getColor();
  } else {
    color = board[y1][x1].getColor();
  }
  for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j) {
    if (i == x1 && j == y1) continue; 
    current = kingOrQueenOrRookOrBishop(board, color, PieceType::Bishop, x, y, i, j);
    if (current == OpenCheckKing::Other) break;
    if (current != OpenCheckKing::None) {
      checkingPiece = current;		
      break;
    }
  }
  if (current != OpenCheckKing::Other && checkingPiece != OpenCheckKing::None) {
    for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
      if (i == x1 && j == y1) continue; 
      current = kingOrQueenOrRookOrBishop(board, color, PieceType::Bishop, x, y, i, y);
      if (current == OpenCheckKing::Other) {
        checkingPiece = OpenCheckKing::None;
        break;
      }
      if (current != OpenCheckKing::None) {
        if (checkingPiece != current) return false;		
        break;
      }
    }
  }
  for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j) {
    if (i == x1 && j == y1) continue; 
    current = kingOrQueenOrRookOrBishop(board, color, PieceType::Bishop, x, y, i, j);
    if (current == OpenCheckKing::Other) break;
    if (current != OpenCheckKing::None) {
      checkingPiece = current;		
      break;
    }
  }
  if (current != OpenCheckKing::Other && checkingPiece != OpenCheckKing::None) {
    for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j) {
      if (i == x1 && j == y1) continue; 
      current = kingOrQueenOrRookOrBishop(board, color, PieceType::Bishop, x, y, i, j);
      if (current == OpenCheckKing::Other) {
        return true;
      }
      if (current != OpenCheckKing::None) {
        if (checkingPiece != current) return false;		
        break;
      }
    }
  }
  return true;
}

P2P_chess::OpenCheckKing P2P_chess::Piece::kingOrQueenOrRookOrBishop(Piece**& board,
                                                                        Color color,
                                                                        PieceType rookOrBishop,
                                                                        int x, int y, int x1, int y1) {
  if (board[y1][x1].getPieceType() == PieceType::King
      && board[y1][x1].getColor() == color) {
    return OpenCheckKing::King;
  } else if ((board[y1][x1].getPieceType() == PieceType::Queen
        || board[y1][x1].getPieceType() == rookOrBishop)
      && board[y1][x1].getColor() != color) {
    return OpenCheckKing::QueenOrRookOrBishop;
  } else if (board[y1][x1].getPieceType() != PieceType::None) {
    return OpenCheckKing::Other;
  }
  return OpenCheckKing::None;
}

bool P2P_chess::Piece::openCheckOnHimselfCastlingTime(Piece**& board, int xK, int xR, int y) {
  if (xK > xR) {
    return openCheckOnHimself(board, xK - 2, y, board[y][xK].getColor());
  }
  return openCheckOnHimself(board, xK + 2, y, board[y][xK].getColor());
}

bool P2P_chess::Piece::openCheckOnHimself(Piece**& board, int x, int y, Color color) {
  if (isTherePawnWithDiagonal(board, x, y, color)) { return false; }
  if (isThereRivalKnight(board, x, y, color))	{ return false; }
  if (isThereQueenOrRookWithVertically(board, x, y, color)) { return false; }
  if (isThereQueenOrBishopWithDiagonal(board, x, y, color)) { return false; }
  if (isThereRivalKing(board, x, y, color)) { return false; }
  return true;
}

bool P2P_chess::Piece::isTherePawnWithDiagonal(Piece**& board, int x, int y, Color color) {
  if (color == Color::White) {
    if (x != 7 && y != 7 
        && (board[y + 1][x + 1].getPieceType() == PieceType::Pawn)
        && (board[y + 1][x + 1].getColor() != color)) { return true; }
    if (x != 0 && y != 7
        && (board[y + 1][x - 1].getPieceType() == PieceType::Pawn)
        && (board[y + 1][x - 1].getColor() != color)) { return true; }   
  } else {
    if (x != 7 && y != 0 
        && (board[y - 1][x + 1].getPieceType() == PieceType::Pawn)
        && (board[y - 1][x + 1].getColor() != color)) { return true; }
    if (x != 0 && y != 0
        && (board[y - 1][x - 1].getPieceType() == PieceType::Pawn)
        && (board[y - 1][x - 1].getColor() != color)) { return true; } 
  }
  return false;
}

void P2P_chess::Piece::getCoordinatePiece(Piece**& board, PieceType type, Color color, std::vector<int>& vec) {
  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (board[i][j].getPieceType() == type && board[i][j].getColor() != color) {
        vec.push_back(i);
        vec.push_back(j);
      }
    }
  }
}

bool P2P_chess::Piece::isThereRivalKnight(Piece**& board, int x, int y, Color color) {
  std::vector<int> vec;
  getCoordinatePiece(board, PieceType::Knight, color, vec);
  for (int i = 0; i < vec.size(); ++i) {
    if (board[vec[i]][vec[i+1]].isValidStep(board, vec[i + 1], vec[i], x , y)) { return true; }
  }
  return false;	
}

bool P2P_chess::Piece::isThereQueenOrRookWithVertically(Piece**& board, int x, int y, Color color) {
  for (int i = x + 1; i < 8; ++i) {
    if (board[y][i].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, i, y, PieceType::Rook, color)) { return true; }
    break;
  }

  for (int i = x - 1; i >= 0; --i) {
    if (board[y][i].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, i, y, PieceType::Rook, color)) { return true; }
    break;
  }

  for (int i = y + 1; i < 8; ++i) {
    if (board[i][x].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, x, i, PieceType::Rook, color)) return true;
    break;
  }

  for (int i = y - 1; i >= 0; --i) {
    if (board[i][x].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, x, i, PieceType::Rook, color)) return true;
    break;
  }
  return false;
}

bool P2P_chess::Piece::isThereQueenOrBishopWithDiagonal(Piece**& board, int x, int y, Color color) {
  for (int i = x + 1, j = y + 1; i < 8 && j < 8; ++i, ++j) { 
    if (board[j][i].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, i, j, PieceType::Bishop, color)) return true;
    break;
  }

  for (int i = x - 1, j = y - 1; i >= 0 && j >= 0; --i, --j) {
    if (board[j][i].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, i, j, PieceType::Bishop, color)) return true;
    break;
  }

  for (int i = x - 1, j = y + 1; i >= 0 && j < 8; --i, ++j) {
    if (board[j][i].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, i, j, PieceType::Bishop, color)) return true;
    break;
  }

  for (int i = x + 1, j = y - 1; i < 8 && j >= 0; ++i, --j) {
    if (board[j][i].getPieceType() == PieceType::None) continue;
    if (isThereWantedPiece(board, i, j, PieceType::Bishop, color)) return true;
    break;
  }
  return false;
}

bool P2P_chess::Piece::isThereWantedPiece(Piece**& board, int x, int y, PieceType type, Color color) {
  if ((board[y][x].getPieceType() != PieceType::Queen
        && board[y][x].getPieceType() != type)
      || board[y][x].getColor() == color) return false;
  return true;
}

bool P2P_chess::Piece::isThereRivalKing(Piece**& board, int x, int y, Color color) {
  std::vector<int> vec;
  getCoordinatePiece(board, PieceType::King, color, vec);
  if (std::abs(vec[0] - y) > 1 || std::abs(vec[1] - x) > 1) {
    return false;
  }
  return true;
}

//########### Board #############
P2P_chess::Board::Board() {
  m_chessBoard = new Piece*[8];
  for (int i = 0; i < 8; ++i) {
    m_chessBoard[i] = new Piece[8];
  }
}

P2P_chess::Board::~Board() {
  for (int i = 0; i < 8; ++i) {
    delete[] m_chessBoard[i];
  }
  delete[] m_chessBoard;
}

void P2P_chess::Board::startGame(int port_number) {
  createBoard();
  if (createNetwork(port_number)) return;
  const char *game = "GAME START\n";

  std::string style =  "   _____ _    _ ______  _____ _____ \n";
              style += "  / ____| |  | |  ____|/ ____/ ____| \n";
              style += " | |    | |__| | |__  | (___| (___  \n";
              style += " | |    |  __  |  __|  \\___  \\___ \\ \n";
              style += " | |____| |  | | |____ ____) |___) | \n";
              style += "  \\_____|_|  |_|______|_____/_____/ \n";
 
 if (write(clients[0]->sockfd, game, strlen(game)) >=  0) {
    if (write(clients[0]->sockfd, style.data(), style.size()) >= 0) {
      if (write(clients[1]->sockfd, game, strlen(game)) >= 0) {
        if (write(clients[1]->sockfd, style.data(), style.size()) < 0) {
            failedWrite(); return;
        }
      }
    }
  }
  showChangedBoard();
  inputStepCoordinate();
}

void P2P_chess::Board::createBoard() {
  for (int i = 2; i < 6; ++i) {
    for (int j = 0; j < 8; ++j) {
      m_chessBoard[i][j].setPieceColorAndSymbol(nullptr, PieceType::None, Color::None, "   |");
    }
  }
  for (int i = 0; i < 8; ++i) {
    m_chessBoard[1][i].setPieceColorAndSymbol(new Pawn(), PieceType::Pawn, Color::White, " \u2659 |");
    m_chessBoard[6][i].setPieceColorAndSymbol(new Pawn(), PieceType::Pawn, Color::Black, " \u265F |");
  }
  m_chessBoard[0][0].setPieceColorAndSymbol(new Rook(), PieceType::Rook, Color::White, " \u2656 |");
  m_chessBoard[0][7].setPieceColorAndSymbol(new Rook(), PieceType::Rook, Color::White, " \u2656 |");
  m_chessBoard[0][1].setPieceColorAndSymbol(new Knight(), PieceType::Knight, Color::White, " \u2658 |");
  m_chessBoard[0][6].setPieceColorAndSymbol(new Knight(), PieceType::Knight, Color::White, " \u2658 |");
  m_chessBoard[0][2].setPieceColorAndSymbol(new Bishop(), PieceType::Bishop, Color::White, " \u2657 |");
  m_chessBoard[0][5].setPieceColorAndSymbol(new Bishop(), PieceType::Bishop, Color::White, " \u2657 |");
  m_chessBoard[0][3].setPieceColorAndSymbol(new Queen(), PieceType::Queen, Color::White, " \u2655 |");
  m_chessBoard[0][4].setPieceColorAndSymbol(new King(), PieceType::King, Color::White, " \u2654 |"); 
  m_chessBoard[7][0].setPieceColorAndSymbol(new Rook(), PieceType::Rook, Color::Black, " \u265C |");
  m_chessBoard[7][7].setPieceColorAndSymbol(new Rook(), PieceType::Rook, Color::Black, " \u265C |");
  m_chessBoard[7][1].setPieceColorAndSymbol(new Knight(), PieceType::Knight, Color::Black, " \u265E |");
  m_chessBoard[7][6].setPieceColorAndSymbol(new Knight(), PieceType::Knight, Color::Black, " \u265E |");
  m_chessBoard[7][2].setPieceColorAndSymbol(new Bishop(), PieceType::Bishop, Color::Black, " \u265D |");
  m_chessBoard[7][5].setPieceColorAndSymbol(new Bishop(), PieceType::Bishop, Color::Black, " \u265D |");
  m_chessBoard[7][3].setPieceColorAndSymbol(new Queen(), PieceType::Queen, Color::Black, " \u265B |");
  m_chessBoard[7][4].setPieceColorAndSymbol(new King(), PieceType::King, Color::Black, " \u265A |");
}

int P2P_chess::Board::createNetwork(int port_number) {
  int socketfd = socket(PF_INET, SOCK_STREAM, 0);

  if (socketfd < 0) {
    std::cout << "ERROR: setsocket failed";
    return EXIT_FAILURE;
  }

  int port = port_number;
  struct sockaddr_in serv_addr;
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(ip);
  serv_addr.sin_port = htons(port);

  /* Ignore pipe signals */
  signal(SIGPIPE, SIG_IGN);

  if (bind(socketfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cout << "ERROR: Socket binding failed" << std::endl;
    return EXIT_FAILURE;
  }

  if (listen(socketfd, 1) < 0) {
    std::cout << "ERROR: Socket listening failed" << std::endl;
    return EXIT_FAILURE;
  }

  struct sockaddr_in cli_addr;
  int connfd = 0, client_count = 0;
  while(client_count < MAX_CLIENTS){
    socklen_t clilen = sizeof(cli_addr);
    connfd = accept(socketfd, (struct sockaddr*)&cli_addr, &clilen);
    /* Check if max clients is reached */
    if((cli_count + 1) == MAX_CLIENTS){
      std::cout << "Max clients reached. Rejected: ";
      print_client_addr(cli_addr);
      std::cout << cli_addr.sin_port << std::endl; 		
      close(connfd);
      continue;
    }

    /* Client settings */
    client_t *cli = new client_t;
    cli->address = cli_addr;
    cli->sockfd = connfd;
    cli->uid = uid++;
    clients[client_count++] = cli;
    char buff_out[BUFFER_SZ];
    char name[32];
    if(recv(cli->sockfd, name, 32, 0) <= 0 ){
      std::cout << "Didn't enter the name.\n";
    } else{
      strcpy(cli->name, name);
      std::cout << cli->name << " has joined" << std::endl;
    }
  }
  return EXIT_SUCCESS;
}

void P2P_chess::Board::print_client_addr(struct sockaddr_in addr){
  std::cout << (addr.sin_addr.s_addr & 0xff)
    << ((addr.sin_addr.s_addr & 0xff00) >> 8)
    << ((addr.sin_addr.s_addr & 0xff0000) >> 16)
    << ((addr.sin_addr.s_addr & 0xff000000) >> 24);
}

void P2P_chess::Board::showChangedBoard() {
  std::string str = "    a   b   c   d   e   f   g   h\n";
  str += " ----------------------------------\n";
  for (int i = 7; i >= 0; --i) {
    str += (" " + std::to_string(i + 1) + "|");
    for (int j = 0; j < 8; ++j) {
      str += (m_chessBoard[i][j].getSymbol()).data();	
    }
    str += std::to_string(i + 1) + "\n ----------------------------------\n";
  }
  str += "    a   b   c   d   e   f   g   h\n";
  if (write(clients[0]->sockfd, str.data(), str.size()) < 0){
    failedWrite();
    return;
  }

  str = "    h   g   f   e   d   c   b   a\n";
  str += " ----------------------------------\n";
  for (int i = 0; i < 8; ++i) {
    str += (" " + std::to_string(i + 1) + "|");
    for (int j = 7; j >= 0; --j) {
      str += (m_chessBoard[i][j].getSymbol()).data();	
    }
    str += std::to_string(i + 1) + "\n ----------------------------------\n";
  }
  str += "    h   g   f   e   d   c   b   a\n";
  if (write(clients[1]->sockfd, str.data(), str.size()) < 0){
    failedWrite();
    return;
  }
}

void P2P_chess::Board::inputStepCoordinate() {	
  if (write(clients[0]->sockfd, "you start\n", 10) < 0){
    failedWrite();
    return;
  }
  int step = 0;
  while(1) {
    if (failedGame) return;
    if (step % 2 == 0) {
      if (!helpInputStepCoordinate(clients[0], clients[1], Color::White, false)) {
        while (!helpInputStepCoordinate(clients[0], clients[1], Color::White, true)) { continue; }
      } 
    } else {
      if (!helpInputStepCoordinate(clients[1], clients[0], Color::Black, false)) {
        while (!helpInputStepCoordinate(clients[1], clients[0], Color::Black, true)) { continue; }
      } 
    }
    if (end == true) return;
    ++step;
  }
}

bool P2P_chess::Board::failedWrite() {
  std::cout << "The customer could not read." << std::endl;
  failedGame = true;
  return true;
}

bool P2P_chess::Board::helpInputStepCoordinate(client_t* cli1, client_t* cli2, Color color, bool sameTurn) {  	
  char buff_out[BUFFER_SZ];
  const char *start = "Please enter the coordinate of the checker and its move\n";
  const char *stop = "please stoped\n";

  bzero(buff_out, BUFFER_SZ);
  if (write(cli1->sockfd, start, strlen(start)) <= 0){
    return failedWrite();
  }
  if (!sameTurn) {
    if (write(cli2->sockfd, stop, strlen(stop)) <= 0){
      return failedWrite();
    }
  }
  if (recv(cli1->sockfd, buff_out, BUFFER_SZ, 0) <= 0 ){
    std::cout << "Didn't enter the name." << std::endl;
  } else {
    int index = 0;
    for (int i = 0; i < BUFFER_SZ; ++i) {
      if (buff_out[i] == ':') {
        index = i + 2;
        break;
      }
    }
    if ((buff_out[index] >= 'a' && buff_out[index] <= 'h') &&
        (buff_out[index + 3] >= 'a' && buff_out[index + 3] <= 'h') &&
        (buff_out[index + 1] >= '0' && buff_out[index + 1] <= '8') &&
        (buff_out[index + 4] >= '0' && buff_out[index + 4] <= '8')) {	
      int x1 = (buff_out[index] - 'a');
      int y1 = (buff_out[index + 1] - '0' - 1);
      int x2 = (buff_out[index + 3] - 'a');
      int y2 = (buff_out[index + 4] - '0' - 1);
      if ((m_chessBoard[y1][x1].getPiece() != nullptr)
          && m_chessBoard[y1][x1].getColor() != color) {
        stop = "Wrong team game : try again\n";
        if (write(cli1->sockfd, stop, strlen(stop)) <= 0){
          return failedWrite();
        }
        return false;
      } 
      if (move(x1, y1, x2, y2)) {
        if (write(cli2->sockfd, buff_out, BUFFER_SZ) <= 0){
          return failedWrite();
        }
        pawnHaveTitleOfQueen();
        showChangedBoard();
        if (isCheck(x2, y2)) {
          if (isCheckMate(x2, y2)) {
            stop = "\n******************CHECKMATE*****************\n";
            if (write(cli1->sockfd, stop, strlen(stop)) <= 0){
              return failedWrite();
            }
            if (write(cli2->sockfd,	stop, strlen(stop)) <= 0){
              return failedWrite();
            }
            end = true;
            return true;;
          } 
          stop = "\n******************CHECK*****************\n";
          if (write(cli1->sockfd, stop, strlen(stop)) <= 0){
            return failedWrite();
          }
          if (write(cli2->sockfd, stop, strlen(stop)) <= 0){
            return failedWrite();
          }
        } 
      } else {
        stop = "\nNone Valid Step: ";
        if (write(cli1->sockfd, stop, strlen(stop)) <= 0){
          return failedWrite();
        }
        return false;
      }
      std::cout << buff_out << std::endl;
      if(write(cli2->sockfd, buff_out, strlen(buff_out)) <= 0){
        return failedWrite();
      }
    }
  }
  return true;
}

bool P2P_chess::Board::move(int x1, int y1, int x2, int y2) {
  if (m_chessBoard[y1][x1].getPiece() == nullptr) return false;
  if ((m_chessBoard[y1][x1].getPiece())->isValidStep(m_chessBoard, x1, y1, x2, y2)) {
    if (isThisCastling(x1, y1, x2, y2)) {
      performCastling(x1, y1, x2, y2);
      return true;
    }
    if (m_chessBoard[y1][x1].getColor() == Color::White
        && m_chessBoard[y1][x1].getPieceType() == PieceType::Pawn
        && m_chessBoard[y2][x2].getPieceType() == PieceType::None
        && m_chessBoard[y2 - 1][x2].getPieceType() == PieceType::Pawn
        && m_chessBoard[y2 - 1][x2].getColor() == Color::Black) {
      m_chessBoard[y2 - 1][x2].removePiece(m_chessBoard);	
    } else if (m_chessBoard[y1][x1].getColor() == Color::Black
        && m_chessBoard[y1][x1].getPieceType() == PieceType::Pawn
        && m_chessBoard[y2][x2].getPieceType() == PieceType::None
        && m_chessBoard[y2 + 1][x2].getColor() == Color::White
        && m_chessBoard[y2 + 1][x2].getPieceType() == PieceType::Pawn) {
      m_chessBoard[y2 + 1][x2].removePiece(m_chessBoard);	
    }
    m_chessBoard[y2][x2] = std::move(m_chessBoard[y1][x1]);
    return true;
  } else {
    return false;
  }
}

void P2P_chess::Board::pawnHaveTitleOfQueen() {
  for (int i = 0; i < 8; ++i) {
    if (m_chessBoard[0][i].getPieceType() == PieceType::Pawn) {
      m_chessBoard[0][i].changePawnToQueen();
      break;
    } else if (m_chessBoard[7][i].getPieceType() == PieceType::Pawn) {
      m_chessBoard[7][i].changePawnToQueen();
      break;
    }
  }
}

bool P2P_chess::Board::isThisCastling(int x1, int y1, int x2, int y2) {
  if ((m_chessBoard[y1][x1].getPieceType() == PieceType::Rook
        && m_chessBoard[y2][x2].getPieceType() == PieceType::King) 
      || (m_chessBoard[y1][x1].getPieceType() == PieceType::King
        && m_chessBoard[y2][x2].getPieceType() == PieceType::Rook)) {
    if (m_chessBoard[y1][x1].getColor() == m_chessBoard[y2][x2].getColor()) {
      return true;
    }
  }
  return false;
}

void P2P_chess::Board::performCastling(int x1, int y1, int x2, int y2) {
  m_chessBoard[y1][x1].changePieceState();
  m_chessBoard[y2][x2].changePieceState();
  if (m_chessBoard[y1][x1].getPieceType() == PieceType::King) {
    if (x1 > x2) {
      m_chessBoard[y2][x1 - 2] = std::move(m_chessBoard[y2][x1]);
      m_chessBoard[y1][x1 - 1] = std::move(m_chessBoard[y1][x2]);
    } else {
      m_chessBoard[y2][x1 + 2] = std::move(m_chessBoard[y2][x1]);
      m_chessBoard[y1][x1 + 1] = std::move(m_chessBoard[y1][x2]);
    }
  } else {
    if (x1 > x2) {
      m_chessBoard[y1][x2 + 2] = std::move(m_chessBoard[y1][x2]);
      m_chessBoard[y2][x2 + 1] = std::move(m_chessBoard[y2][x1]);
    } else {
      m_chessBoard[y1][x2 - 2] = std::move(m_chessBoard[y1][x2]);
      m_chessBoard[y2][x2 - 1] = std::move(m_chessBoard[y2][x1]);
    }
  }
}

bool P2P_chess::Board::isCheck(int x, int y) {
  if (m_chessBoard[y][x].getPieceType() == PieceType::None) return false;
  std::vector<int> vec;
  m_chessBoard[y][x].getCoordinatePiece(m_chessBoard, PieceType::King, m_chessBoard[y][x].getColor(), vec); 
  if ((m_chessBoard[y][x].getPiece())->isValidStep(m_chessBoard, x, y, vec[1], vec[0])) {
    return true;
  }
  return false;
}

bool P2P_chess::Board::isCheckMate(int x, int y) {
  std::vector<int> vec; //:( piti getCoordinate@ Board-i func liner
  Color color = (m_chessBoard[y][x].getColor() == Color::White) ? Color::Black : Color::White;
  m_chessBoard[y][x].getCoordinatePiece(m_chessBoard, PieceType::King, color, vec); 
  if (m_chessBoard[y][x].openCheckOnHimself(m_chessBoard, vec[0], vec[1], color)) return true;
  return false;
}

//########### Pawn #############
bool P2P_chess::Pawn::isValidStep(Piece**& board, int x1, int y1, int x2, int y2) {
  if (board[y1][x1].getColor() == Color::White) {
    return isValidStepOfWhitePiece(board, x1, y1, x2, y2);
  } else if (board[y1][x1].getColor() == Color::Black) {
    return isValidStepOfBlackPiece(board, x1, y1, x2, y2);
  }
  return false;
}

bool P2P_chess::Pawn::isValidStepOfWhitePiece(Piece**& board, int x1, int y1, int x2, int y2) {
  if (y2 - y1 == 1 && x2 == x1 && board[y2][x2].getPieceType() == PieceType::None
      || y2 - y1 == 2 && x2 == x1 && y1 == 1 && board[y2][x2].getPieceType() == PieceType::None) {
    //does he open a check for his king?
    if (!isOpenCheckForKingVertical(board, x1, y1)) {
      if (isOpenCheckForKingVertical(board, x2, y2, x1, y1)) {
        return false;
      }
    }
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) return false;
    return true;
  }
  //regular capturing
  if (y2 - y1 == 1 && ((x2 - x1 == 1 && x2 != 8) || (x1 - x2 == 1 && x1 != -1))
      && board[y2][x2].getPieceType() != PieceType::None
      && board[y2][x2].getColor() != Color::White) {
    if (!isOpenCheckForKingVertical(board, x1, y1)) return false;
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) return false;
    return true;
  }
  // en passant capture
  if (y2 - y1 == 1 && ((x2 - x1 == 1 && x2 != 8) || (x1 - x2 == 1 && x1 != -1))
      && board[y2][x2].getPieceType() == PieceType::None
      && board[y2 - 1][x2].getPieceType() == PieceType::Pawn
      && board[y2 - 1][x2].getColor() != Color::White) {
    if (!isOpenCheckForKingVertical(board, x1, y1)) return false;
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) {
      if (isOpenCheckForKingDiagonal(board, x2, y2, x1, y1)) return false;
    }
    return true;
  }
  return false;
}

bool P2P_chess::Pawn::isValidStepOfBlackPiece(Piece**& board, int x1, int y1, int x2, int y2) {
  if ((y1 - y2 == 1 && x2 == x1 && board[y2][x2].getPieceType() == PieceType::None)
      || (y1 - y2 == 2 && x2 == x1 && y1 == 6 && board[y2][x2].getPieceType() == PieceType::None)) {
    // does he open a check for his king?
    if (!isOpenCheckForKingVertical(board, x1, y1)) { 
      if (isOpenCheckForKingVertical(board, x2, y2, x1, y1)) {
        return false;
      }
    }
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) return false;
    return true;
  }
  // regular capturing
  if (y1 - y2 == 1 && ((x2 - x1 == 1 && x2 != 8) || (x1 - x2 == 1 && x1 != -1))
      && board[y2][x2].getPieceType() != PieceType::None
      && board[y2][x2].getColor() != Color::Black) {
    if (!isOpenCheckForKingVertical(board, x1, y1)) return false;
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) return false;
    return true;
  }
  // en passant capture
  if(y1 - y2 == 1 && ((x2 - x1 == 1 && x2 != 8) || (x1 - x2 == 1 && x1 != -1))
      && board[y2][x2].getPieceType() == PieceType::None
      && board[y2 + 1][x2].getPieceType() == PieceType::Pawn
      && board[y2 + 1][x2].getColor() != Color::Black) {
    if (!isOpenCheckForKingVertical(board, x1, y1)) return false;
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) {
      if (isOpenCheckForKingDiagonal(board, x2, y2, x1, y1)) return false; //hnaravor e uteluc nuyn uxxu vra mnum
    }
    return true;
  }
  return false;
}


//########## Knight ###########
bool P2P_chess::Knight::isValidStep(Piece**& board, int x1, int y1, int x2, int y2) {
  if ((std::abs(y2 - y1) == 1 && std::abs(x2 - x1) == 2)
      || (std::abs(y2 - y1) == 2 && std::abs(x2 - x1) == 1)) {
    if (board[y1][x1].getColor() != board[y2][x2].getColor()) {
      if (!isOpenCheckForKingVertical(board, x1, y1)) return false;
      if (!isOpenCheckForKingDiagonal(board, x1, y1)) return false;
      return true;
    }
  }	
  return false;
}

//########### Rook ##########
bool P2P_chess::Rook::isValidStep(Piece**& board, int x1, int y1, int x2, int y2) { 
  if ((y1 != y2 && x1 == x2) || (x1 != x2 && y1 == y2)) {
    if (!isThereFigureOnTheWay(board, x1, y1, x2, y2)) { return false; }
    if (!areFromSameTeam(board, x1, y1, x2, y2)) { 
      if (board[y2][x2].getPieceType() != PieceType::King) { return false; }
      if (!meetsConditionsOfCastling(board, x1, y1, x2, y2)) { return false; } 
      if (!openCheckOnHimselfCastlingTime(board, x2, x1, y1)) { return false; }
    } else { 
      if (!isOpenCheckForKingVertical(board, x1, y1)) {
        if (isOpenCheckForKingVertical(board, x2, y2, x1, y1)) return false;
      }
      if (!isOpenCheckForKingDiagonal(board, x1, y1)) return false;
      board[y1][x1].changePieceState();
    }
    return true;
  }
  return false;
}

bool P2P_chess::Rook::areFromSameTeam(Piece**& board, int x1, int y1, int x2, int y2) {
  if (board[y1][x1].getColor() != board[y2][x2].getColor()) { return true; }
  return false;
}

bool P2P_chess::Rook::isThereFigureOnTheWay(Piece**& board, int x1, int y1, int x2, int y2) {
  if (y1 > y2) {
    for (int i = y2 + 1; i < y1; ++i) {
      if (board[i][x1].getPieceType() != PieceType::None) { return false; }
    }
  } else if (y2 > y1) {
    for (int i = y1 + 1; i < y2; ++i) {
      if (board[i][x1].getPieceType() != PieceType::None) { return false; }
    }
  } else if (x1 > x2) {
    for (int i = x2 + 1; i < x1; ++i) {
      if (board[y1][i].getPieceType() != PieceType::None) { return false; }
    }
  } else if (x2 > x1) {
    for (int i = x1 + 1; i < x2; ++i) {
      if (board[y1][i].getPieceType() != PieceType::None) { return false;}
    }
  }
  return true;
}

bool P2P_chess::Rook::meetsConditionsOfCastling(Piece**& board, int x1, int y1, int x2, int y2) {
  if (board[y1][x1].getState() == false 
      && board[y2][x2].getState() == false) {
    return true;
  }
  return false;
}

//########### Bishop ########### 
bool P2P_chess::Bishop::isValidStep(Piece**& board, int x1, int y1, int x2, int y2) {
  if (std::abs(x1 - x2) != std::abs(y1 - y2)) { return false; } //is the step right?
  if (!isThereFigureOnTheWay(board, x1, y1, x2, y2)) { return false; }
  if (!areFromSameTeam(board, x1, y1, x2, y2)) { return false; }
  if (!isOpenCheckForKingVertical(board, x1, y1)) return false;
  if (!isOpenCheckForKingDiagonal(board, x1, y1)) {
    if (isOpenCheckForKingDiagonal(board, x2, y2, x1, y1)) return false;
  }
  return true;
}


bool P2P_chess::Bishop::isThereFigureOnTheWay(Piece**& board, int x1, int y1, int x2, int y2) {
  if (y1 > y2 && x1 > x2) {
    for (int i = x1 - 1, j = y1 - 1; i > x2 && j > y2; --i, --j) {
      if (board[j][i].getPieceType() != PieceType::None) { return false; }
    }
  } else if (y1 > y2 && x2 > x1) {
    for (int i = x1 + 1, j = y1 - 1; i < x2 && j > y2; ++i, --j) {
      if (board[j][i].getPieceType() != PieceType::None) { return false; }
    }
  } else if (y2 > y1 && x1 > x2) {
    for (int i = x1 - 1, j = y1 + 1; i > x2 && j < y2; --i, ++j) {
      if (board[j][i].getPieceType() != PieceType::None) { return false; }
    }
  } else if (y2 > y1 && x2 > x1) {
    for (int i = x1 + 1, j = y1 + 1; i < x2 && j < y2; ++i, ++j) {
      if (board[j][i].getPieceType() != PieceType::None) { return false;}
    }
  }
  return true;
}

bool P2P_chess::Bishop::areFromSameTeam(Piece**& board, int x1, int y1, int x2, int y2) {
  if (board[y1][x1].getColor() != board[y2][x2].getColor()) { return true; }
  return false;
}

//############ Queen #############
bool P2P_chess::Queen::isValidStep(Piece**& board, int x1, int y1, int x2, int y2) {
  if (Bishop::isValidStep(board, x1, y1, x2, y2)) { return true; }
  if ((y1 != y2 && x1 == x2) || (x1 != x2 && y1 == y2)) {
    if (!Rook::isThereFigureOnTheWay(board, x1, y1, x2, y2)) { return false; }
    if (!Rook::areFromSameTeam(board, x1, y1, x2, y2)) { return false; }
    if (!isOpenCheckForKingVertical(board, x1, y1)) {
      if (isOpenCheckForKingVertical(board, x2, y2, x1, y1)) { return false; }
    }
    if (!isOpenCheckForKingDiagonal(board, x1, y1)) { return false; }
    return true;
  }
  return false;
}

//############ King #############
bool P2P_chess::King::isValidStep(Piece**& board, int x1, int y1, int x2, int y2) {
  if (std::abs(y1 - y2) > 1 || std::abs(x1 - x2) > 1) { 
    if (board[y1][x1].getColor() != board[y2][x2].getColor()
        || board[y2][x2].getPieceType() != PieceType::Rook) { return false; }
    if (!meetsConditionsOfCastling(board, x1, y1, x2, y2)) { return false; }	
    //the king opens check on himself?
    if (!openCheckOnHimselfCastlingTime(board, x1, x2, y1)) return false;
    return true; 
  }
  if (board[y1][x1].getColor() != board[y2][x2].getColor()) {
    if (!openCheckOnHimself(board, x2, y2, board[y1][x1].getColor())) return false;
    board[y1][x1].changePieceState();	
    return true;
  }
  return false;
}

bool P2P_chess::King::meetsConditionsOfCastling(Piece**& board, int x1, int y1, int x2, int y2) {
  if (board[y1][x1].getState() == false 
      && board[y2][x2].getState() == false) {
    return true;
  }
  return false;
}
