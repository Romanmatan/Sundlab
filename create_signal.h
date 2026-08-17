#ifndef CREATE_SIGNAL_H
#define CREATE_SIGNAL_H

#endif // CREATE_SIGNAL_H
#include <AL/al.h>
#include <AL/alc.h>
//#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#define sample_rate 44100.0

void WriteSamplesMono(short *samples, double *signal, ALsizei buf_size);
void WriteSamplesStereo(short *samples, double *signal_left, double *signal_right, ALsizei buf_size);
void CreateSignal1Stereo(double *signal_left, double *signal_right, ALsizei buf_size, double frequency, double angle, double rotfreq, bool rotation, bool leftchan, bool rightchan,bool smooth, double ampl);
void CreateSignal2Mono(double *signal,  ALsizei buf_size, double duration1, double duration2, double a1, double a2,  double time1, double time2,  double frequency1, double frequency2);
void CreateSignal3MonoAM(double *signal,  ALsizei buf_size, double frequency, double modfrequency, double moddepth, bool smooth, double ampl);
void CreateSignal3MonoPM(double *signal,  ALsizei buf_size, double frequency, double modfrequency, double moddepth, bool smooth, double ampl);
void CreateSignal3MonoFM(double *signal,  ALsizei buf_size, double frequency, double modfrequency, double freqdev, bool smooth, double ampl);
double GetfadeValue(double frequency, std::vector<double> *freqs, std::vector<double> *values);
void attenuator (double *signal, ALsizei buf_size, double sampfreq, std::vector<double> *freqs, std::vector<double> *values);
