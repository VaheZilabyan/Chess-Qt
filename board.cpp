#include "board.h"

Board *Board::getInstance(QGraphicsScene *s)
{
    static Board instance(s);
    return &instance;
}

void Board::setupInitialPosition()
{
    // print Board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QColor color = (row + col) % 2 == 0 ? Qt::white : Qt::gray;
            QGraphicsRectItem* square = scene->addRect(col * tileSize, row * tileSize, tileSize, tileSize, QPen(Qt::NoPen), QBrush(color));
            square->setData(0, row);
            square->setData(1, col);
        }
    }
    QGraphicsRectItem *square = scene->addRect(0, 0, 8 * tileSize, 8 * tileSize, QPen(Qt::gray));

    int num = 97;
    for (int i = 0; i < 8; ++i) {
        QColor color = i % 2 == 0 ? Qt::white : Qt::darkGray;
        QGraphicsTextItem *text = scene->addText(QString(QChar(num++)), QFont("Arial", 8, QFont::Bold, true));
        text->setDefaultTextColor(color);
        text->setPos(i * tileSize - 3, 7 * tileSize + tileSize / 1.35);

    }
    for (int i = 7; i >= 0; --i) {
        QColor color = i % 2 == 0 ? Qt::white : Qt::darkGray;
        QGraphicsTextItem *text = scene->addText(QString::number(8 - i), QFont("Arial", 8, QFont::Bold, true));
        text->setDefaultTextColor(color);
        text->setPos(7 * tileSize + tileSize/1.22, i * tileSize);
    }

    QMap<QString, QString> svgPaths = {
                                       {"wp", ":/svg_files/img/pawn-w.svg"},
                                       {"wr", ":/svg_files/img/rook-w.svg"},
                                       {"wn", ":/svg_files/img/knight-w.svg"},
                                       {"wb", ":/svg_files/img/bishop-w.svg"},
                                       {"wq", ":/svg_files/img/queen-w.svg"},
                                       {"wk", ":/svg_files/img/king-w.svg"},

                                       {"bp", ":/svg_files/img/pawn-b.svg"},
                                       {"br", ":/svg_files/img/rook-b.svg"},
                                       {"bn", ":/svg_files/img/knight-b.svg"},
                                       {"bb", ":/svg_files/img/bishop-b.svg"},
                                       {"bq", ":/svg_files/img/queen-b.svg"},
                                       {"bk", ":/svg_files/img/king-b.svg"},
                                       };

    // Пешки
    for (int col = 0; col < 8; ++col) {
        ChessPiece* wp = new ChessPiece(ChessPiece::Pawn, ChessPiece::White, svgPaths["wp"]); //
        wp->setPos(col * tileSize, 6 * tileSize);
        wp->setScale(tileSize / 128.0);  // подстройка масштаба
        scene->addItem(wp);
        whitePieces.append(wp);  // если whitePieces теперь QVector<QGraphicsSvgItem*>

        ChessPiece* bp = new ChessPiece(ChessPiece::Pawn, ChessPiece::White, svgPaths["bp"]);
        bp->setPos(col * tileSize, 1 * tileSize);
        bp->setScale(tileSize / 128.0);
        scene->addItem(bp);
        blackPieces.append(bp);
    }

    // Расстановка других фигур по стандарту
    const ChessPiece::PieceType order[8] = {
        ChessPiece::Rook, ChessPiece::Knight, ChessPiece::Bishop,
        ChessPiece::Queen, ChessPiece::King,
        ChessPiece::Bishop, ChessPiece::Knight, ChessPiece::Rook
    };

    const QString whiteKeys[8] = { "wr", "wn", "wb", "wq", "wk", "wb", "wn", "wr" };
    const QString blackKeys[8] = { "br", "bn", "bb", "bq", "bk", "bb", "bn", "br" };

    for (int col = 0; col < 8; ++col) {
        ChessPiece* wPiece = new ChessPiece(order[col], ChessPiece::White, svgPaths[whiteKeys[col]]);
        wPiece->setPos(col * tileSize, 7 * tileSize);
        wPiece->setScale(tileSize / 128.0);
        scene->addItem(wPiece);
        whitePieces.append(wPiece);

        ChessPiece* bPiece = new ChessPiece(order[col], ChessPiece::White, svgPaths[blackKeys[col]]);
        bPiece->setPos(col * tileSize, 0 * tileSize);
        bPiece->setScale(tileSize / 128.0);
        scene->addItem(bPiece);
        blackPieces.append(bPiece);
    }
}

QList<QPoint> Board::availableMoves(ChessPiece* piece) const {
    QList<QPoint> moves;

    if (piece->getType() == ChessPiece::Pawn) {
        int dir = (piece->getColor() == ChessPiece::White) ? -1 : 1;
        QPoint pos = piece->getBoardPosition();  // например (4, 6)

        int x = pos.x();
        int y = pos.y() + dir;

        if (isEmpty(x, y)) {
            moves.append(QPoint(x, y));
            // если на начальной позиции — может идти на 2 клетки
            if ((piece->getColor() == ChessPiece::White && pos.y() == 6) ||
                (piece->getColor() == ChessPiece::Black && pos.y() == 1)) {
                if (isEmpty(x, y + dir)) {
                    moves.append(QPoint(x, y + dir));
                }
            }
        }

        // Удары по диагонали
        for (int dx : {-1, 1}) {
            if (isEnemy(x + dx, y, piece->getColor()))
                moves.append(QPoint(x + dx, y));
        }
    }

    return moves;
}

bool Board::isEmpty(int x, int y) const
{
    return true;
}

bool Board::isEnemy(int, int, ChessPiece::Color) const
{
    return false;
}

void Board::showHints(const QList<QPoint>& moves) {
    clearHints();

    for (const QPoint& pos : moves) {
        QGraphicsEllipseItem* dot = scene->addEllipse(
            pos.x() * tileSize + tileSize / 3,
            pos.y() * tileSize + tileSize / 3,
            tileSize / 3, tileSize / 3,
            QPen(Qt::NoPen),
            QBrush(QColor(50, 150, 255, 150))
            );
        hintDots.append(dot);
    }
}

void Board::clearHints() {
    for (auto dot : hintDots) {
        scene->removeItem(dot);
        delete dot;
    }
    hintDots.clear();
}
