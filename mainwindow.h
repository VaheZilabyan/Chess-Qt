#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chesspiece.h"
#include "chessclock.h"
#include "board.h"

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTableWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPieceCaptured(ChessPiece* piece);
    void onAddMove(ChessPiece* piece, QPoint from, QPoint to);
    void onNewGameClicked();
    void onChangeBoardClicked();
    void timeOverSlot(const QString &player);
    void checkMateSlot();
    void vsComputerClicked();

private:
    QString getPieceNameStr(ChessPiece*);

    static constexpr int MIN_WIDTH = 840;
    static constexpr int MIN_HEIGHT = 600;

    Board *board;
    QGraphicsScene *scene;
    QGraphicsView *whiteGraveView;
    QGraphicsView *blackGraveView;
    QGraphicsScene* whiteGraveScene;
    QGraphicsScene* blackGraveScene;
    QTableWidget *historyWidget;
    ChessClock *clock;
    StockfishEngine *engine;
};
#endif // MAINWINDOW_H
