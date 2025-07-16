#ifndef SOUND_H
#define SOUND_H

#include <QSoundEffect>

class Sound
{
public:
    static Sound& instance() {
        static Sound s;
        return s;
    }
    void setupSounds();
    void playCaptureSound();
    void playCastleSound();
    void playCheckSound();
    void playDrawSound();
    void playStartSound();
    void playMoveSound();
    void playPromoteSound();
    void playTenSecondSound();

private:
    QSoundEffect captureSound;
    QSoundEffect castleSound;
    QSoundEffect checkSound;
    QSoundEffect drawSound;
    QSoundEffect startSound;
    QSoundEffect moveSound;
    QSoundEffect promoteSound;
    QSoundEffect tenSecondSound;

    Sound() {}
};

#endif // SOUND_H
