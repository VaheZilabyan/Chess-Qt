#ifndef STOCKFISHENGINE_H
#define STOCKFISHENGINE_H

#include <QObject>
#include <QProcess>
#include <QDebug>

#include <QObject>
#include <QProcess>

class StockfishEngine : public QObject {
    Q_OBJECT
public:
    StockfishEngine(QObject *parent = nullptr);
    void sendCommand(const QString &command);

signals:
    void bestMoveReceived(const QString &move);

private:
    QProcess *engine;
};

#endif // STOCKFISHENGINE_H
