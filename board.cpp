#include "board.h"

/*Board* getInstance(QObject* parent = nullptr) {
    static Board instance(parent);
    return &instance;
}*/

void Board::addPiece(ChessPiece::PieceType type, ChessPiece::Color color, int x, int y)
{
    QString path = getSvgPath(type, color);
    ChessPiece* piece = new ChessPiece(type, color, path);
    piece->setScale(tileSize / 128.0);
    piece->setPos(x * tileSize, y * tileSize);
    piece->setPositionOnTheBoard(QPoint(x, y));
    scene->addItem(piece);
    pieces[y][x] = piece;
    piece->setZValue(1);

    // Обновляем позицию короля
    if (type == ChessPiece::King) {
        if (color == ChessPiece::White)
            whiteKingPos = QPoint(x, y);
        else
            blackKingPos = QPoint(x, y);
    }
}

QString Board::getSvgPath(ChessPiece::PieceType type, ChessPiece::Color color)
{
    QString base = ":/svg_files/img/";
    QString name;

    switch (type) {
    case ChessPiece::King:   name = "king"; break;
    case ChessPiece::Queen:  name = "queen"; break;
    case ChessPiece::Rook:   name = "rook"; break;
    case ChessPiece::Bishop: name = "bishop"; break;
    case ChessPiece::Knight: name = "knight"; break;
    case ChessPiece::Pawn:   name = "pawn"; break;
    }

    name += (color == ChessPiece::White) ? "-w.svg" : "-b.svg";
    return base + name;
}

void Board::resetBoard()    //calls for new game
{
    // 1. Удаляем все фигуры с доски и со сцены
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            if (pieces[y][x]) {
                scene->removeItem(pieces[y][x]);
                delete pieces[y][x];
                pieces[y][x] = nullptr;
            }
        }
    }
    // 2. Сброс данных
    hintDots.clear();
    killedWhitePieces.clear();
    killedBlackPieces.clear();
    whiteKingPos = QPoint(4, 7);
    blackKingPos = QPoint(4, 0);
    enPassantTarget = QPoint(-1, -1);
    currentTurn = ChessPiece::White;
    ChessPiece::setSelectedPiece(nullptr);

    clearHints();
    setupInitialPosition();

    qDebug() << "Board reset completed.";
}

void Board::setupBoard() {
    // Перед перерисовкой доски:
    for (QGraphicsItem* item : scene->items()) {
        // Удаляем только клетки (те, у кого ZValue == 0)
        if (item->zValue() == 0) {
            scene->removeItem(item);
            delete item;
        }
    }

    // Рисуем доску
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QColor color = (row + col) % 2 == 0 ? boardColor.first : boardColor.second;
            QGraphicsRectItem* square = scene->addRect(col * tileSize, row * tileSize, tileSize, tileSize, QPen(Qt::NoPen), QBrush(color));
            square->setData(0, row);
            square->setData(1, col);
            square->setZValue(0);
        }
    }

    // Текстовые координаты
    for (int i = 0; i < 8; ++i) {
        //QColor color = i % 2 == 0 ? Qt::white : Qt::black;
        scene->addText(QString(QChar(97 + i)), QFont("Arial", 8, QFont::Bold, true))->setPos(i * tileSize - 3, 7 * tileSize + tileSize / 1.35);
        scene->addText(QString::number(8 - i), QFont("Arial", 8, QFont::Bold, true))->setPos(7 * tileSize + tileSize / 1.22, i * tileSize);
    }
}

void Board::setupInitialPosition()
{
    setupBoard();

    // Пешки
    for (int col = 0; col < 8; ++col) {
        addPiece(ChessPiece::Pawn, ChessPiece::White, col, 6);
        addPiece(ChessPiece::Pawn, ChessPiece::Black, col, 1);
    }

    // Остальные фигуры
    const ChessPiece::PieceType order[8] = {
        ChessPiece::Rook, ChessPiece::Knight, ChessPiece::Bishop,
        ChessPiece::Queen, ChessPiece::King,
        ChessPiece::Bishop, ChessPiece::Knight, ChessPiece::Rook
    };

    for (int col = 0; col < 8; ++col) {
        addPiece(order[col], ChessPiece::White, col, 7);
        addPiece(order[col], ChessPiece::Black, col, 0);
    }
}


