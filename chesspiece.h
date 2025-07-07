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
    void setSelectedState(bool selected);

    QPoint getBoardPosition() const { return position; }
    void setBoardPosition(const QPoint& pos) { position = pos; }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    QPoint toBoardCoord(QPointF scenePos);

private:
    PieceType type;
    Color color;

    QPointF dragStartPos;
    QPoint position;
    QPointF oldClickPosition;

    static ChessPiece* selectedPiece;

    Board *board;
};

#endif // PIECE_H
