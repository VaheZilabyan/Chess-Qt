#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    static constexpr int MIN_WIDTH = 700;
    static constexpr int MIN_HEIGHT = 700;
};
#endif // MAINWINDOW_H
