#include "sound.h"

#include <QDebug>

void Sound::setupSounds() {
    captureSound.setSource(QUrl("qrc:/chess_sounds/sounds/capture.wav"));
    captureSound.setVolume(0.5);

    castleSound.setSource(QUrl("qrc:/chess_sounds/sounds/castle.wav"));
    castleSound.setVolume(0.5);

    checkSound.setSource(QUrl("qrc:/chess_sounds/sounds/check.wav"));
    checkSound.setVolume(0.5);

    drawSound.setSource(QUrl("qrc:/chess_sounds/sounds/game-draw.wav"));
    drawSound.setVolume(0.5);

    startSound.setSource(QUrl("qrc:/chess_sounds/sounds/game-start.wav"));
    startSound.setVolume(0.5);

    moveSound.setSource(QUrl("qrc:/chess_sounds/sounds/move.wav"));
    moveSound.setVolume(0.5);

    promoteSound.setSource(QUrl("qrc:/chess_sounds/sounds/promote.wav"));
    promoteSound.setVolume(0.5);

    tenSecondSound.setSource(QUrl("qrc:/chess_sounds/sounds/tenseconds.wav"));
    tenSecondSound.setVolume(0.5);
}

void Sound::playCaptureSound() {
    if (captureSound.isLoaded()) {
        captureSound.play();
    } else {
        qDebug() << "Capture sound not loaded yet!";
    }
}
void Sound::playCastleSound() {
    if (castleSound.isLoaded()) {
        castleSound.play();
    } else {
        qDebug() << "Castle sound not loaded";
    }
}
void Sound::playCheckSound() {
    if (checkSound.isLoaded()) {
        checkSound.play();
    } else {
        qDebug() << "Check sound not loaded";
    }
}
void Sound::playDrawSound() {
    if (drawSound.isLoaded()) {
        drawSound.play();
    } else {
        qDebug() << "Draw sound not loaded";
    }
}
void Sound::playStartSound() {
    if (startSound.isLoaded()) {
        startSound.play();
    } else {
        qDebug() << "Start sound not loaded";
    }
}
void Sound::playMoveSound() {
    if (moveSound.isLoaded()) {
        moveSound.play();
    } else {
        qDebug() << "Move sound not loaded";
    }
}
void Sound::playPromoteSound() {
    if (promoteSound.isLoaded()) {
        promoteSound.play();
    } else {
        qDebug() << "Promote sound not loaded";
    }
}
void Sound::playTenSecondSound() {
    if (tenSecondSound.isLoaded()) {
        tenSecondSound.play();
    } else {
        qDebug() << "Ten seconds sound not loaded";
    }
}
