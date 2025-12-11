#include "board.h"
#include "sound.h"
#include "promotiondialog.h"

#include <QMessageBox>

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
    whiteKingPos = QPoint(4, 7);
    blackKingPos = QPoint(4, 0);
    enPassantTarget = QPoint(-1, -1);
    currentTurn = ChessPiece::White;
    ChessPiece::setSelectedPiece(nullptr);

    clearHints();
    //setupInitialPosition();
    moveHistory.clear();

    qDebug() << "Board reset completed.";
}

void Board::setupBoard() {
    Sound::instance().playStartSound();
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

    for (const QPoint& move : moves) {
        int x = move.x();
        int y = move.y();

        // Сохраняем цель
        originalTarget = pieces[y][x];

        // Двигаем фигуру
        pieces[originalPos.y()][originalPos.x()] = nullptr;
        pieces[y][x] = piece;
        piece->setPositionOnTheBoard(move);

        bool isSafe = false;
        if (piece->getType() == ChessPiece::King) {
            isSafe = !isSquareAttacked(move, piece->getColor());
        } else {
            isSafe = !isKingInCheck(piece->getColor());
        }

        if (isSafe)
            safeMoves.append(move);

        // Откат
        pieces[y][x] = originalTarget;
        pieces[originalPos.y()][originalPos.x()] = piece;
        piece->setPositionOnTheBoard(originalPos);
    }

    return safeMoves;
}


QList<QPoint> Board::rawAvailableMoves(ChessPiece* piece) const {
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

        for (const QPoint& dir : kingDirs) {
            int x = x0 + dir.x();
            int y = y0 + dir.y();

            if (isInsideBoard(x, y) &&
                (isEmpty(x, y) || isEnemy(x, y, piece->getColor()))) {
                moves.append(QPoint(x, y));
            }
        }

        break;
    }
    }

    return moves;
}

QList<QPoint> Board::availableMoves(ChessPiece* piece) const {
    QList<QPoint> moves = rawAvailableMoves(piece);

    if (piece->getType() == ChessPiece::King && !piece->hasMovedAlready()) {
        int row = (piece->getColor() == ChessPiece::White) ? 7 : 0;

        // короткая рокировка
        ChessPiece* rookShort = pieceAt(7, row);
        if (rookShort && rookShort->getType() == ChessPiece::Rook &&
            !rookShort->hasMovedAlready() &&
            isEmpty(5, row) && isEmpty(6, row) &&
            !isSquareAttacked(QPoint(4, row), piece->getColor()) &&
            !isSquareAttacked(QPoint(5, row), piece->getColor()) &&
            !isSquareAttacked(QPoint(6, row), piece->getColor())) {
            moves.append(QPoint(6, row));
        }

        // длинная рокировка
        ChessPiece* rookLong = pieceAt(0, row);
        if (rookLong && rookLong->getType() == ChessPiece::Rook &&
            !rookLong->hasMovedAlready() &&
            isEmpty(1, row) && isEmpty(2, row) && isEmpty(3, row) &&
            !isSquareAttacked(QPoint(4, row), piece->getColor()) &&
            !isSquareAttacked(QPoint(3, row), piece->getColor()) &&
            !isSquareAttacked(QPoint(2, row), piece->getColor())) {
            moves.append(QPoint(2, row));
        }
    }

    // здесь можешь ещё фильтровать ходы на шах для других фигур, если надо
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

    emit pieceCaptured(enemy);

    scene->removeItem(enemy);
    enemy->hide();
    pieces[y][x] = nullptr;

}

