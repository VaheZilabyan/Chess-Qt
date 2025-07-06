#ifndef PIECE_H
#define PIECE_H

#include <QGraphicsSvgItem>
#include <QGraphicsSceneMouseEvent>

class ChessPiece : public QGraphicsSvgItem {
public:
    enum PieceType { King, Queen, Rook, Bishop, Knight, Pawn };
    enum Color { White, Black };

    ChessPiece();
    ChessPiece(PieceType type, Color color, const QString& svgPath);

    PieceType getType() const;
    Color getColor() const;
    void setSelectedState(bool selected);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    PieceType type;
    Color color;
    QPointF dragStartPos;
    static ChessPiece* selectedPiece;
};

#endif // PIECE_H
