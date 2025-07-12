#ifndef CHESSCLOCK_H
#define CHESSCLOCK_H

#include <QWidget>
#include <QLCDNumber>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class ChessClock : public QWidget
{
    Q_OBJECT

public:
    explicit ChessClock(QWidget *parent = nullptr);

    void startWhite();
    void startBlack();
    void stopAll();
    void reset(int secondsPerSide);

    QLCDNumber* getWhiteDisplay() const { return whiteDisplay; }
    QLCDNumber* getBlackDisplay() const { return blackDisplay; }

    void setTime(QLCDNumber* clock, int seconds);
    QString formatTime(int totalSeconds);

signals:
    void timeOut(QString color);  // "White" or "Black"

private:
    QLCDNumber* whiteDisplay;
    QLCDNumber* blackDisplay;
    QTimer* whiteTimer;
    QTimer* blackTimer;
    int whiteSeconds;
    int blackSeconds;

    int initialTime = 5 * 60;  // default 5 minutes per side
};

#endif // CHESSCLOCK_H
