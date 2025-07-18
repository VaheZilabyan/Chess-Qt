#ifndef BOARD_H
#define BOARD_H

#include "chesspiece.h"
#include "chessclock.h"
#include "stockfishengine.h"

#include <QGraphicsScene>
#include <QObject>

// Singleton class
class Board : public QObject {
    Q_OBJECT
    explicit Board(QObject* parent = nullptr) : QObject(parent) {
    }

public slots:
    void onBestMoveReceived(const QString& move);
public:
    static constexpr int tileSize = 65;

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

    ChessPiece* pieceAt(int x, int y) const;    //del
    bool isMoveValid(QPoint from, QPoint to) const;    //del

    QList<QPoint> legalMoves(ChessPiece* piece);    //del
    QList<QPoint> rawAvailableMoves(ChessPiece* piece) const;    //del
    QList<QPoint> availableMoves(ChessPiece* piece) const;    //del

    void showHints(const QList<QPoint>& moves);
    void clearHints();
    void highlightAfterCheck(ChessPiece::Color);
    QGraphicsRectItem* getHighlight() const {return highlight; }

    bool isEmpty(int x, int y) const;    //del
    bool isEnemy(int, int, ChessPiece::Color) const;    //del

    void capturePiece(int x, int y);
    void movePieceFromTo(ChessPiece* piece, QPoint from, QPoint to);
    void movePiece(ChessPiece *piece, int x, int y);

    void setScene(QGraphicsScene *s) { this->scene = s; }
    QGraphicsScene* getScene() const { return scene; }

    ChessPiece::Color currentTurn = ChessPiece::White;

    bool isCorrectTurn(ChessPiece* piece) const;    //del
    void switchTurn();    //del

    ChessPiece* pawnPromotion(ChessPiece::PieceType type, ChessPiece::Color color);
    QPoint getEnPassantTarget() const { return enPassantTarget; }

    bool isKingInCheck(ChessPiece::Color color);    //del
    bool isSquareAttacked(QPoint pos, ChessPiece::Color byColor) const;    //del
    bool isCheckmate(ChessPiece::Color color);    //del
    bool isStalemate(ChessPiece::Color color);    //del

    void addMoveHistory(ChessPiece* piece, QPoint from, QPoint to) {
        emit addMoveSignal(piece, from, to);
    }

    void setBoardColor(QPair<QColor, QColor> bc);
    void setClock(ChessClock* clock) { this->clock = clock; }
    ChessClock* getClock() const { return clock; }

    void setDifficultyLevel(QString level) { engine->sendCommand("setoption name Skill Level value " + level + "\n"); }
    void setEngine(StockfishEngine *e) { engine = e; }
    StockfishEngine* getEngine() const { return engine; }
    void setVSComputer(bool vsSt) { vsStockfish = vsSt; }
    bool isAgainstComputer() const { return vsStockfish; }

    QStringList moveHistory;

private: //helper methods
    bool isInsideBoard(int x, int y) const { return x >= 0 && x < 8 && y >= 0 && y < 8; }
    QString getSvgPath(ChessPiece::PieceType type, ChessPiece::Color color);

private:
    StockfishEngine *engine;

    ChessPiece *pieces[8][8] = {};
    QGraphicsScene *scene = nullptr;    //keep this, delete scene from mainWindos
    QList<QGraphicsEllipseItem*> hintDots;
    QPoint enPassantTarget = {-1, -1};
    QPoint whiteKingPos = {4, 7};
    QPoint blackKingPos = {4, 0};
    bool vsStockfish = false;

    QGraphicsRectItem *highlight;
    QPair<QColor, QColor> boardColor = {Qt::white, Qt::gray};
    ChessClock *clock;

signals:
    void pieceCaptured(ChessPiece* piece);  // фигура, которую убили
    void addMoveSignal(ChessPiece* piece, QPoint from, QPoint to);
    void checkmateSignal();
};

#endif // BOARD_H
