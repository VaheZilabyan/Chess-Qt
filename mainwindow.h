#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "chesspiece.h"

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPieceCaptured(ChessPiece* piece);

private:
    static constexpr int MIN_WIDTH = 840;
    static constexpr int MIN_HEIGHT = 600;

    QGraphicsScene *scene;
    QGraphicsView *whiteGraveView;
    QGraphicsView *blackGraveView;
    QGraphicsScene* whiteGraveScene;
    QGraphicsScene* blackGraveScene;
};
#endif // MAINWINDOW_H