QList<QPoint> Board::legalMoves(ChessPiece* piece)
{
    QList<QPoint> moves = availableMoves(piece);
    QList<QPoint> safeMoves;

    QPoint originalPos = piece->getPositionFromBoard();
    ChessPiece* originalTarget = nullptr;

    if (piece->getType() == ChessPiece::King) {
        for (const QPoint& move : moves) {
            int x = move.x();
            int y = move.y();

            // Временный ход
            originalTarget = pieces[y][x];
            originalPos = piece->getPositionFromBoard();

            pieces[originalPos.y()][originalPos.x()] = nullptr;
            pieces[y][x] = piece;
            piece->setPositionOnTheBoard(move);

            // 👇 Новый: проверка, не атакуется ли поле
            if (!isSquareAttacked(move, piece->getColor())) {
                safeMoves.append(move);
            }

            // Откат
            pieces[y][x] = originalTarget;
            pieces[originalPos.y()][originalPos.x()] = piece;
            piece->setPositionOnTheBoard(originalPos);
        }

        return safeMoves; // досрочно для короля
    }

    for (const QPoint& move : moves) {
        int x = move.x();
        int y = move.y();

        // --- Сохраняем состояние ---
        originalTarget = pieces[y][x];
        pieces[originalPos.y()][originalPos.x()] = nullptr;
        pieces[y][x] = piece;
        piece->setPositionOnTheBoard(move);

        // --- Проверка шаха ---
        if (!isKingInCheck(piece->getColor())) {
            safeMoves.append(move);
        }

        // --- Откат ---
        pieces[originalPos.y()][originalPos.x()] = piece;
        pieces[y][x] = originalTarget;
        piece->setPositionOnTheBoard(originalPos);
    }

    return safeMoves;
}

QList<QPoint> Board::availableMoves(ChessPiece* piece) const {
    QList<QPoint> moves;
    ChessPiece::PieceType pieceType = piece->getType();

    switch (pieceType) {
    case ChessPiece::Pawn: {
        int dir = (piece->getColor() == ChessPiece::White) ? -1 : 1;
        QPoint pos = piece->getPositionFromBoard();  // например (4, 6)
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
        // Удары по диагонали   //implement after isEnemy method
        for (int dx : {-1, 1}) {
            int targetX = x + dx;
            int targetY = y;

            if (isEnemy(targetX, targetY, piece->getColor())) {
                moves.append(QPoint(targetX, targetY));
            }
        }

        // ВЗЯТИЕ НА ПРОХОДЕ
        QPoint enPassantTarget = getEnPassantTarget();
        for (int dx : {-1, 1}) {
            int adjX = pos.x() + dx;
            int targetY = pos.y() + dir;
            if (QPoint(adjX, targetY) == enPassantTarget) {
                moves.append(enPassantTarget);
            }
        }
        break;
    }
    case ChessPiece::Knight: {
        static const QList<QPoint> knightMoves = {
            {1, 2}, {2, 1}, {2, -1}, {1, -2},
            {-1, -2}, {-2, -1}, {-2, 1}, {-1, 2}
        };

        QPoint pos = piece->getPositionFromBoard();
        int x = pos.x();
        int y = pos.y();

        for (const QPoint& move : knightMoves) {
            int nx = x + move.x();
            int ny = y + move.y();

            // Проверка границ доски    //isEnemy not impl yet
            if (isInsideBoard(nx, ny)) {
                if (isEmpty(nx, ny) || isEnemy(nx, ny, piece->getColor())) {
                    moves.append(QPoint(nx, ny));
                }
            }
        }
        break;
    }
    case ChessPiece::Bishop: {
        static const QList<QPoint> bishopDirs = {
            {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
        };

        QPoint pos = piece->getPositionFromBoard();
        int x0 = pos.x();
        int y0 = pos.y();
        ChessPiece::Color color = piece->getColor();

        for (const QPoint& dir : bishopDirs) {
            int x = x0 + dir.x();
            int y = y0 + dir.y();

            while (isInsideBoard(x, y)) {
                if (isEmpty(x, y)) {
                    moves.append(QPoint(x, y));
                } else {
                    if (isEnemy(x, y, color)) {
                        moves.append(QPoint(x, y)); // можно побить
                    }
                    break; // путь прерван
                }
                x += dir.x();
                y += dir.y();
            }
        }
        break;
    }
    case ChessPiece::Rook: {
        static const QList<QPoint> rookDirs = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1}  // вправо, влево, вниз, вверх
        };

        QPoint pos = piece->getPositionFromBoard();
        int x0 = pos.x();
        int y0 = pos.y();
        ChessPiece::Color color = piece->getColor();

        for (const QPoint& dir : rookDirs) {
            int x = x0 + dir.x();
            int y = y0 + dir.y();

            while (isInsideBoard(x, y)) {
                if (isEmpty(x, y)) {
                    moves.append(QPoint(x, y));
                } else {
                    if (isEnemy(x, y, color)) {
                        moves.append(QPoint(x, y)); // можно побить
                    }
                    break; // путь прерван
                }

                x += dir.x();
                y += dir.y();
            }
        }
        break;
    }
    case ChessPiece::Queen: {
        static const QList<QPoint> queenDirs = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
        };

        QPoint pos = piece->getPositionFromBoard();
        int x0 = pos.x();
        int y0 = pos.y();
        ChessPiece::Color color = piece->getColor();

        for (const QPoint& dir : queenDirs) {
            int x = x0 + dir.x();
            int y = y0 + dir.y();

            while (isInsideBoard(x, y)) {
                if (isEmpty(x, y)) {
                    moves.append(QPoint(x, y));
                } else {
                    if (isEnemy(x, y, color)) {
                        moves.append(QPoint(x, y)); // можно побить
                    }
                    break; // путь прерван
                }

                x += dir.x();
                y += dir.y();
            }
        }
        break;
    }
    case ChessPiece::King: {
        static const QList<QPoint> kingDirs = {
            {1, 0}, {-1, 0}, {0, 1}, {0, -1},
            {1, 1}, {-1, 1}, {-1, -1}, {1, -1}
        };

        QPoint pos = piece->getPositionFromBoard();
        int x0 = pos.x();
        int y0 = pos.y();
        //ChessPiece::Color color = piece->getColor();

        for (const QPoint& dir : kingDirs) {
            int x = x0 + dir.x();
            int y = y0 + dir.y();

            if (isInsideBoard(x, y)) {
                if (isEmpty(x, y) || isEnemy(x, y, piece->getColor())) {
                    moves.append(QPoint(x, y));
                }
            }
        }
        // castling add later
        if (piece->getType() == ChessPiece::King && !piece->hasMovedAlready()) {
            int row = (piece->getColor() == ChessPiece::White) ? 7 : 0;

            // КОРОТКАЯ РОКИРОВКА (король вправо)
            ChessPiece* rookShort = pieceAt(7, row);
            if (rookShort && rookShort->getType() == ChessPiece::Rook &&
                !rookShort->hasMovedAlready() &&
                isEmpty(5, row) && isEmpty(6, row) &&
                !isSquareAttacked(QPoint(4, row), piece->getColor()) &&
                !isSquareAttacked(QPoint(5, row), piece->getColor()) &&
                !isSquareAttacked(QPoint(6, row), piece->getColor())) {

                moves.append(QPoint(6, row)); // Король пойдёт на g1/g8
            }

            // ДЛИННАЯ РОКИРОВКА (король влево)
            ChessPiece* rookLong = pieceAt(0, row);
            if (rookLong && rookLong->getType() == ChessPiece::Rook &&
                !rookLong->hasMovedAlready() &&
                isEmpty(1, row) && isEmpty(2, row) && isEmpty(3, row) &&
                !isSquareAttacked(QPoint(4, row), piece->getColor()) &&
                !isSquareAttacked(QPoint(3, row), piece->getColor()) &&
                !isSquareAttacked(QPoint(2, row), piece->getColor())) {

                moves.append(QPoint(2, row)); // Король пойдёт на c1/c8
            }
        }


        break;
    }
    }

    return moves;
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
        dot->setZValue(1);
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

