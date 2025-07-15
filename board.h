#ifndef BOARD_H
#define BOARD_H

#include "chesspiece.h"
#include "chessclock.h"

#include <QGraphicsScene>
#include <QObject>
#include <QSoundEffect>

// Singleton class
class Board : public QObject {
    Q_OBJECT
    explicit Board(QObject* parent = nullptr) : QObject(parent) {
        // private constructor
        setupSounds();
    }
public:
    Board(const Board&) = delete;
    Board& operator=(const Board&) = delete;

    static Board* getInstance(QObject* parent = nullptr) {
        static Board instance(parent);
        return &instance;
    }

    void resetBoard();
    void setupBoard();
    void setupInitialPosition();
    void addPiece(ChessPiece::PieceType type, ChessPiece::Color color, int x, int y);

    ChessPiece* pieceAt(int x, int y) const;
    bool isMoveValid(QPoint from, QPoint to) const;
    // void makeMove(QPoint from, QPoint to);

    QList<QPoint> legalMoves(ChessPiece* piece);
    QList<QPoint> availableMoves(ChessPiece* piece) const;       // return all aviable moves
    void showHints(const QList<QPoint>& moves);                 //  show aviable moves
    void clearHints();

    bool isEmpty(int x, int y) const;
    bool isEnemy(int, int, ChessPiece::Color) const;

    void capturePiece(int x, int y);
    void movePiece(ChessPiece* piece, int x, int y);

    void setScene(QGraphicsScene *s) { this->scene = s; }
    QGraphicsScene* getScene() const { return scene; }

    static constexpr int tileSize = 65;

    ChessPiece::Color currentTurn = ChessPiece::White;

    bool isCorrectTurn(ChessPiece* piece) const;
    void switchTurn();

    ChessPiece* pawnPromotion(ChessPiece::PieceType type, ChessPiece::Color color);
    QPoint getEnPassantTarget() const { return enPassantTarget; }

    bool isKingInCheck(ChessPiece::Color color);
    bool isSquareAttacked(QPoint pos, ChessPiece::Color byColor) const;
    bool isCheckmate(ChessPiece::Color color);
    bool isStalemate(ChessPiece::Color color);

    void addMoveHistory(ChessPiece* piece, QPoint from, QPoint to) {
        emit addMoveSignal(piece, from, to);
    }

    void setBoardColor(QPair<QColor, QColor> bc);
    void setClock(ChessClock* clock) { this->clock = clock; }
    ChessClock* getClock() const { return clock; }

    void playCaptureSound();
    void playCastleSound();
    void playCheckSound();
    void playDrawSound();
    void playStartSound();
    void playMoveSound();
    void playPromoteSound();
    void playTenSecondSound();

private: //helper methods
    bool isInsideBoard(int x, int y) const { return x >= 0 && x < 8 && y >= 0 && y < 8; }
    QString getSvgPath(ChessPiece::PieceType type, ChessPiece::Color color);

private:
    QSoundEffect captureSound;
    QSoundEffect castleSound;
    QSoundEffect checkSound;
    QSoundEffect drawSound;
    QSoundEffect startSound;
    QSoundEffect moveSound;
    QSoundEffect promoteSound;
    QSoundEffect tenSecondSound;
    void setupSounds();

    ChessPiece *pieces[8][8] = {};
    QGraphicsScene *scene = nullptr;
    QList<QGraphicsEllipseItem*> hintDots;
    mutable QVector<ChessPiece*> killedWhitePieces;
    mutable QVector<ChessPiece*> killedBlackPieces;
    QPoint enPassantTarget = {-1, -1};
    QPoint whiteKingPos = {4, 7};
    QPoint blackKingPos = {4, 0};

    QPair<QColor, QColor> boardColor = {Qt::white, Qt::gray};
    ChessClock *clock;

signals:
    void pieceCaptured(ChessPiece* piece);  // фигура, которую убили
    void addMoveSignal(ChessPiece* piece, QPoint from, QPoint to);
};

#endif // BOARD_H
