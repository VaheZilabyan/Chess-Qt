#include "promotiondialog.h"
#include "chesspiece.h"
#include "board.h"
#include "sound.h"

#include <QDebug>
#include <QPixmap>
#include <QGraphicsColorizeEffect>
#include <QSvgRenderer>
#include <QFile>
#include <QMessageBox>

// need to move sounds ant other methods from mouseReleaseEven to movePiece or other method
// because after stockfish moves we doesnt see highlights or sounds effects

ChessPiece* ChessPiece::selectedPiece = nullptr;

ChessPiece::ChessPiece(PieceType type, Color color, const QString& svgPath)
    : QGraphicsSvgItem(svgPath), type(type), color(color), svgPath(svgPath)
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

    if (!Board::getInstance()->isCorrectTurn(this)) {
        // ❗ Снять выделение, если очередь не твоя
        setSelectedState(false);
        selectedPiece = nullptr;
        return;
    }

    dragStartPos = event->scenePos();
    setZValue(1);  // Bring to front

    position = toBoardCoord(event->scenePos()); //do the same

    cachedMoves = board->legalMoves(this);

    board->showHints(cachedMoves );

    // Unselect previous piece
    if (selectedPiece && selectedPiece != this) {
        selectedPiece->setSelectedState(false);
    }
    // Toggle selection
    if (selectedPiece == this) {
        setSelectedState(false);
        selectedPiece = nullptr;
        board->clearHints();
    } else {
        setSelectedState(true);
        selectedPiece = this;
    }

    QGraphicsSvgItem::mousePressEvent(event);
}


void ChessPiece::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {

    if (!Board::getInstance()->isCorrectTurn(this)) {
        // ❗ Снять выделение, если очередь не твоя
        setSelectedState(false);
        selectedPiece = nullptr;
        return;
    }

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
    board->showHints(cachedMoves );
    setSelectedState(true);

    QGraphicsSvgItem::mouseMoveEvent(event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    Board* board = Board::getInstance();

    QPoint oldBoardPos = getPositionFromBoard();
    if (!board->isCorrectTurn(this)) {
        qDebug() << "Not your turn!";
        // Откат назад
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
        return;
    }

    QPoint newPosInScene = event->scenePos().toPoint();
    int x = std::clamp(newPosInScene.x() / Board::tileSize, 0, 7);
    int y = std::clamp(newPosInScene.y() / Board::tileSize, 0, 7);
    QPoint newBoardPos(x, y);

    if (cachedMoves.contains(newBoardPos)) {
        //qDebug() << "Move is allowed";
        board->makeMove(this, oldBoardPos, newBoardPos, true); //true - player

    } else {
        //qDebug() << "Move not allowed, snapping back";
        // Возврат в старую позицию
        QPoint oldBoardPos = getPositionFromBoard();
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
    }


    QGraphicsSvgItem::mouseReleaseEvent(event);
}
