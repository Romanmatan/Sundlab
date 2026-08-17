#include "create_signal.h"
#if defined (__cplusplus)
extern "C" {//Добавляем заголовочный файл на языке "С".
#endif
#include <fft.h>
bool Fft_transform(double real[], double imag[], size_t n);
bool Fft_inverseTransform(double real[], double imag[], size_t n);

#if defined (__cplusplus)
}
#endif
//Запись монофонического буфера для OpenAL
void WriteSamplesMono(short *samples, double *signal, ALsizei buf_size)
{
    for(int i = 0; i<buf_size; ++i)
    {
        if (signal[i] > 4) signal[i] = 4;
        if (signal[i] < -4) signal[i] = -4;
        samples[i] = short(8191 * signal[i]);
    }
}
//Запись стереофонического буфера для OpenAL
void WriteSamplesStereo(short *samples, double *signal_left, double *signal_right, ALsizei buf_size)
{
    for(int i = 0; i<buf_size; ++i)
    {//Сигнал ограничен уровнями -4,4 для предотвращения превышения максимально возможного значения переменной типа short.
        if (signal_left[i] > 4) signal_left[i] = 4;
        if (signal_right[i] > 4) signal_right[i] = 4;
        if (signal_left[i] < -4) signal_left[i] = -4;
        if (signal_right[i] < -4) signal_right[i] = -4;
        samples[2*i] = short(8191 * signal_left[i]);
        samples[2*i+1]  = short(8191 * signal_right[i]);
    }
}

//Запись стереофонического сигнала для первого задания. Позволяет вносить фазовый сдвиг между каналами.
void CreateSignal1Stereo(double *signal_left, double *signal_right, ALsizei buf_size, double frequency, double angle, double rotfreq, bool rotation, bool leftchan, bool rightchan, bool smooth, double ampl)
{
    ALsizei i=0;

    double phase = 0.21*cos(angle)*frequency*2*M_PI/341.0;
    if (rotation == false)
    {
        for(; i<buf_size; ++i)
        {
            if (leftchan == true) signal_left[i] = ampl*sin( (2.f * M_PI * frequency)/sample_rate * i );
            else signal_left[i] = 0;
            if (rightchan == true) signal_right[i] = ampl*sin( (2.f * M_PI * frequency)/sample_rate * i + phase);
            else signal_right[i] = 0;
        }
    }
    else
    {
        for(; i<buf_size; ++i)
        {//Если наобходимо смоделировать движение источника звука в пространстве, добввляем фазовый сдвиг.
            angle = angle + rotfreq*2*M_PI/sample_rate;
            phase = 0.21*cos(angle)*frequency*2*M_PI/341.0;
            if (leftchan == true) signal_left[i] = ampl*sin( (2.f * M_PI * frequency)/sample_rate * i - phase/2.0);
            else signal_left[i] = 0;
            if (rightchan == true) signal_right[i] = ampl*sin( (2.f * M_PI * frequency)/sample_rate * i + phase/2.0);
            else signal_right[i] = 0;
        }
    }
    if (smooth == 1)
    {
        ALsizei fadesamp = ALsizei(sample_rate/frequency*3);
        for(i=0; i<fadesamp; i++) //Плавное нарастание и спад уровня звука для предотвращения появления высокочастотных составляющих спектра.
        {
             if (leftchan == true) signal_left[i] *= (1-exp(-i/(1.0*sample_rate/frequency)))*1.0525;
             if (rightchan == true) signal_right[i] *= (1-exp(-i/(1.0*sample_rate/frequency)))*1.0525;
        }

        for(i=buf_size-fadesamp; i<buf_size; i++)
        {
             if (leftchan == true) signal_left[i] *= (exp(-(i-(buf_size-fadesamp))/(1.0*sample_rate/frequency)));
             if (rightchan == true) signal_right[i] *= (exp(-(i-(buf_size-fadesamp))/(1.0*sample_rate/frequency)));
        }
    }

}




//Создание монофонического сигнала для второго задания.
void CreateSignal2Mono(double *signal,  ALsizei buf_size, double duration1, double duration2, double a1, double a2,  double time1, double time2,  double frequency1, double frequency2)
{
    ALsizei n1 = ALsizei(time1*sample_rate);
    ALsizei n2 = ALsizei(time2*sample_rate);
    ALsizei d1 = ALsizei(duration1*sample_rate);
    ALsizei d2 = ALsizei(duration2*sample_rate);
    if(a1<0)a1=0;
    if(a2<0)a2=0;
    if(a1>1)a1=1; //Амплитуда сигнала должна быть в интервале 0,1.
    if(a2>1)a2=1;

        for(ALsizei i=0; i<buf_size; ++i)
        {
            signal[i] = 0;
            if (i > n1 && i<n1+d1) signal[i] += sin( (2.f * M_PI * frequency1)/sample_rate * (i - n1 - 1))*a1; //Первый импульс
            if (i > n2 && i<n2+d2) signal[i] += sin( (2.f * M_PI * frequency2)/sample_rate * (i - n1 - 1))*a2; //Второй импульс
        }
}




