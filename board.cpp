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

    QMap<QString, QPixmap> pixmaps = {
        {"wp", QPixmap(":/res/images/white-pawn.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"wr", QPixmap(":/res/images/white-rook.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"wn", QPixmap(":/res/images/white-knight.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"wb", QPixmap(":/res/images/white-bishop.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"wq", QPixmap(":/res/images/white-queen.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"wk", QPixmap(":/res/images/white-king.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},

        {"bp", QPixmap(":/res/images/black-pawn.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"br", QPixmap(":/res/images/black-rook.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"bn", QPixmap(":/res/images/black-knight.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"bb", QPixmap(":/res/images/black-bishop.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"bq", QPixmap(":/res/images/black-queen.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
        {"bk", QPixmap(":/res/images/black-king.png").scaled(tileSize, tileSize, Qt::KeepAspectRatio, Qt::SmoothTransformation)},
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
