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
    Board::getInstance()->showHints(cachedMoves );
    setSelectedState(true);

    QGraphicsSvgItem::mouseMoveEvent(event);
}

void ChessPiece::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    Board* board = Board::getInstance();

    if (!board->isCorrectTurn(this)) {
        qDebug() << "Not your turn!";
        // Откат назад
        QPoint oldBoardPos = getPositionFromBoard();
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
        return;
    }

    QPoint newPosInScene = event->scenePos().toPoint();
    int x = std::clamp(newPosInScene.x() / Board::tileSize, 0, 7);
    int y = std::clamp(newPosInScene.y() / Board::tileSize, 0, 7);
    QPoint newBoardPos(x, y);

    bool moveAllowed = false;
    ChessPiece *promoted = nullptr;

    if (cachedMoves.contains(newBoardPos)) {
        qDebug() << "Move is allowed";

        // Попытка захвата вражеской фигуры
        if (board->isEnemy(x, y, this->getColor())) {
            board->capturePiece(x, y);  // 👈 Удаляет и заносит в список убитых
        }

        // En Passant - взятие на проходе
        if (type == PieceType::Pawn && newBoardPos == board->getEnPassantTarget()) {
            int dy = (getColor() == ChessPiece::White) ? 1 : -1;
            QPoint enemyPos(newBoardPos.x(), newBoardPos.y() + dy);
            board->capturePiece(enemyPos.x(), enemyPos.y());
        }

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
                    promoted = board->pawnPromotion(promotedType, color, x, finalRank);
                }
            }
        }

        ChessPiece *newPiece = (promoted == nullptr) ? this : promoted;
        board->movePiece(newPiece, x, y);
        //newPiece->setPos(x * Board::tileSize, y * Board::tileSize);

        // Удаляем старую пешку, если промоция
        if (promoted != nullptr) {
            this->deleteLater();  // безопасное удаление в Qt
        }

        moveAllowed = true;
        Board::getInstance()->switchTurn();
    } else {
        qDebug() << "Move not allowed, snapping back";
        // Возврат в старую позицию
        QPoint oldBoardPos = getPositionFromBoard();
        setPos(oldBoardPos.x() * Board::tileSize, oldBoardPos.y() * Board::tileSize);
    }


    setZValue(0);
    cachedMoves.clear();
    setSelectedState(false);
    if (promoted) {
        promoted->setSelectedState(false);
        selectedPiece = promoted;
    }
    if (moveAllowed) {
        board->clearHints();
    }

    QGraphicsSvgItem::mouseReleaseEvent(event);
}