//Создание монофонического сигнала для третьего задания.
void CreateSignal3MonoAM(double *signal,  ALsizei buf_size, double frequency, double modfrequency, double moddepth, bool smooth, double ampl)
{
    if (smooth == 1)
    {
        if(ampl<0)ampl=0;
        if(ampl>1)ampl=1; //Амплитуда сигнала должна быть в интервале 0,1.

        ALsizei i=0;

        for(; i<buf_size; ++i)
        {//Формирование АМ - сигнала.
            signal[i] = 1/(1+moddepth) * ampl*sin((2.f * M_PI * frequency)/(1.0*sample_rate) * i)*(1 + moddepth*sin((2.f * M_PI * modfrequency)/(1.0*sample_rate) * i));
        }
        //Плавное нарастание и спад уровня звука для предотвращения появления высокочастотных составляющих спектра.
        ALsizei fadesamp = ALsizei(sample_rate/frequency*3);
        for(i=0; i<fadesamp; i++)
        {
             signal[i] *= (1-exp(-i/(1.0*sample_rate/frequency)))*1.0525;
        }

        for(i=buf_size-fadesamp; i<buf_size; i++)
        {
             signal[i] *= (exp(-(i-(buf_size-fadesamp))/(1.0*sample_rate/frequency)));
        }
    }
}

void CreateSignal3MonoPM(double *signal,  ALsizei buf_size, double frequency, double modfrequency, double moddepth, bool smooth, double ampl)
{
    if(ampl<0)ampl=0;
    if(ampl>1)ampl=1;

    ALsizei i=0;

    for(; i<buf_size; ++i)
    {//Формирование ФМ - сигнала.
        signal[i] = ampl*sin((2.f * M_PI * frequency)/sample_rate * i + (moddepth*sin((2.f * M_PI * modfrequency)/sample_rate * i)));
    }
    if (smooth == 1)
    {
        ALsizei fadesamp = ALsizei(sample_rate/frequency*3);
        for(i=0; i<fadesamp; i++)
        {
             signal[i] *= (1-exp(-i/(1.0*sample_rate/frequency)))*1.0525;
        }

        for(i=buf_size-fadesamp; i<buf_size; i++)
        {
             signal[i] *= (exp(-(i-(buf_size-fadesamp))/(1.0*sample_rate/frequency)));
        }
    }
}


void CreateSignal3MonoFM(double *signal,  ALsizei buf_size, double frequency, double modfrequency, double freqdev, bool smooth, double ampl)
{
    if(ampl<0)ampl=0;
    if(ampl>1)ampl=1;

    ALsizei i=0;

    for(; i<buf_size; ++i)
    {//Формирование ЧМ - сигнала.
        double f2 = freqdev/modfrequency*sin(2.f * M_PI * modfrequency/sample_rate * i);
        signal[i] = ampl*sin((2.f * M_PI * frequency)/sample_rate * i +f2); //freqdev*sin(2.f * M_PI * modfrequency)
    }
    if (smooth == 1)
    {
        ALsizei fadesamp = ALsizei(sample_rate/frequency*3);
        for(i=0; i<fadesamp; i++)
        {
             signal[i] *= (1-exp(-i/(1.0*sample_rate/frequency)))*1.0525;
        }

        for(i=buf_size-fadesamp; i<buf_size; i++)
        {
             signal[i] *= (exp(-(i-(buf_size-fadesamp))/(1.0*sample_rate/frequency)));
        }
    }
}

double GetfadeValue(double frequency, std::vector<double> *freqs, std::vector<double> *values) //Получение относительного коэффициента затухания для некоторой частоты в соответствии с АЧХ наушников.
{
    int p1 = 0;
    int p2 = 0;
    if (frequency < freqs->at(0))
    {
        p1 = 0;
        p2 = 1;
    }
    else if (frequency > freqs->at(freqs->size()-1))
    {
        p1 = int(freqs->size()-1);
        p2 = int(freqs->size()-2);
    }
    else
    {
        for (int k = 0; k < int(freqs->size())-1; k++)
        {
            if (frequency >= freqs->at(k) && frequency < freqs->at(k+1))
            {
                p1 = k;
                p2 = k+1;
                break;
            }
        }
    }

    double result = 0;
    if(freqs->at(p2) != freqs->at(p1))
    {
        double a = (values->at(p2) - values->at(p1))/(freqs->at(p2) - freqs->at(p1)); //Линейная аппроксимация.
        result = values->at(p1) + a * (frequency - freqs->at(p1));
        if(result < 0) result = 0;
        if(result > 1) result = 1;
    }
    else result = values->at(p1); //Если наклон аппроксимирующей прямой равен нулю.

    return result;
}

void attenuator (double *signal, ALsizei buf_size, double sampfreq, std::vector<double> *freqs, std::vector<double> *values)//Эквалайзер на основе преобразования Фурье.
{
    double df = sampfreq/(buf_size*1.0);
    double *imag = new double[buf_size];
    for(int k = 0; k<buf_size; k++) imag[k] = 0;
    Fft_transform(signal, imag, buf_size);//Сигнал переводится в частотную область.
    double frequency;
    for(int k = 0; k<buf_size/2+1; k++)
    {
        frequency = k*df;
        double fade = GetfadeValue(frequency, freqs, values);//Производится домножение частотных составляющих на коэффициенты, компенсирующие АЧХ наушников.
        signal[k] = signal[k]*fade;
        imag[k] = imag[k]*fade;
    }
    for(int k = buf_size/2+1; k<buf_size; k++)
    {
        frequency = df*(buf_size-k);
        double fade = GetfadeValue(frequency, freqs, values);//Производится домножение частотных составляющих на коэффициенты, компенсирующие АЧХ наушников.
        signal[k] = signal[k]*fade;
        imag[k] = imag[k]*fade;
    }
    Fft_inverseTransform(signal, imag, buf_size);//Восстановление сигнала во временной области.
    for(int k = 0; k < buf_size; k++)
    {
        signal[k] = signal[k]/buf_size;//Нормировка сигнала.
    }
    delete [] imag;
}