ChessPiece *Board::pieceAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    return pieces[y][x]; // по стандарту y — строка, x — столбец
}

bool Board::isMoveValid(QPoint from, QPoint to) const
{
    if (from.x() < 0 || from.x() >= 8 || from.y() < 0 || from.y() >= 8 ||
        to.x() < 0 || to.x() >= 8 || to.y() < 0 || to.y() >= 8) {
        return false;
    }

    ChessPiece* piece = pieces[from.y()][from.x()];
    if (!piece) return false;

    QList<QPoint> moves = availableMoves(piece); //legalMove?
    return moves.contains(QPoint(to.x(), to.y()));
}

bool Board::isCorrectTurn(ChessPiece* piece) const {
    return piece && piece->getColor() == currentTurn;
}

void Board::switchTurn() {
    currentTurn = (currentTurn == ChessPiece::White) ? ChessPiece::Black : ChessPiece::White;
    if (clock) {
        if (currentTurn == ChessPiece::White)
            clock->startWhite();
        else
            clock->startBlack();
    }
}

bool Board::isEmpty(int x, int y) const
{
    if (pieceAt(x, y) == nullptr) return true;
    return false;
}

bool Board::isEnemy(int x, int y, ChessPiece::Color color) const
{
    ChessPiece *enemy = pieceAt(x, y);
    if (isInsideBoard(x, y) && enemy && enemy->getColor() != color) {
        return true;
    }
    return false;
}

