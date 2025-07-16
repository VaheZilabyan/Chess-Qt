#include "mainwindow.h"
#include "chesspiece.h"
#include "settingswindow.h"
#include "board.h"
#include "chessclock.h"
#include "stockfishengine.h"

#include <QProcess>
#include <QLayout>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(MIN_WIDTH, MIN_HEIGHT);
    this->setMinimumWidth(MIN_WIDTH);
    this->setMinimumHeight(MIN_HEIGHT);
    this->setFixedHeight(MIN_HEIGHT);   // // // //

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    clock = new ChessClock(this);
    clock->reset(5 * 60);  // 5 минут

    QGraphicsView *graphicsView = new QGraphicsView(this);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    mainLayout->addWidget(graphicsView);

    whiteGraveView  = new QGraphicsView(this);
    blackGraveView  = new QGraphicsView(this);
    whiteGraveScene = new QGraphicsScene(this);
    blackGraveScene = new QGraphicsScene(this);

    whiteGraveView->setScene(whiteGraveScene);
    whiteGraveView->setFixedHeight(80);
    whiteGraveView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    whiteGraveView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    blackGraveView->setScene(blackGraveScene);
    blackGraveView->setFixedHeight(80);
    blackGraveView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    blackGraveView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *graveWidget = new QWidget(this);
    QVBoxLayout *graveLayout = new QVBoxLayout(graveWidget);

    historyWidget = new QTableWidget(0, 2);
    historyWidget->setHorizontalHeaderLabels(QStringList() << "Piece" << "Move");
    historyWidget->setColumnWidth(0, 122);
    historyWidget->setColumnWidth(1, 122);
    historyWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    graveLayout->addWidget(clock->getBlackDisplay());
    graveLayout->addWidget(whiteGraveView);
    graveLayout->addWidget(historyWidget);
    graveLayout->addWidget(blackGraveView);
    graveLayout->addWidget(clock->getWhiteDisplay());

    mainLayout->addWidget(graveWidget);

    int boardSize = Board::tileSize * 8;
    scene->setSceneRect(0, 0, boardSize, boardSize); // 0,0 to 640,640
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    int viewSize = boardSize + 2 * graphicsView->frameWidth();
    graphicsView->setFixedSize(viewSize, viewSize);

    Board *board = Board::getInstance();
    board->setScene(scene);
    board->setupInitialPosition();
    board->setClock(clock);

    setCentralWidget(centralWidget);

    QMenu* playMenu = menuBar()->addMenu("Play");
    QMenu* settingsMenu = menuBar()->addMenu("Settings");
    QAction* newGameAction = new QAction("New Game", this);
    QAction* settingsAction = new QAction("Edit...", this);
    playMenu->addAction(newGameAction);
    settingsMenu->addAction(settingsAction);

    connect(newGameAction, &QAction::triggered, this, &MainWindow::onNewGameClicked);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::onChangeBoardClicked);

    connect(board, &Board::pieceCaptured, this, &MainWindow::onPieceCaptured);
    connect(board, &Board::addMoveSignal, this, &MainWindow::onAddMove);
    connect(clock, &ChessClock::timeOut, this, &MainWindow::timeOverSlot);
}

MainWindow::~MainWindow() {}

void MainWindow::onPieceCaptured(ChessPiece* piece) {
    if (!piece) return;

    QGraphicsScene* graveScene = (piece->getColor() == ChessPiece::White)
                                     ? whiteGraveScene : blackGraveScene;
    if (!graveScene) return;

    piece->hide();

    ChessPiece* copy = new ChessPiece(piece->getType(), piece->getColor(), piece->getSvgPath());

    int index = graveScene->items().size();
    int row = index / 8;
    int col = index % 8;

    int tile = Board::tileSize;
    copy->setScale((tile * 0.5) / 128.0);
    copy->setPos(col * 30, row * 35);

    graveScene->addItem(copy);
}

void MainWindow::onAddMove(ChessPiece *piece, QPoint from, QPoint to)
{
    QString pieceStr = getPieceNameStr(piece);
    QString fromX = QString(QChar(97 + from.x()));
    QString fromY = QString::number(8 - from.y());
    QString toX = QString(QChar(97 + to.x()));
    QString toY = QString::number(8 - to.y());

    int row = historyWidget->rowCount();
    historyWidget->insertRow(row);
    historyWidget->setItem(row, 0, new QTableWidgetItem(pieceStr));
    historyWidget->setItem(row, 1, new QTableWidgetItem(fromX + fromY + "-" + toX + toY));

    historyWidget->scrollToItem(historyWidget->item(row, 0));
}

void MainWindow::onNewGameClicked()
{
    // Reset board, clear move history, etc.
    qDebug() << "Starting new game...";

    whiteGraveScene->clear();
    blackGraveScene->clear();
    Board::getInstance()->resetBoard();      // You need to implement this
    historyWidget->setRowCount(0);           // Clear move history
    clock->reset(5 * 60);
}

void MainWindow::onChangeBoardClicked()
{
    SettingsWindow setting(this);
    setting.exec(); // Modal, blocks until user closes the dialog
}

void MainWindow::timeOverSlot(const QString& player)
{
    QString winner = (player == "White") ? "Black" : "White";
    QMessageBox::information(this, "Time Over", winner + " wins by timeout!");
    onNewGameClicked();
}

QString MainWindow::getPieceNameStr(ChessPiece *piece)
{
    ChessPiece::Color color = piece->getColor();
    ChessPiece::PieceType type = piece->getType();
    QString pieceTypeStr = "";

    if (color == ChessPiece::Color::White) pieceTypeStr += "w ";
    else pieceTypeStr += "b ";

    switch (type) {
    case ChessPiece::PieceType::Pawn:
        pieceTypeStr += "Pawn";
        break;
    case ChessPiece::PieceType::Rook:
        pieceTypeStr += "Rook";
        break;
    case ChessPiece::PieceType::Knight:
        pieceTypeStr += "Knight";
        break;
    case ChessPiece::PieceType::Bishop:
        pieceTypeStr += "Bishop";
        break;
    case ChessPiece::PieceType::King:
        pieceTypeStr += "King";
        break;
    case ChessPiece::PieceType::Queen:
        pieceTypeStr += "Queen";
        break;
    }
    return pieceTypeStr;
}








