#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <QLabel>
#include "fftwindow.h"
#include "player.h"
#include "QVector"
#include <QSettings>
#include "colorednoise.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

#define sample_rate 44100.0

struct tab1p
{
    double frequency;
    double loudness;
    double azimuth;
    double rotfreq;
    bool rotation;
};
struct tab2p
{
    double frequency1_2;
    double frequency2_2;
    double loudness1_2;
    double loudness2_2;
    double duration1_2;
    double duration2_2;
    double aptime1_2;
    double aptime2_2;
};
struct tab3p
{
    double frequency_3;
    double loudness_3;
    double modulationfreq_3;
    double freqdeviation_3;
    double moddepth_3;
    bool AM;
    bool PM;
    bool FM;
};
struct pannelp
{
    double signalduration;
    bool left;
    bool right;
    bool smooth;
    double loudmultipler;
    bool addnoise;
    bool addinterf;
    double snr;
    double sinterf;
    QVector<double> noisefreqs;
    QVector<double> noisebands;
    QVector<double> noiseampls;
    QVector<double> interffreqs;
    QVector<double> interfphases;
    QVector<double> interfampls;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    QLabel *status;
    bool LoadResponse(QString name);
    ~MainWindow();

signals:
    void stopAll();

private slots:
   // void on_FrequencySlider_valueChanged(int value);

    void on_PlayButton_clicked();

    void on_SpectrumButton_clicked();

    void on_FrequencyBox_valueChanged(double arg1);

    void on_FrequencySlider_sliderMoved(int position);

    void on_StopButton_clicked();

    void on_OnlyLeftBox_toggled(bool checked);

    void on_OnlyRightBox_toggled(bool checked);

    void on_LoudnessMultEdit_textChanged(const QString &arg1);

    void on_LoudnessBox_valueChanged(double arg1);

    void on_LoudnessSlider_sliderMoved(int position);

    void on_FreqSeqButton_low_clicked();

    void on_FreqSeqButton_high_clicked();

    void on_HearButton_clicked();

    void on_NotHearButton_clicked();

    void on_LoudSeqButton_clicked();

    void on_Duration2Slider_2_sliderMoved(int position);

    void on_Duration2Box_2_valueChanged(double arg1);

    void on_Periods2Box_2_valueChanged(double arg1);

    void on_Loudness2Slider_2_sliderMoved(int position);

    void on_Loudness2Box_2_valueChanged(double arg1);

    void on_Time2Slider_2_sliderMoved(int position);

    void on_Time2Box_2_valueChanged(double arg1);

    void on_DeleteStringButton_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_DurationEdit_textChanged(const QString &arg1);

    void on_DeviationSlider_3_sliderMoved(int position);

    void on_DeviationBox_3_valueChanged(double arg1);

    void on_DepthSlider_3_sliderMoved(int position);

    void on_DepthBox_3_valueChanged(double arg1);

    void on_AMBox_toggled(bool checked);

    void on_PMBox_toggled(bool checked);

    void on_FMBox_toggled(bool checked);

    void on_Freq2Edit_2_editingFinished();

    void on_Duration1Edit_2_textChanged(const QString &arg1);

    void on_Periods1Edit_2_textChanged(const QString &arg1);

    void on_Time1Edit_2_textChanged(const QString &arg1);

    void on_Freq1Edit_2_textChanged(const QString &arg1);

    void on_HearButton_2_clicked();

    void on_HearButton_3_clicked();

    void on_PeriodSeqButton_2_clicked();

    void on_TimeSeqButton_2_clicked();

    void on_LoudSeqButton_2_clicked();

    void on_AMSeqButton_clicked();

    void on_PMSeqButton_clicked();

    void on_FMSeqButton_3_clicked();

    void on_SaveResultButton_clicked();

    void on_SaveResultButton_2_clicked();

    void on_SaveResultButton_3_clicked();

    void on_DeleteStringButton_2_clicked();

    void on_DeleteStringButton_3_clicked();

public slots:
        void GetErr(QString err);
        void DFinish(QString err);


private:
    Ui::MainWindow *ui;
    FFTWindow *fftwindow;
    tab1p *tab1params;
    tab2p *tab2params;
    tab3p *tab3params;
    pannelp *pannelparams;
    QSettings *settings;
    QString err;
    QString lastPath;
    void LoadSettings();
    void SaveSettings();
    bool stopflag;
    bool stopflag2;
    bool stopflag3;
    bool switch1;
    vector<double> freqs;
    vector<double> values;
    bool dialog_finished;
    void RefreshFromUi1(tab1p *p1);
    void RefreshFromUi2(tab2p *p2);
    void RefreshFromUi3(tab3p *p3);
    void RefreshPannel(pannelp *pn);
    void GetNoiseVector(vector<noise_param> *noises);
    void GetInterfVector(vector<interf_param> *interf);
    void tab3_sequences(QString type);
    void Play_buffer(short *samples, ALsizei buf_size, double azimuth, double distance, double rotfreq, ALenum format, double volume);
    void StopThreads();
};
#endif // MAINWINDOW_H