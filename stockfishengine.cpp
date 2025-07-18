#include "stockfishengine.h"

#include <QCoreApplication>
#include <QDebug>

StockfishEngine::StockfishEngine(QObject *parent) : QObject(parent) {
    engine = new QProcess(this);
    QString path = QCoreApplication::applicationDirPath() + "/../../Chess/stockfish/stockfish-windows-x86-64-avx2.exe";
    engine->start(path);

    qDebug() << QCoreApplication::applicationDirPath();
    qDebug() << path;
    if (!engine->waitForStarted()) {
        qWarning() << "❌ Stockfish не запустился!";
    } else {
        qDebug() << "✅ Stockfish запущен";
    }

    connect(engine, &QProcess::readyReadStandardOutput, [=]() {
        QString output = engine->readAllStandardOutput();
        if (output.contains("bestmove")) {
            QString move = output.split("bestmove ")[1].split(" ")[0];
            emit bestMoveReceived(move);
        }
    });
}

void StockfishEngine::sendCommand(const QString &command) {
    engine->write(command.toUtf8() + "\n");
}