void Board::movePiece(ChessPiece *piece, int x, int y) {
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

void Board::movePieceFromTo(ChessPiece *piece, QPoint from, QPoint to)
{
    int x = to.x();
    int y = to.y();
    pieces[y][x] = piece;
    piece->setPositionOnTheBoard(to);
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
                QList<QPoint> attacks = rawAvailableMoves(piece); // Без фильтрации по шаху
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
                QList<QPoint> enemyMoves = rawAvailableMoves(piece);
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

void Board::highlightAfterCheck(ChessPiece::Color enemyColor)
{
    QPoint kingPos = (enemyColor == ChessPiece::Color::White) ? whiteKingPos : blackKingPos;

    // Создаем красный прямоугольник поверх клетки
    highlight = new QGraphicsRectItem(
        kingPos.x() * tileSize, kingPos.y() * tileSize,
        tileSize, tileSize
        );
    highlight->setBrush(Qt::red);
    highlight->setOpacity(0.5);
    scene->addItem(highlight);

    // Удаляем подсветку через 2 секунды
    QTimer::singleShot(1000, [=]() {
        scene->removeItem(highlight);
        delete highlight;
        highlight = nullptr;
    });
}

void Board::onBestMoveReceived(const QString& move) {
    qDebug() << "Stockfish move:" << move;

    if (move.length() < 4) {
        qWarning() << "Некорректный формат хода:" << move;
        return;
    }

    int fromX = move[0].toLatin1() - 'a';
    int fromY = 8 - (move[1].toLatin1() - '0');
    int toX = move[2].toLatin1() - 'a';
    int toY = 8 - (move[3].toLatin1() - '0');

    QPoint from(fromX, fromY);
    QPoint to(toX, toY);

    ChessPiece* piece = pieceAt(fromX, fromY);
    if (!piece) {
        qWarning() << "❌ Stockfish ссылается на пустую клетку:" << from;
        return;
    }
    makeMove(piece, from, to, false);
}

void Board::makeMove(ChessPiece* piece, QPoint oldBoardPos, QPoint newBoardPos, bool isFromPlayer) {
    bool moveAllowed = false;
    bool soundPlayed = false;
    bool gameOver = false;
    ChessPiece *promoted = nullptr;

    //ракировка
    if (piece->getType() == ChessPiece::King) {
        int row = (piece->getColor() == ChessPiece::White) ? 7 : 0;

        // КОРОТКАЯ
        if (newBoardPos == QPoint(6, row) && !piece->hasMovedAlready()) {
            ChessPiece* rook = pieceAt(7, row);
            if (rook) {
                movePiece(rook, 5, row); // h1 → f1
                //movePieceFromTo(rook, oldBoardPos, QPoint(5, row)); // h1 → f1
            }
        }

        // ДЛИННАЯ
        if (newBoardPos == QPoint(2, row) && !piece->hasMovedAlready()) {
            ChessPiece* rook = pieceAt(0, row);
            if (rook) {
                movePiece(rook, 3, row); // a1 → d1
                //movePieceFromTo(rook, oldBoardPos, QPoint(3, row)); // a1 → d1
            }
        }
        Sound::instance().playCastleSound();
        soundPlayed = true;
    }

    // Попытка захвата вражеской фигуры
    if (isEnemy(newBoardPos.x(), newBoardPos.y(), piece->getColor())) {
        capturePiece(newBoardPos.x(), newBoardPos.y());  // 👈 Удаляет и заносит в список убитых
        Sound::instance().playCaptureSound();
        soundPlayed = true;
    }

    // En Passant - взятие на проходе
    if (piece->getType() == ChessPiece::PieceType::Pawn && newBoardPos == getEnPassantTarget()) {
        int dy = (piece->getColor() == ChessPiece::Color::White) ? 1 : -1;
        QPoint enemyPos(newBoardPos.x(), newBoardPos.y() + dy);
        capturePiece(enemyPos.x(), enemyPos.y());
        Sound::instance().playCaptureSound();
        soundPlayed = true;
    }

    // Pawn Promotion
    bool isPromotion  = false;
    if (piece->getType() == ChessPiece::PieceType::Pawn) {
        int finalRank = (piece->getColor() == ChessPiece::Color::White) ? 0 : 7;
        if (newBoardPos.y() == finalRank) {
            qDebug() << "become Queen";
            ChessPiece::Color color = piece->getColor();
            if (isFromPlayer) {
                PromotionDialog dialog(color);
                if (dialog.exec() == QDialog::Accepted) {
                    ChessPiece::PieceType promotedType = dialog.getSelectedPieceType();
                    qDebug() << "pawn become to " << promotedType;
                    promoted = pawnPromotion(promotedType, color);
                    Sound::instance().playPromoteSound();
                    soundPlayed = true;
                    isPromotion  = true;
                }
            } else {    //stockfish
                promoted = new ChessPiece(ChessPiece::PieceType::Queen, ChessPiece::Color::Black, ":/svg_files/img/queen-b.svg");
                promoted->setScale(tileSize / 128.0);
                scene->addItem(promoted);
                soundPlayed = true;
                isPromotion  = true;
            }
        }
    }

    ChessPiece *currentPiece = (promoted == nullptr) ? piece : promoted;
    movePieceFromTo(currentPiece, oldBoardPos, newBoardPos);
    //newPiece->setPos(x * Board::tileSize, y * Board::tileSize);

    // Удаляем старую пешку, если промоция
    if (promoted != nullptr) {
        piece->deleteLater();  // безопасное удаление в Qt
    }

    moveAllowed = true;

    ChessPiece::Color opponentColor = (piece->getColor() == ChessPiece::White) ? ChessPiece::Black : ChessPiece::White;
    if (isKingInCheck(opponentColor)) {
        qDebug() << "Шах!";
        highlightAfterCheck(opponentColor);
        if (!soundPlayed) Sound::instance().playCheckSound();
        soundPlayed = true;
    }
    if (isCheckmate(opponentColor)) {
        qDebug() << "♚♛ МАТ!";
        clock->stopAll();
        QMessageBox::information(nullptr, "Мат", QString(" мат ") + (opponentColor == ChessPiece::White ? "Белым!" : "Чёрным!"));
        if (!soundPlayed) Sound::instance().playCheckSound();
        soundPlayed = true;
        gameOver = true;
    } else if (isStalemate(opponentColor)) {
        qDebug() << "🤝 ПАТ!";
        clock->stopAll();
        QMessageBox::information(nullptr, "Пат", "Ничья: патовое положение!");
        if (!soundPlayed) Sound::instance().playDrawSound();
        soundPlayed = true;
    }

    //need to add in historyMove
    addMoveHistory(currentPiece, oldBoardPos, newBoardPos);

    if (!soundPlayed) Sound::instance().playMoveSound();

    // Stockfish Engine
    // Добавляем в историю
    if (isAgainstComputer()) {
        QString fromX = QString(QChar(97 + oldBoardPos.x()));
        QString fromY = QString::number(8 - oldBoardPos.y());
        QString toX = QString(QChar(97 + newBoardPos.x()));
        QString toY = QString::number(8 - newBoardPos.y());
        QString moveNotation = fromX + fromY + toX + toY;
        qDebug() << "Move notation = " << moveNotation;

        if (isPromotion) {
            moveNotation += "q";
            isPromotion  = false;
        }

        moveHistory.append(moveNotation);
        // Отправляем движку всю историю
        QString moves = moveHistory.join(" ");
        getEngine()->sendCommand("position startpos moves " + moves);
        if (isFromPlayer) {
            getEngine()->sendCommand("go movetime 2000");
        }
    }

    if (!gameOver) switchTurn();

    currentPiece->setZValue(1);
    piece->getCachedMoves().clear();
    piece->setSelectedState(false);
    if (promoted) {
        promoted->setSelectedState(false);
        piece->setSelectedPiece(promoted);
    }
    if (moveAllowed) {
        clearHints();
    }
}

