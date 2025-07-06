#ifndef BOARD_H
#define BOARD_H

#include "chesspiece.h"

#include <QGraphicsScene>

class Board {   // Singleton class
    Board(QGraphicsScene *s) {
        this->scene = s;
    }
public:
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    static Board* getInstance(QGraphicsScene *s = nullptr);
    void setupInitialPosition();

    ChessPiece* pieceAt(int x, int y) const;                     // узнать тип фигури в конкретнем поле
    // bool isMoveValid(Position from, Position to);                // проверка хода
    // void makeMove(Position from, Position to);                   // setPos

    // find and show aviable moves for cureent(clicked) piece
    QList<QPoint> availableMoves(ChessPiece* piece) const;       // return all aviable moves
    void showHints(const QList<QPoint>& moves);                 // call after availableMoves() and show moves
    void clearHints();

    bool isEmpty(int x, int y) const;
    bool isEnemy(int, int, ChessPiece::Color) const;

    void removePiece(int x, int y) { pieces[y][x] = nullptr; }

    void movePiece(ChessPiece* piece, int x, int y) { pieces[y][x] = piece; }

    static constexpr int tileSize = 65;
private:
    ChessPiece *pieces[8][8] = {};
    QGraphicsScene *scene = nullptr;
    QList<QGraphicsEllipseItem*> hintDots;
    QVector<ChessPiece*> whitePieces;
    QVector<ChessPiece*> blackPieces;
};

#endif // BOARD_H
