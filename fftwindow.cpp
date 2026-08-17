#include "fftwindow.h"
#include "ui_fftwindow.h"
#include <QFileDialog>
#include <QFile>

FFTWindow::FFTWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FFTWindow)
{
    ui->setupUi(this);
    QWidget::setWindowTitle ("Модельный сигнал и спектр");
    ui->SignalPlot->setTitle( "Сигнал" );
    ui->SpectrPlot->setTitle( "Спектр" );
    ui->SignalPlot->setAxisTitle(QwtPlot::yLeft, "Значения");
    ui->SignalPlot->setAxisTitle(QwtPlot::xBottom, "Время, мс.");
    ui->SpectrPlot->setAxisTitle(QwtPlot::yLeft, "Значения");
    ui->SpectrPlot->setAxisTitle(QwtPlot::xBottom, "Частота, Гц.");

    this->grid_sig.setMajorPen(QPen(Qt::black, 0, Qt::SolidLine));
    this->grid_sig.setMinorPen(QPen(Qt::gray, 0, Qt::SolidLine));
    this->grid_sig.attach(ui->SignalPlot);
    this->curv_sig.setRenderHint(QwtPlotItem::RenderAntialiased);
    this->curv_sig.setPen(QPen(Qt::blue));
    this->curv_sig.attach(ui->SignalPlot);
    ui->SignalPlot->canvas()->setCursor(Qt::ArrowCursor);

    this->grid_spec.setMajorPen(QPen(Qt::black, 0, Qt::SolidLine));
    this->grid_spec.setMinorPen(QPen(Qt::gray, 0, Qt::SolidLine));
    this->grid_spec.attach(ui->SpectrPlot);
    this->curv_spec.setRenderHint(QwtPlotItem::RenderAntialiased);
    this->curv_spec.setPen(QPen(Qt::blue));
    this->curv_spec.attach(ui->SpectrPlot);
    ui->SpectrPlot->canvas()->setCursor(Qt::ArrowCursor);

    this->samples = 0;
}

FFTWindow::~FFTWindow()
{
    if (this->samples != 0)
    {
        delete [] timedata;
        delete [] freqdata;
    }
    delete ui;
}

void FFTWindow::reject()
{
     emit finalize("Noerror");
     QDialog::reject();
}

void CutPath2(QString *path)
{
    int i = path->length();
    while (i > 0)
    {
        if (path->at(i-1) == "\\" || path->at(i-1) == "/") break;
        i--;
    }
    *path = path->left(i);
}

void FFTWindow::on_SaveButton_clicked()
{
    if (this->samples == 0) return;
    QString path = this->settings->value("lastPath","C://").toString();
    QString namestr = QFileDialog::getSaveFileName(this, ("Сохранить сигнал и спектр в файл"), path , "Text files (*.txt)");
    if (namestr.length() == 0) return;
    QFile file;
    file.setFileName(namestr);
    if (!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    QString temp = "time | signal | freq | spectr";
    out<<temp<<"\n";
    for (unsigned long long k = 0; k < samples/2; k++) out<< timedata[k] <<"\t"<< sigdata[k] <<"\t"<< freqdata[k] <<"\t"<< specdata[k] <<"\r\n";
    for (unsigned long long k = samples/2; k < samples; k++) out<< timedata[k] <<"\t"<< sigdata[k] <<"\r\n";
    file.close();
    CutPath2(&namestr);
    this->settings->setValue("lastPath",namestr);
}

void FFTWindow::SetData(double *sigdata, double *specdata, double sampfreq, unsigned long long samples)
{
    this->sigdata = sigdata;
    this->specdata = specdata;
    this->sampfreq = sampfreq;
    this->samples = samples;
    ui->Time1Box->setMaximum(1000.0*samples/sampfreq-1);
    ui->Time2Box->setMaximum(1000.0*samples/sampfreq);
    ui->Freq1Box->setMaximum(sampfreq/2.0-10);
    ui->Freq2Box->setMaximum(sampfreq/2.0);
    ui->Time1Box->setValue(0);
    ui->Time2Box->setValue(1000.0*samples/sampfreq);
    ui->Freq1Box->setValue(0);
    ui->Freq2Box->setValue(sampfreq/2.0);
    timedata = new double[samples];
    freqdata = new double[samples/2];
    dt = 1000/sampfreq;
    df = sampfreq/(samples*1.0);
    for(unsigned long long k = 0; k < samples/2; k++)
    {
        timedata[k] = k*dt;
        freqdata[k] = k*df;
    }
    for(unsigned long long k = samples/2; k < samples; k++)
    {
        timedata[k] = k*dt;
    }

    unsigned long long first = ui->Time1Box->value()/dt;
    unsigned long long count =  ui->Time2Box->value()/dt - first;
    RedrawSignal(first, count);

    first = ui->Freq1Box->value()/df;
    count =  ui->Freq2Box->value()/df - first;
    RedrawSpectr(first, count);
}

void FFTWindow::SetSettings(QSettings *settings)
{
    this->settings = settings;
}

void FFTWindow::RedrawSignal(unsigned long long first, unsigned long long count)
{
    this->curv_sig.detach();
    this->curv_sig.setStyle(QwtPlotCurve::Lines);
    this->curv_sig.setPen(QPen(Qt::blue,3.0));
    this->curv_sig.setSamples(&timedata[first],&sigdata[first],count);
    this->curv_sig.attach(ui->SignalPlot);
    ui->SignalPlot->replot();
}
void FFTWindow::RedrawSpectr(unsigned long long first, unsigned long long count)
{
    this->curv_spec.detach();
    this->curv_spec.setStyle(QwtPlotCurve::Lines);
    this->curv_spec.setPen(QPen(Qt::blue,3.0));
    this->curv_spec.setSamples(&freqdata[first],&specdata[first],count);
    this->curv_spec.attach(ui->SpectrPlot);
    ui->SpectrPlot->replot();
}

void FFTWindow::on_UpdataSignalButton_clicked()
{
    unsigned long long first = ui->Time1Box->value()/dt;
    unsigned long long count =  ui->Time2Box->value()/dt - first;
    RedrawSignal(first, count);
}

void FFTWindow::on_UpdateSpectrButton_clicked()
{
    unsigned long long first = ui->Freq1Box->value()/df;
    unsigned long long count =  ui->Freq2Box->value()/df - first;
    RedrawSpectr(first, count);
}
