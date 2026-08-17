#include "colorednoise.h"
#include <vector>
#include <complex>
//#include <math.h>
#include <random>
#include <iostream>
#include <chrono>
#include <fstream>
#include <QDebug>//!!!!!!!!!!!!!!!!!!!
#if defined (__cplusplus)
extern "C" {
#endif
#include "fft.h"
bool Fft_transform(double real[], double imag[], size_t n);
bool Fft_inverseTransform(double real[], double imag[], size_t n);

#if defined (__cplusplus)
}
#endif

using namespace std;

#define PI 3.1415926535897932

//uniform_real_distribution<double> distribution(0, 2*M_PI); // Генерируется равномерно распределённая величина в диапазоне 0:2*PI.
normal_distribution<double> distribution(0,1);
unsigned seed;
default_random_engine generator;

double normpdf (double x, double mean, double sigm)
{
    double val = 1/(sigm*sqrt(2*PI))*exp((-pow((x-mean),2))/(2*pow(sigm,2)));
    return val;
}
double medium(float *mass, int n)
{
    double med = 0;
    for(int i = 0; i < n; i++)
        med += mass[i];
    return med/(double)n;
}
double rms(float *mass, int n)
{
    double med = medium(mass, n);
    double rms = 0;
    for(int i = 0; i < n; i++)
        rms += pow(mass[i] - med, 2);
    return rms/(double)n;
}

double fft_noiser(double *result, double *SPD[2], unsigned long long n)
{

    Fft_inverseTransform(SPD[0], SPD[1],(size_t)n); //Преобразуем шум из частотной области во временную.

    for(unsigned long long k = 0; k < n; k++)
    {
        result[k] += SPD[0][k]/pow(1.0*n, 0.5);
    }
    return 0;
}

void AddCnoise(double Data[], unsigned long long dis_samples, double fsamp, double SNR, double ESignal, std::vector<noise_param>& noises)
{
//int progress = 0;
double dt = 1.0/fsamp;

    double samn = 0;
    for(unsigned int p = 0; p < noises.size(); p++) // Получаем коэффициент для нормировки относительных амплитуд шума
    {
        samn += (noises[p].relamp)*(noises[p].relamp); // Сумма относительных энергий
    }
    samn = sqrt(samn);

    if (isnan(SNR) == false)
    {
        double ENoise=ESignal/ pow(10,(SNR/10));


            //complex<double> *SPD = new complex<double>[dis_samples/2 + 1];
            double *SPD[2];
            SPD[0] = new double[dis_samples];
            SPD[1] = new double[dis_samples];

            for(unsigned int p = 0; p < noises.size(); p++) // Можно задать несколько гауссовских полос пропускания
            {
                double medium = noises[p].medium, bandwidth = noises[p].bandwidth;
                double sigm = bandwidth/0.83255461115769775635; //Значение СКО, при котором функция Гаусса на частоте среза равна 1/sqrt(2)
                double Fmax = (((dis_samples+1)/2)-1)/((dis_samples - 2)*dt);
                double arg1 = - medium/sigm;
                double arg2 = (Fmax - medium)/sigm;
                double calc_value = (0.5*erf(arg2)+0.5)-(0.5*erf(arg1)+0.5); //Значение площади под Гауссовой кривой с учётом ограничения частотного диапазона, используемое для нормировки спектральной плотности.
                //if(i == numTraces - 1) cout<<calc_value<<"  "<<"The area under the gaussian curve - theoretical value"<<"\n";
                double val;
                if (calc_value>0.01) val = (noises[p].relamp/samn)*pow(noises.size()/(dis_samples*dt)*3.544907701811032*sigm/calc_value*fsamp*ENoise*0.5, 0.5);
                else val = double(sqrt(ENoise/(dis_samples*dt)));
                for(unsigned int k = 0; k < dis_samples/2 + 1; k++)
                {
                    double pdf;
                    if (calc_value>0.01) pdf = normpdf(k/(dt*dis_samples), medium, sigm );
                    else pdf = 1;

                    SPD[0][k] = pdf*val*distribution(generator);
                    SPD[1][k] = pdf*val*distribution(generator);
                }

                for(unsigned long long k = 0; k < dis_samples/2; k++)
                {
                    SPD[0][dis_samples/2 + k] = SPD[0][dis_samples/2 - k];
                    SPD[1][dis_samples/2 + k] = -1.0*SPD[1][dis_samples/2 - k];
                }

                fft_noiser(&Data[0], SPD, dis_samples);

            }
            delete[] SPD[0];
            delete[] SPD[1];
    }

    //double SInterf = 0.1;

}

void AddInterf(double Data[], unsigned long long dis_samples, double fsamp, double SInterf, double ESignal, std::vector<interf_param>& interf)
{
    double EInterf=ESignal/pow(10,(SInterf/20));
    double samsin=0;
    double dt = 1.0/fsamp;
    for(unsigned int p = 0; p < interf.size(); p++)// Получаем коэффициент для нормировки относительных амплитуд помехи
    {
        samsin += (interf[p].relamp)*(interf[p].relamp); // Сумма относительных энергий гармоник помехи
    }
    samsin = sqrt(samsin);
    //double sum = 0;
    if (samsin != 0)
    {
        for(unsigned int p = 0; p < interf.size(); p++) // добавляем синусоиды
        {
            //sum += pow(sinuses[p].relamp/samsin,2)*2/(N*dt);
            double freq = interf[p].frequency, phase = interf[p].phase, amplitude = double(interf[p].relamp/samsin*sqrt(2/(dis_samples*dt)))*sqrt(EInterf);
            if (freq != 0 && amplitude != 0) {
                for(unsigned int j = 0; j < dis_samples; j++)
                    //if(j > 1000 || j < dis_samples - 1000)
                        Data[j] += amplitude*sin(2.0*M_PI*freq*dt*j - phase*M_PI/180.0);
            }
        }
    }
}



