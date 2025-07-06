#include "chesspiece.h"
#include "board.h"

#include <QDebug>
#include <QPixmap>
#include <QGraphicsColorizeEffect>
#include <QSvgRenderer>
#include <QFile>

ChessPiece* ChessPiece::selectedPiece = nullptr;

ChessPiece::ChessPiece(PieceType type, Color color, const QString& svgPath)
    : QGraphicsSvgItem(svgPath), type(type), color(color)
{
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptedMouseButtons(Qt::LeftButton);
    setAcceptHoverEvents(true);
    setZValue(1);
    setScale(Board::tileSize/ 128);
}

ChessPiece::PieceType ChessPiece::getType() const {
    return type;
}

ChessPiece::Color ChessPiece::getColor() const {
    return color;
}

void ChessPiece::setSelectedState(bool selected) {
    if (selected) {
        setOpacity(0.6);
    } else {
        setOpacity(1.0);
    }
}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);

    dragStartPos = event->scenePos();
    //position = dragStartPos.toPoint();
    setZValue(1);  // Bring to front

    int cellSize = 64;  // размер одной клетки в пикселях (задай свой)
    int col = static_cast<int>(scenePos().x()) / cellSize;  // номер колонки 0..7
    int row = static_cast<int>(scenePos().y()) / cellSize;  // номер строки 0..7
    position.setX(col);
    position.setY(row);

    QList<QPoint> moves = Board::getInstance()->availableMoves(this);
    Board::getInstance()->showHints(moves);

    // Unselect previous piece
    if (selectedPiece && selectedPiece != this) {
        selectedPiece->setSelectedState(false);
    }
    // Toggle selection
    if (selectedPiece == this) {
        setSelectedState(false);
        selectedPiece = nullptr;
        Board::getInstance()->clearHints();
    } else {
        setSelectedState(true);
        selectedPiece = this;
    }

    QGraphicsSvgItem::mousePressEvent(event);
}


void ChessPiece::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    QPointF offset = event->scenePos() - dragStartPos;
    QPointF newPos = pos() + offset;

    const int minX = 0;
    const int minY = 0;
    const int maxX = 7 * Board::tileSize;
    const int maxY = 7 * Board::tileSize;

    qreal clampedX = std::clamp(newPos.x(), qreal(minX), qreal(maxX));
    qreal clampedY = std::clamp(newPos.y(), qreal(minY), qreal(maxY));

    setPos(clampedX, clampedY);
    dragStartPos = event->scenePos();
    setSelectedState(false);

    QGraphicsSvgItem::mouseMoveEvent(event);
}


void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    Board::getInstance()->clearHints();

    int x = int(event->scenePos().x()) / Board::tileSize;
    int y = int(event->scenePos().y()) / Board::tileSize;

    x = std::clamp(x, 0, 7);
    y = std::clamp(y, 0, 7);

    Q_ASSERT(Board::getInstance() != nullptr); //

    // befor move must be checked
    QPoint oldPos = getBoardPosition();
    Board::getInstance()->removePiece(oldPos.x(), oldPos.y());     // ❗ старую позицию очистить
    Board::getInstance()->movePiece(this, x, y);                  // ❗ поставить в новую
    setBoardPosition(QPoint(x, y));                 // ❗ обновить свою позицию

    setPos(x * Board::tileSize, y * Board::tileSize);
    setZValue(0);

    QGraphicsSvgItem::mouseReleaseEvent(event);
}

