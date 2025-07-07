#include "mainwindow.h"
#include "chesspiece.h"
#include "board.h"

#include <QLayout>
#include <QPushButton>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QPixmap>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->resize(MIN_WIDTH, MIN_HEIGHT);
    this->setMinimumWidth(MIN_WIDTH);
    this->setMinimumHeight(MIN_HEIGHT);
    this->setFixedHeight(MIN_HEIGHT);   // // // //

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    /*QPushButton *startButton = new QPushButton("Player vs Player");
    QPushButton *settingsButton = new QPushButton("Settings");
    QPushButton *exitButton = new QPushButton("Exit");

    mainLayout->addWidget(startButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addWidget(exitButton);*/

    QGraphicsView *graphicsView = new QGraphicsView(this);
    graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QGraphicsScene *scene = new QGraphicsScene(this);
    graphicsView->setScene(scene);
    mainLayout->addWidget(graphicsView);

    whiteGraveView  = new QGraphicsView(this);
    blackGraveView  = new QGraphicsView(this);
    whiteGraveScene = new QGraphicsScene(this);
    blackGraveScene = new QGraphicsScene(this);

    whiteGraveView->setScene(whiteGraveScene);
    //whiteGraveView->setFixedSize(8 * Board::tileSize / 2, 5 * Board::tileSize);
    //whiteGraveView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //whiteGraveView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    blackGraveView->setScene(blackGraveScene);
    //blackGraveView->setFixedSize(8 * Board::tileSize / 2, 5 * Board::tileSize);
    //blackGraveView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //blackGraveView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *graveWidget = new QWidget(this);
    QVBoxLayout *graveLayout = new QVBoxLayout(graveWidget);
    graveLayout->addWidget(whiteGraveView);
    graveLayout->addWidget(blackGraveView);

    mainLayout->addWidget(graveWidget);

    scene->setSceneRect(-12, -12, 690, 690); //
    graphicsView->setFixedSize(550, 550); // need to scene size == view size

    Board *board = Board::getInstance();
    board->setScene(scene);
    board->setupInitialPosition();
    setLayout(mainLayout);
    setCentralWidget(centralWidget);

    connect(board, &Board::pieceCaptured, this, &MainWindow::onPieceCaptured);
}

MainWindow::~MainWindow() {}

void MainWindow::onPieceCaptured(ChessPiece* piece) {
    if (!piece) return;

    QGraphicsScene* graveScene = (piece->getColor() == ChessPiece::White) ? whiteGraveScene : blackGraveScene;
    if (!graveScene) return;

    // Удалить с игровой сцены
    Board::getInstance()->getScene()->removeItem(piece);
    piece->setVisible(true);

    // Посчитать позицию: 4 фигуры в ряд
    int index = graveScene->items().size();
    int row = index / 4;
    int col = index % 4;

    int tile = Board::tileSize;
    piece->setScale((tile * 0.6) / 128.0);
    piece->setPos(col * 40, row * 35);  // немного сжатие между фигурами

    graveScene->addItem(piece);
}


