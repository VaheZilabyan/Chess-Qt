#ifndef BOARD_H
#define BOARD_H

#include "chesspiece.h"

#include <QGraphicsScene>

class Board {   // Singleton class
    Board() {}
public:
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    static Board* getInstance();

    static void setupInitialPosition(QGraphicsScene *scene);
    //bool isMoveValid(Position from, Position to); // проверка хода
    //void makeMove(Position from, Position to);
    // дополнительные функции: шах, мат, рокировка и т.д.

    static constexpr int tileSize = 65;
private:
    static QVector<ChessPiece*> whitePieces;
    static QVector<ChessPiece*> blackPieces;
};

#endif // BOARD_H
