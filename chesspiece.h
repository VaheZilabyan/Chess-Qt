#ifndef PIECE_H
#define PIECE_H

#include <QGraphicsSvgItem>
#include <QGraphicsSceneMouseEvent>

class Board;

class ChessPiece : public QGraphicsSvgItem {
public:
    enum PieceType { King, Queen, Rook, Bishop, Knight, Pawn };
    enum Color { White, Black };

    ChessPiece();
    ChessPiece(PieceType type, Color color, const QString& svgPath);

    PieceType getType() const;
    Color getColor() const;
    QString getSvgPath() const { return svgPath; }

    void setSelectedState(bool selected);

    QPoint getPositionFromBoard() const { return position; }
    void setPositionOnTheBoard(const QPoint& pos) { position = pos; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    QPoint toBoardCoord(QPointF scenePos);

private:
    PieceType type;
    Color color;
    QString svgPath;

    QPointF dragStartPos;
    QPoint position;

    static ChessPiece* selectedPiece;
    QList<QPoint> cachedMoves;

    Board *board;
};

#endif // PIECE_H
