#ifndef P2P_CHESS_H
#define P2P_CHESS_H

// chessBoard + place
#include <string>
#include <vector>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <iostream>


namespace P2P_chess {

  #define MAX_CLIENTS 2
  #define BUFFER_SZ 4096

  //const char *ip = "192.168.88.67";

  typedef struct {
    struct sockaddr_in address;
    int sockfd;
    int uid;
    char name[32];
  } client_t;

  enum class Color {
    White,
    Black,
    None
  };

  enum class PieceType {
    King,
    Queen,
    Rook,
    Knight,
    Bishop,
    Pawn,
    None
  };

  enum class OpenCheckKing {
    QueenOrRookOrBishop,
    King,
    Other,
    None
  };

  //ete coordinate pahei esqan dajan cher lini
  class Piece
  {
  public:
    Piece() = default;
    virtual ~Piece();

  public:
    Piece& operator=(Piece&& rhs);
    void removePiece(Piece**& board);

  public:
    virtual bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2){ return false; };
    void setPieceColorAndSymbol(Piece*, PieceType, Color, const std::string&);
    void changePawnToQueen();
    void changePieceState();

  public:
    PieceType getPieceType() const;
    Color getColor() const;
    bool getState() const;
    std::string getSymbol() const;
    Piece* getPiece() const;

  public:
    bool isOpenCheckForKingVertical(Piece**& board, int x, int y, int x1 = -1, int y1 = -1);
    bool isOpenCheckForKingDiagonal(Piece**& board, int x, int y, int x1 = -1, int y1 = -1);
    OpenCheckKing kingOrQueenOrRookOrBishop(Piece**& board, Color color, PieceType rookOrBishop, int x, int y, int x1, int y1);

    //the king opens check on himself?
    bool openCheckOnHimselfCastlingTime(Piece**& board, int xk, int xR, int y);
    bool openCheckOnHimself(Piece**& board, int movedX, int movedY, Color color);

  private:
    bool isTherePawnWithDiagonal(Piece**& board, int x, int y, Color color);
    bool isThereRivalKnight(Piece**& board, int x, int y, Color color);
    bool isThereQueenOrRookWithVertically(Piece**& board, int x, int y, Color color);
    bool isThereQueenOrBishopWithDiagonal(Piece**& board, int x, int y, Color color);
    bool isThereRivalKing(Piece**& board, int x, int y, Color color);
    bool isThereWantedPiece(Piece**& board, int x, int y, PieceType type, Color color);
  public:
    void getCoordinatePiece(Piece**& board, PieceType type, Color color, std::vector<int>& vec);

  private:
    Color m_color;
    PieceType m_type;
    bool m_moved;
    std::string m_symbol;
    Piece* m_piece;
  };

  class Board 
  {
  public:
    Board();
    virtual ~Board();

  public:
    bool move(int x1, int y1, int x2, int y2);
    void performCastling(int x1, int y1, int x2, int y2);
    bool isThisCastling(int x1, int y1, int x2, int y2);
    void pawnHaveTitleOfQueen();
    void inputStepCoordinate();
    bool isCheck(int x, int y);
    bool isCheckMate(int x, int y);
    void showChangedBoard();
    void startGame(int port_number); 
    void createBoard(); 
    void print_client_addr(struct sockaddr_in addr);
    int createNetwork(int port_number);

  protected:
    Piece** m_chessBoard;
    client_t *clients[MAX_CLIENTS];
  private:
    bool helpInputStepCoordinate(client_t* cli1, client_t* cli2, Color color, bool sameTurn);  	
    bool failedWrite();

  private:
    bool end = false;
    bool failedGame = false;

  }; //Board

  class King : public Piece
  {
  public:
    King() = default;
    virtual ~King() = default;
    bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2) override;

  private:
    bool meetsConditionsOfCastling(Piece**& board, int x1, int y1, int x2, int y2);
  }; //King

  class Rook : virtual public Piece
  {
  public:
    Rook() = default;
    virtual ~Rook() = default;
    bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2) override;

  protected:
    bool areFromSameTeam(Piece**& board, int x1, int y1, int x2, int y2);
    bool isThereFigureOnTheWay(Piece**& board, int x1, int y1, int x2, int y2);

  private:
    bool meetsConditionsOfCastling(Piece**& board, int x1, int y1, int x2, int y2); 
  }; //Rook

  class Knight : public Piece
  {
  public:
    Knight() = default;
    virtual ~Knight() = default;
    bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2) override;
  }; // Knight

  class Bishop : virtual public Piece
  {
  public:
    Bishop() = default;
    virtual ~Bishop() = default;
    bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2) override;

  protected:
    bool isThereFigureOnTheWay(Piece**& board, int x1, int y1, int x2, int y2);
    bool areFromSameTeam(Piece**& board, int x1, int y1, int x2, int y2);
  }; //Bishop

  class Pawn : public Piece
  {
  public:
    Pawn() = default;
    virtual ~Pawn() = default;
    bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2) override;

  private:
    bool isValidStepOfBlackPiece(Piece**& board, int x1, int y1, int x2, int y2);
    bool isValidStepOfWhitePiece(Piece**& board, int x1, int y1, int x2, int y2);
  }; //Pawn

  class Queen : virtual public Piece, public Bishop, public Rook
  {
  public:
    Queen() = default;
    virtual ~Queen() = default;

    bool isValidStep(Piece**& board, int x1, int y1, int x2, int y2) override;
  }; //Queen

} //P2P_chess

#endif // P2P_CHESS_H
