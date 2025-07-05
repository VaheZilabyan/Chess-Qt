#include "chesspiece.h"
#include "board.h"

#include <QDebug>
#include <QPixmap>
#include <QGraphicsColorizeEffect>

ChessPiece* ChessPiece::selectedPiece = nullptr;

ChessPiece::ChessPiece(PieceType t, Color c, QPixmap pixmap)
    : QGraphicsPixmapItem(pixmap), type(t), color(c) {
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

ChessPiece::PieceType ChessPiece::getType() const {
    return type;
}

ChessPiece::Color ChessPiece::getColor() const {
    return color;
}

void ChessPiece::setSelectedState(bool selected)
{
    if (selected)
        setGraphicsEffect(nullptr);  // убираем старые эффекты

    QGraphicsColorizeEffect* effect = selected ? new QGraphicsColorizeEffect : nullptr;
    if (effect) {
        effect->setColor(Qt::yellow);
        effect->setStrength(0.7);
        setGraphicsEffect(effect);
    } else {
        setGraphicsEffect(nullptr);
    }
}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);

    /*QString colorStr = (color == White) ? "White" : "Black";
    QString typeStr;

    switch (type) {
    case King:   typeStr = "King"; break;
    case Queen:  typeStr = "Queen"; break;
    case Rook:   typeStr = "Rook"; break;
    case Bishop: typeStr = "Bishop"; break;
    case Knight: typeStr = "Knight"; break;
    case Pawn:   typeStr = "Pawn"; break;
    }

    qDebug() << colorStr << typeStr << "clicked!";*/

    // Снять выделение с предыдущей фигуры
    if (selectedPiece && selectedPiece != this)
        selectedPiece->setSelectedState(false);

    // Если кликаем по уже выделенной фигуре — снять выделение
    if (selectedPiece == this) {
        setSelectedState(false);
        selectedPiece = nullptr;
    } else {
        setSelectedState(true);
        selectedPiece = this;
    }
    dragStartPos = event->scenePos();
    setZValue(1);  // выше всех при перетаскивании
    QGraphicsPixmapItem::mousePressEvent(event);
}

void ChessPiece::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF offset = event->scenePos() - dragStartPos;
    QPointF newPos = pos() + offset;

    // Размер доски и клетки
    const int minX = 0;
    const int minY = 0;
    const int maxX = 7 * Board::tileSize;
    const int maxY = 7 * Board::tileSize;

    // Ограничиваем координаты
    qreal clampedX = std::clamp(newPos.x(), qreal(minX), qreal(maxX));
    qreal clampedY = std::clamp(newPos.y(), qreal(minY), qreal(maxY));

    setPos(clampedX, clampedY);
    dragStartPos = event->scenePos();
    setSelectedState(false);

    QGraphicsPixmapItem::mouseMoveEvent(event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Привязка к сетке доски (tileSize)
    int x = int(event->scenePos().x()) / Board::tileSize;
    int y = int(event->scenePos().y()) / Board::tileSize;

    x = std::clamp(x, 0, 7);
    y = std::clamp(y, 0, 7);

    // Оставляем фигуру по центру клетки
    setPos(x * Board::tileSize, y * Board::tileSize);

    setZValue(0);  // вернуть назад

    QGraphicsPixmapItem::mouseReleaseEvent(event);
}
