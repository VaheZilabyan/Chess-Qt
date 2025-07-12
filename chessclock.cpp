#include "ChessClock.h"
#include <QMessageBox>

ChessClock::ChessClock(QWidget *parent)
    : QWidget(parent),
    whiteDisplay(new QLCDNumber(5, this)),
    blackDisplay(new QLCDNumber(5, this)),
    whiteTimer(new QTimer(this)),
    blackTimer(new QTimer(this)),
    whiteSeconds(initialTime),
    blackSeconds(initialTime)
{
    whiteDisplay->setSegmentStyle(QLCDNumber::Flat);
    blackDisplay->setSegmentStyle(QLCDNumber::Flat);

    whiteDisplay->display(formatTime(whiteSeconds));
    blackDisplay->display(formatTime(blackSeconds));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("♙ White"));
    layout->addWidget(whiteDisplay);
    layout->addWidget(new QLabel("♟ Black"));
    layout->addWidget(blackDisplay);
    setLayout(layout);

    connect(whiteTimer, &QTimer::timeout, this, [=]() {
        whiteSeconds--;
        whiteDisplay->display(formatTime(whiteSeconds));
        if (whiteSeconds <= 0) {
            whiteTimer->stop();
            emit timeOut("White");
        }
    });

    connect(blackTimer, &QTimer::timeout, this, [=]() {
        blackSeconds--;
        blackDisplay->display(formatTime(blackSeconds));
        if (blackSeconds <= 0) {
            blackTimer->stop();
            emit timeOut("Black");
        }
    });
}

void ChessClock::startWhite() {
    blackTimer->stop();
    whiteTimer->start(1000);
}

void ChessClock::startBlack() {
    whiteTimer->stop();
    blackTimer->start(1000);
}

void ChessClock::stopAll() {
    whiteTimer->stop();
    blackTimer->stop();
}

void ChessClock::reset(int secondsPerSide) {
    whiteSeconds = secondsPerSide;
    blackSeconds = secondsPerSide;
    whiteDisplay->display(formatTime(whiteSeconds));
    blackDisplay->display(formatTime(blackSeconds));
    stopAll();
}

QString ChessClock::formatTime(int totalSeconds) {
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0'));
}
