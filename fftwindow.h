#ifndef FFTWINDOW_H
#define FFTWINDOW_H

#include <QDialog>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <QSettings>

namespace Ui {
class FFTWindow;
}

class FFTWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FFTWindow(QWidget *parent = nullptr);
    void SetData(double *sigdata, double *specdata, double sampfreq, unsigned long long samples);
    void SetSettings(QSettings *settings);
    ~FFTWindow();


signals:
    void finalize(QString err);

private slots:
    void on_SaveButton_clicked();


    void on_UpdataSignalButton_clicked();

    void on_UpdateSpectrButton_clicked();

private:
    Ui::FFTWindow *ui;
    QwtPlotCurve curv_sig;
    QwtPlotGrid grid_sig;
    QwtPlotCurve curv_spec;
    QwtPlotGrid grid_spec;
    double *sigdata;
    double *specdata;
    double *timedata;
    double *freqdata;
    QSettings *settings;
    double sampfreq;
    double dt;
    double df;
    unsigned long long samples;
    void RedrawSignal(unsigned long long first, unsigned long long count);
    void RedrawSpectr(unsigned long long first, unsigned long long count);
    void reject();
    //ui->
};

#endif // FFTWINDOW_H
