#ifndef PIECE_H
#define PIECE_H

#include "sound.h"

#include <QGraphicsSvgItem>
#include <QGraphicsSceneMouseEvent>

class Board;

class ChessPiece : public QGraphicsSvgItem {
public:
    enum PieceType { King, Queen, Rook, Bishop, Knight, Pawn };
    enum Color { White, Black };

    ChessPiece(PieceType type, Color color, const QString& svgPath);

    QList<QPoint> getCachedMoves() const { return cachedMoves; }
    QPoint getPositionFromBoard() const { return position; }
    QString getSvgPath() const { return svgPath; }
    PieceType getType() const;
    Color getColor() const;

    void setPositionOnTheBoard(const QPoint& pos) { position = pos; }
    void setSelectedState(bool selected);

    //for castling
    bool hasMovedAlready() const { return hasMoved; }
    void markAsMoved() { hasMoved = true; }

    static void setSelectedPiece(ChessPiece* sp) { selectedPiece = sp; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    QPoint toBoardCoord(QPointF scenePos);

private:
    static ChessPiece* selectedPiece;
    QList<QPoint> cachedMoves;
    QPointF dragStartPos;
    QPoint position;
    PieceType type;
    Color color;
    QString svgPath;
    Board *board;
    bool hasMoved = false;
};

#endif // PIECE_H
