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

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

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

    scene->setSceneRect(-10, -10, 690, 690); //
    graphicsView->setFixedSize(700, 700); // need to scene size == view size

    Board *board = Board::getInstance(scene);
    board->setupInitialPosition();

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}
