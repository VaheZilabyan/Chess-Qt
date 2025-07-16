#include "stockfishengine.h"

#include <QDebug>

StockfishEngine::StockfishEngine(QObject *parent) : QObject(parent) {
    engine = new QProcess(this);
    engine->start("C:/Users/vzila/Downloads/stockfish-windows-x86-64-avx2/stockfish/stockfish-windows-x86-64-avx2.exe");

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
