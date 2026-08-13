#include "player.h"
#include <QCoreApplication>

void Player::al_check_error(const char * given_label)
{

    ALenum al_error;
    al_error = alGetError();
    if(AL_NO_ERROR != al_error)
    {
        errors.append("ERROR - ");
        errors.append(alGetString(al_error));
        errors.append(given_label);
        errors.append("\r");
    }
}

/*
{

}
*/
Player::Player(short *buf, ALsizei num, int rate, ALenum format)
{
    SampBuffer = buf;
    SampRate = rate;
    SampNumber = num;
    const char * defname = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    openal_output_device  = alcOpenDevice(defname);
    openal_output_context = alcCreateContext(openal_output_device, NULL);
    alcMakeContextCurrent(openal_output_context);
    // setup buffer and source
    alGenBuffers(1, & internal_buffer);
    al_check_error("failed call to alGenBuffers");

    /* upload buffer to OpenAL */
    alBufferData( internal_buffer, format, SampBuffer, SampNumber, SampRate);
    al_check_error("populating alBufferData");

    // ---------------------
    alGenSources(1, & streaming_source[0]);
    alSourcei(streaming_source[0], AL_BUFFER, internal_buffer);
    stopv = 0;
}

Player::~Player() {

    ALenum errorCode = 0;

    // Stop the sources
    alSourceStopv(1, & streaming_source[0]);        //      streaming_source
    //int ii;
    //for (ii = 0; ii < 1; ++ii) {
   // alSourcei(streaming_source[0], AL_BUFFER, 0);
   // }
    // Clean-up
    alDeleteSources(1, &streaming_source[0]);
    alDeleteBuffers(16, &streaming_source[0]);
    errorCode = alGetError();
    alcMakeContextCurrent(NULL);
    errorCode = alGetError();
    alcDestroyContext(openal_output_context);
    alcCloseDevice(openal_output_device);
}

void Player::SetPosition(ALfloat X, ALfloat Y, ALfloat Z)
{
    ALfloat Pos[3] = { X, Y, Z };
    alSourcefv(streaming_source[0], AL_POSITION, Pos);
    alDistanceModel(AL_NONE);
}

void Player::SetVolume(ALfloat volume)
{
    alSourcef(streaming_source[0], AL_GAIN, volume);
}

void Player::Play()
{



    alSourcePlay(streaming_source[0]);

    // ---------------------

    ALenum current_playing_state;
    alGetSourcei(streaming_source[0], AL_SOURCE_STATE, & current_playing_state);
    al_check_error("alGetSourcei AL_SOURCE_STATE");

    while (AL_PLAYING == current_playing_state)
    {

        alGetSourcei(streaming_source[0], AL_SOURCE_STATE, & current_playing_state);
        for (int i = 0; i<5; i++)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            QCoreApplication::processEvents();
            al_check_error("alGetSourcei AL_SOURCE_STATE");
            if (stopv == 1)
            {
                alSourceStopv(1, & streaming_source[0]);
            }
            //ALfloat Pos[3] = { 0.1, 54, 0 };
            //alSourcefv(streaming_source[0], AL_POSITION, Pos);
        }

    }
}

void Player::stop()
{
    stopv = 1;
}






void PlayThread::SetData(short *buf, ALsizei num, int rate, ALenum format)
{
    this->buf = buf;
    this->num = num;
    this->rate = rate;
    this->format = format;
}

void PlayThread::process()
{
    /*
    if (this->num == 0) return;
    plr->Play();
    error = plr->CheckErrors();
    num = 0;
    delete(plr);
    */

CoInitialize(0);
    plr = new Player(buf, num, rate, format);
    plr->SetPosition(X,Y,Z);
    plr->SetVolume(volume);
    plr->Play();
    string err = plr->CheckErrors();
    delete(plr);
    //emit finished(QString::fromStdString(err));
    emit ErRet(QString::fromStdString(err));
    emit finished();
    //QThread::sleep(1);

}

void PlayThread::SetVolume(ALfloat vol)
{
    //plr->SetVolume(vol);
    this->volume = vol;
}

void PlayThread::SetPosition(ALfloat X, ALfloat Y, ALfloat Z)
{
    //plr->SetPosition(X,Y,Z);
    this->X = X;
    this->Y = Y;
    this->Z = Z;
}

void PlayThread::stop()
{
    if(plr != NULL)
    {
        plr->stop();
    }
    return ;
}

