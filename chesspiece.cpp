#include "promotiondialog.h"
#include "chesspiece.h"
#include "board.h"

#include <QDebug>
#include <QPixmap>
#include <QGraphicsColorizeEffect>
#include <QSvgRenderer>
#include <QFile>

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

ChessPiece::ChessPiece(const ChessPiece &other)
    : QGraphicsSvgItem() // Call base class copy constructor (important!)
{
    this->type = other.type;
    this->color = other.color;
    this->dragStartPos = other.dragStartPos;
    this->position = other.position;
    this->oldClickPosition = other.oldClickPosition;
    this->cachedMoves = other.cachedMoves;

    this->board = other.board;  // Shallow copy: both pieces point to the same board
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

    if (!Board::getInstance()->isCorrectTurn(this)) {
        // ❗ Снять выделение, если очередь не твоя
        setSelectedState(false);
        selectedPiece = nullptr;
        return;
    }

    cachedMoves  = Board::getInstance()->availableMoves(this);
    Board::getInstance()->showHints(cachedMoves );

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
    setSelectedState(false);

    QGraphicsSvgItem::mouseMoveEvent(event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    Board* board = Board::getInstance();

    if (!board->isCorrectTurn(this)) {
        qDebug() << "Not your turn!";
        // Откат назад
        QPoint oldBoardPos = getBoardPosition();
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
        return;
    }

    QPoint newPosInScene = event->scenePos().toPoint();
    int x = std::clamp(newPosInScene.x() / Board::tileSize, 0, 7);
    int y = std::clamp(newPosInScene.y() / Board::tileSize, 0, 7);
    QPoint newBoardPos(x, y);


    // if (Board::getInstance()->isMoveValid(getBoardPosition(), newBoardPos)) { //this way we call availableMoves twice
    if (cachedMoves.contains(newBoardPos)) {
        qDebug() << "Move is allowed";

        // Pawn Promotion
        if (type == PieceType::Pawn) {
            int finalRank = (getColor() == ChessPiece::White) ? 0 : 7;
            if (newBoardPos.y() == finalRank) {
                qDebug() << "become Queen";
                Color color = getColor();
                PromotionDialog dialog(color);
                if (dialog.exec() == QDialog::Accepted) {
                    ChessPiece::PieceType promotedType = dialog.getSelectedPieceType();
                    qDebug() << "pawn become to " << promotedType;

                    // ❗ Удаляем пешку и создаём новую фигуру
                    ChessPiece* pawn = board->pieceAt(position.x(), position.y());
                    if (!pawn) {
                        qWarning() << "❌ No piece at" << x << y;
                        return;
                    }

                    // Удалить старую пешку
                    board->getScene()->removeItem(pawn);
                    pawn->hide();
                    board->removePiece(position.x(), position.y());
                    //delete pawn;

                    board->capturePiece(x, y);
                    board->pawnPromotion(promotedType, color, x, finalRank);

                    board->switchTurn();  // передаём ход сразу
                    board->clearHints();
                    cachedMoves.clear();
                    setZValue(0);
                    return;  // ❗ Никаких дальнейших действий
                }
            }
        }

        // Попытка захвата вражеской фигуры
        if (board->isEnemy(x, y, this->getColor())) {
            board->capturePiece(x, y);  // 👈 Удаляет и заносит в список убитых
        }

        // Обновляем положение фигуры
        board->removePiece(getBoardPosition().x(), getBoardPosition().y());  // Удаляем со старой позиции
        board->movePiece(this, x, y);                                        // Регистрируем новую
        setBoardPosition(newBoardPos);                                       // Обновляем внутреннюю позицию
        setPos(x * Board::tileSize, y * Board::tileSize);                    // Перемещаем визуально

        Board::getInstance()->switchTurn();
    } else {
        qDebug() << "Move not allowed, snapping back";

        // Возврат в старую позицию
        QPoint oldBoardPos = getBoardPosition();
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
    }


    setZValue(0);
    board->clearHints();
    cachedMoves.clear();


    QGraphicsSvgItem::mouseReleaseEvent(event);
}

