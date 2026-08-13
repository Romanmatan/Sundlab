#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <windows.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <thread>
#include <QObject>
#include <QThread>
#include <QString>
//alBufferData
using namespace std;

class Player
{
public:
Player(short *buf, ALsizei num, int rate, ALenum format);
~Player();

void Play();
void stop();
void SetVolume(ALfloat volume);
void SetPosition(ALfloat X, ALfloat Y, ALfloat Z);
string CheckErrors()
{
    return errors;
};


private:
int SampRate;
ALsizei SampNumber;
short *SampBuffer;
ALenum format;
bool stopv;
ALCdevice  * openal_output_device;
ALCcontext * openal_output_context;

ALuint internal_buffer;
ALuint streaming_source[1];
string errors;

void al_check_error(const char *given_label);

};



class PlayThread : public QObject
{
    Q_OBJECT

public:
   void SetData(short *buf, ALsizei num, int rate, ALenum format);
   void SetPosition(ALfloat X, ALfloat Y, ALfloat Z);
   void SetVolume(ALfloat vol);


public slots:
    void process();
    void stop();

signals:
    void finished();
    void ErRet(QString err);
private:
   short *buf;
   ALenum format;
   ALsizei num;
   ALfloat volume;
   ALfloat X;
   ALfloat Y;
   ALfloat Z;
   int rate;
   Player *plr;
   QString error;
};

#endif // PLAYER_H