#ifndef BOARD_H
#define BOARD_H

#include "chesspiece.h"

#include <QGraphicsScene>
#include <QObject>

// Singleton class
class Board : public QObject {
    Q_OBJECT
    explicit Board(QObject* parent = nullptr) : QObject(parent) {
        // private constructor
    }
public:
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    static Board* getInstance(QObject* parent = nullptr) {
        static Board instance(parent);
        return &instance;
    }
    void setupInitialPosition();

    ChessPiece* pieceAt(int x, int y) const;
    bool isMoveValid(QPoint from, QPoint to) const;
    // void makeMove(QPoint from, QPoint to);

    QList<QPoint> availableMoves(ChessPiece* piece) const;       // return all aviable moves
    void showHints(const QList<QPoint>& moves);                 //  show aviable moves
    void clearHints();

    bool isEmpty(int x, int y) const;
    bool isEnemy(int, int, ChessPiece::Color) const;

    void removePiece(int x, int y) { pieces[y][x] = nullptr; }
    void capturePiece(int x, int y);
    void movePiece(ChessPiece* piece, int x, int y) { pieces[y][x] = piece; }
    void placePiece(ChessPiece* piece, int x, int y);

    void setScene(QGraphicsScene *s) { this->scene = s; }
    QGraphicsScene* getScene() { return scene; }

    static constexpr int tileSize = 65;

    ChessPiece::Color currentTurn = ChessPiece::White;

    bool isCorrectTurn(ChessPiece* piece) const;
    void switchTurn();

    ChessPiece* pawnPromotion(ChessPiece::PieceType type, ChessPiece::Color color, int x, int y);

private: //helper methods
    bool isInsideBoard(int x, int y) const { return x >= 0 && x < 8 && y >= 0 && y < 8; }

private:
    ChessPiece *pieces[8][8] = {};
    QGraphicsScene *scene = nullptr;
    QList<QGraphicsEllipseItem*> hintDots;
    mutable QVector<ChessPiece*> killedWhitePieces;
    mutable QVector<ChessPiece*> killedBlackPieces;
    bool hasMoved = false;

signals:
    void pieceCaptured(ChessPiece* piece);  // фигура, которую убили
};

#endif // BOARD_H
