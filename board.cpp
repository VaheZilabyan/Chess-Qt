#include "board.h"

QVector<ChessPiece*> Board::whitePieces = {};
QVector<ChessPiece*> Board::blackPieces = {};

Board *Board::getInstance()
{
    static Board instance;
    return &instance;
}

void Board::setupInitialPosition(QGraphicsScene *scene)
{
    // print Board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QColor color = (row + col) % 2 == 0 ? Qt::white : Qt::gray;
            QGraphicsRectItem* square = scene->addRect(col * tileSize, row * tileSize, tileSize, tileSize, QPen(Qt::black), QBrush(color));
            square->setData(0, row);
            square->setData(1, col);
        }
    }

    int num = 97;
    for (int i = 0; i < 8; ++i) {
        QGraphicsTextItem *text = scene->addText(QString(QChar(num++)), QFont("Arial", 9, QFont::Bold, true));
        text->setPos(i * tileSize - 3, 7 * tileSize + tileSize/1.27);
    }
    for (int i = 7; i >= 0; --i) {
        QGraphicsTextItem *text = scene->addText(QString::number(8 - i), QFont("Arial", 9, QFont::Bold, true));
        text->setPos(7 * tileSize + tileSize/1.2, i * tileSize);
    }

    QMap<QString, QPixmap> pixmaps = {
        {"wp", QPixmap(":/res/images/white-pawn.png").scaled(tileSize, tileSize)},
        {"wr", QPixmap(":/res/images/white-rook.png").scaled(tileSize, tileSize)},
        {"wn", QPixmap(":/res/images/white-knight.png").scaled(tileSize, tileSize)},
        {"wb", QPixmap(":/res/images/white-bishop.png").scaled(tileSize, tileSize)},
        {"wq", QPixmap(":/res/images/white-queen.png").scaled(tileSize, tileSize)},
        {"wk", QPixmap(":/res/images/white-king.png").scaled(tileSize, tileSize)},

        {"bp", QPixmap(":/res/images/black-pawn.png").scaled(tileSize, tileSize)},
        {"br", QPixmap(":/res/images/black-rook.png").scaled(tileSize, tileSize)},
        {"bn", QPixmap(":/res/images/black-knight.png").scaled(tileSize, tileSize)},
        {"bb", QPixmap(":/res/images/black-bishop.png").scaled(tileSize, tileSize)},
        {"bq", QPixmap(":/res/images/black-queen.png").scaled(tileSize, tileSize)},
        {"bk", QPixmap(":/res/images/black-king.png").scaled(tileSize, tileSize)},
    };

    // Пешки
    for (int col = 0; col < 8; ++col) {
        ChessPiece* wp = new ChessPiece(ChessPiece::Pawn, ChessPiece::White, pixmaps["wp"]);
        wp->setPos(col * tileSize, 6 * tileSize);
        scene->addItem(wp);
        whitePieces.append(wp);

        ChessPiece* bp = new ChessPiece(ChessPiece::Pawn, ChessPiece::Black, pixmaps["bp"]);
        bp->setPos(col * tileSize, 1 * tileSize);
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
        // Белые
        ChessPiece* wPiece = new ChessPiece(order[col], ChessPiece::White, pixmaps[whiteKeys[col]]);
        wPiece->setPos(col * tileSize, 7 * tileSize);
        scene->addItem(wPiece);
        whitePieces.append(wPiece);

        // Чёрные
        ChessPiece* bPiece = new ChessPiece(order[col], ChessPiece::Black, pixmaps[blackKeys[col]]);
        bPiece->setPos(col * tileSize, 0 * tileSize);
        scene->addItem(bPiece);
        blackPieces.append(bPiece);
    }
}
