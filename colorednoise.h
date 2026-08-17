
#ifndef COLOREDNOISE_H
#define COLOREDNOISE_H

#endif // COLOREDNOISE_H

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <vector>


struct noise_param //  Параметры шума и помехи
{
    double relamp, medium, bandwidth;
};

struct interf_param
{
    double relamp, frequency, phase;
};

void AddCnoise(double Data[], unsigned long long dis_samples, double fsamp, double SNR, double ESignal, std::vector<noise_param>& noises);
void AddInterf(double Data[], unsigned long long dis_samples, double fsamp, double SInterf, double ESignal, std::vector<interf_param>& interf);