void Board::capturePiece(int x, int y)
{
    ChessPiece* enemy = pieces[y][x];
    if (!enemy) return;

    if (enemy->getColor() == ChessPiece::White) {
        killedWhitePieces.append(enemy);
        //addToGraveyard(enemy, true);
    } else {
        killedBlackPieces.append(enemy);
        //addToGraveyard(enemy, false);
    }

    emit pieceCaptured(enemy);

    scene->removeItem(enemy);
    enemy->hide();
    pieces[y][x] = nullptr;

}

void Board::movePiece(ChessPiece *piece, int x, int y)
{
    QPoint from = piece->getPositionFromBoard();

    pieces[y][x] = piece;
    piece->setPositionOnTheBoard(QPoint(x, y));
    piece->setPos(x * Board::tileSize, y * Board::tileSize);

    pieces[from.y()][from.x()] = nullptr;

    // En Passant логика:
    if (piece->getType() == ChessPiece::Pawn &&
        abs(y - from.y()) == 2) {
        int dir = (piece->getColor() == ChessPiece::White) ? 1 : -1;
        enPassantTarget = QPoint(x, y + dir);
    } else {
        enPassantTarget = {-1, -1}; // сбрасываем, если не 2-клеточный ход пешки
    }

    if (piece->getType() == ChessPiece::King) {
        if (piece->getColor() == ChessPiece::White)
            whiteKingPos = QPoint(x, y);
        else
            blackKingPos = QPoint(x, y);
    }

    if (piece->getType() == ChessPiece::King || piece->getType() == ChessPiece::Rook) {
        piece->markAsMoved();
    }
}

ChessPiece *Board::pawnPromotion(ChessPiece::PieceType newType, ChessPiece::Color color) {
    // Заменить новой фигурой
    QString svgPath;
    if (color == ChessPiece::White) {
        if (newType == ChessPiece::Queen)  svgPath = ":/svg_files/img/queen-w.svg";
        if (newType == ChessPiece::Rook)   svgPath = ":/svg_files/img/rook-w.svg";
        if (newType == ChessPiece::Bishop) svgPath = ":/svg_files/img/bishop-w.svg";
        if (newType == ChessPiece::Knight) svgPath = ":/svg_files/img/knight-w.svg";
    } else {
        if (newType == ChessPiece::Queen)  svgPath = ":/svg_files/img/queen-b.svg";
        if (newType == ChessPiece::Rook)   svgPath = ":/svg_files/img/rook-b.svg";
        if (newType == ChessPiece::Bishop) svgPath = ":/svg_files/img/bishop-b.svg";
        if (newType == ChessPiece::Knight) svgPath = ":/svg_files/img/knight-b.svg";
    }

    ChessPiece* promoted = new ChessPiece(newType, color, svgPath);
    promoted->setScale(tileSize / 128.0);
    scene->addItem(promoted);
    return promoted;
}

bool Board::isSquareAttacked(QPoint pos, ChessPiece::Color byColor) const
{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = pieces[y][x];
            if (piece && piece->getColor() != byColor) {
                QList<QPoint> attacks = availableMoves(piece); // Без фильтрации по шаху
                if (attacks.contains(pos)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Board::isKingInCheck(ChessPiece::Color color)
{
    QPoint kingPos = (color == ChessPiece::White) ? whiteKingPos : blackKingPos;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = pieces[y][x];
            if (piece && piece->getColor() != color) {
                QList<QPoint> enemyMoves = availableMoves(piece);
                if (enemyMoves.contains(kingPos)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool Board::isCheckmate(ChessPiece::Color color)
{
    if (!isKingInCheck(color))
        return false; // если нет шаха — не может быть мат

    // Проходим по всем фигурам этого цвета
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = pieces[y][x];
            if (piece && piece->getColor() == color) {
                QList<QPoint> moves = legalMoves(piece);
                if (!moves.isEmpty()) {
                    return false; // хоть один ход есть → не мат
                }
            }
        }
    }

    return true; // и шах, и ходов нет → мат
}

bool Board::isStalemate(ChessPiece::Color color)
{
    if (isKingInCheck(color))
        return false; // если шах — это не пат

    // Проверяем все фигуры игрока
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = pieces[y][x];
            if (piece && piece->getColor() == color) {
                QList<QPoint> moves = legalMoves(piece);
                if (!moves.isEmpty()) {
                    return false; // есть хоть один ход — не пат
                }
            }
        }
    }

    return true; // ни одного хода и нет шаха → пат
}

void Board::setBoardColor(QPair<QColor, QColor> bc) {
    boardColor = bc;
    setupBoard();
}
