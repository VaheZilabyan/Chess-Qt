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
    static constexpr int MIN_WIDTH = 600;
    static constexpr int MIN_HEIGHT = 600;
};
#endif // MAINWINDOW_H
