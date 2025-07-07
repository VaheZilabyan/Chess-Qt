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
    board = Board::getInstance();
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

QPoint ChessPiece::toBoardCoord(QPointF scenePos)
{
    int x = static_cast<int>(scenePos.x()) / Board::tileSize;
    int y = static_cast<int>(scenePos.y()) / Board::tileSize;
    return QPoint(x, y);
}

void ChessPiece::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    Q_UNUSED(event);

    dragStartPos = event->scenePos();
    setZValue(1);  // Bring to front
    oldClickPosition = dragStartPos;

    /*int cellSize = 64;  // размер одной клетки в пикселях (задай свой)
    int col = static_cast<int>(scenePos().x()) / cellSize;  // номер колонки 0..7
    int row = static_cast<int>(scenePos().y()) / cellSize;  // номер строки 0..7
    position.setX(col);
    position.setY(row);*/
    position = toBoardCoord(event->scenePos()); //do the same

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
        //Board::getInstance()->clearHints();
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
    Board* board = Board::getInstance();

    QPoint newPosInScene = event->scenePos().toPoint();
    int x = std::clamp(newPosInScene.x() / Board::tileSize, 0, 7);
    int y = std::clamp(newPosInScene.y() / Board::tileSize, 0, 7);
    QPoint newBoardPos(x, y);

    QList<QPoint> moves = board->availableMoves(this);
    board->showHints(moves);  // show hints on click

    bool canMove = moves.contains(newBoardPos);

    //if (canMove) {
    if (Board::getInstance()->isMoveValid(getBoardPosition(), newBoardPos)) {
        qDebug() << "✅ Move is allowed";

        board->removePiece(getBoardPosition().x(), getBoardPosition().y());     // Remove from old pos
        board->movePiece(this, x, y);                                           // Register new position in the board array
        setBoardPosition(newBoardPos);                                          // Update internal state
        setPos(x * Board::tileSize, y * Board::tileSize);                       // Move visually
    } else {
        qDebug() << "❌ Move not allowed, snapping back";

        QPoint oldBoardPos = getBoardPosition();
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
    }

    setZValue(0);
    board->clearHints();

    QGraphicsSvgItem::mouseReleaseEvent(event);
}

