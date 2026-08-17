#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fftwindow.h"
#include "create_signal.h"
#include <combaseapi.h>
#include <QFileDialog>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>
#include <QFile>
#include <chrono>
#if defined (__cplusplus)
extern "C" { //Подключение заголовочного файла на языке "С"
#endif
#include <fft.h>
bool Fft_transform(double real[], double imag[], size_t n);
//bool Fft_inverseTransform(double real[], double imag[], size_t n);
#if defined (__cplusplus)
}
#endif

void MainWindow::LoadSettings()
{
    ui->LoudnessMultEdit->setText(this->settings->value("LoudnessMultEdit","1").toString());
    ui->CompenseBox->setChecked(this->settings->value("Compense").toBool());
    ui->SmoothBox->setChecked(this->settings->value("Smooth").toBool());
    this->lastPath = this->settings->value("lastPath","C://").toString();
}

void MainWindow::SaveSettings()
{
    this->settings->setValue("LoudnessMultEdit", ui->LoudnessMultEdit->text());
    this->settings->setValue("Compense", ui->CompenseBox->isChecked());
    this->settings->setValue("Smooth", ui->SmoothBox->isChecked());
    this->settings->setValue("lastPath", lastPath);
    this->settings->sync();
}

bool MainWindow::LoadResponse(QString name)
{
    QFile InFile(name);
    if (!InFile.exists())
    {
        freqs.push_back(0.0);
        freqs.push_back(22000.0);
        values.push_back(1.0);
        values.push_back(1.0);
        return 0;
    }
    if (!InFile.open(QIODevice::ReadOnly | QIODevice::Text)) return 0;
    QTextStream in(&InFile);
    QStringList tempstr;
    while(in.atEnd() == false)
    {
            tempstr = in.readLine().split("\t");
            int le = tempstr.length()-1;
            values.push_back(tempstr.at(le).toDouble());
            freqs.push_back(tempstr.at(le-1).toDouble());
    }
    return 1;
}

void MainWindow::RefreshPannel(pannelp *pn)
{
    pn->signalduration = ui->DurationEdit->text().toDouble();
    if (pn->signalduration == 0)
    {
        this->status->setText("Ошибка ввода длительности сигнала. Установлено Т = 5 с.");
        pn->signalduration = 5;
    }
    if (ui->OnlyLeftBox->isChecked())
    {
     pn->left = true;
     pn->right = false;
    }
    else if (ui->OnlyRightBox->isChecked())
    {
     pn->left = false;
     pn->right = true;
    }
    else
    {
     pn->right = true;
     pn->left = true;
    }
    pn->loudmultipler = ui->LoudnessMultEdit->text().toDouble();
    pn->addnoise = ui->AddNoiseBox->isChecked();
    pn->addinterf = ui->AddInterfBox->isChecked();
    pn->snr = ui->SNREdit->text().toDouble();
    pn->sinterf = ui->SInterfEdit->text().toDouble();
    pn->smooth = ui->SmoothBox->isChecked();

    QStringList temp = ui->NoisesFreqEdit->text().split(";");
    bool ok;
    double tempn;
    pn->noisebands.clear();
    pn->noiseampls.clear();
    pn->noisefreqs.clear();
    pn->interfampls.clear();
    pn->interffreqs.clear();
    pn->interfphases.clear();

    for (int i = 0; i < temp.size(); i++)
    {
        tempn = temp[i].toDouble(&ok);
        if (ok == true) pn->noisefreqs.append(tempn);
    }

    temp = ui->NoisesBandEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        tempn = temp[i].toDouble(&ok);
        if (ok == true) pn->noisebands.append(tempn);
    }

    temp = ui->NoisesRelampEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        tempn = temp[i].toDouble(&ok);
        if (ok == true) pn->noiseampls.append(tempn);
    }

    temp = ui->InterfFreqEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        tempn = temp[i].toDouble(&ok);
        if (ok == true) pn->interffreqs.append(tempn);
    }

    temp = ui->InterfLevelEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        tempn = temp[i].toDouble(&ok);
        if (ok == true) pn->interfphases.append(tempn);
    }

    temp = ui->InterfRelampEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        tempn = temp[i].toDouble(&ok);
        if (ok == true) pn->interfampls.append(tempn);
    }
    temp.clear();
}

void MainWindow::RefreshFromUi1(tab1p *p1)
{
    p1->frequency = ui->FrequencyBox->value();
    p1->loudness = ui->LoudnessBox->value();
    p1->azimuth = ui->AzimuthEdit->text().toDouble();
    p1->rotation = ui->RotationBox->isChecked();
    p1->rotfreq = ui->RotFreqEdit->text().toDouble();
}

void MainWindow::RefreshFromUi2(tab2p *p2)
{
    p2->frequency1_2 = ui->Freq1Edit_2->text().toDouble();
    p2->frequency2_2 = ui->Freq2Edit_2->text().toDouble();
    p2->loudness1_2 = ui->Loudness1Edit_2->text().toDouble();
    p2->loudness2_2 = ui->Loudness2Box_2->value();
    p2->aptime1_2 = ui->Time1Edit_2->text().toDouble();
    p2->aptime2_2 = ui->Time2Box_2->value();
    p2->duration1_2 = ui->Duration1Edit_2->text().toDouble();
    p2->duration2_2 = ui->Duration2Box_2->value();
}

void MainWindow::RefreshFromUi3(tab3p *p3)
{
    p3->frequency_3 = ui->FrequencyEdit_3->text().toDouble();
    p3->loudness_3 = ui->LoudnessEdit_3->text().toDouble();
    p3->modulationfreq_3 = ui->ModFreqEdit_3->text().toDouble();
    p3->freqdeviation_3 = ui->DeviationBox_3->value();
    p3->moddepth_3 = ui->DepthBox_3->value();
    p3->AM = ui->AMBox->isChecked();
    p3->PM = ui->PMBox->isChecked();
    p3->FM = ui->FMBox->isChecked();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->tabBar()->setExpanding(true);
    ui->tabWidget->tabBar()->setElideMode(Qt::ElideNone);
    ui->tabWidget->tabBar()->setUsesScrollButtons(false);
    this->status = new QLabel(this);
    ui->statusbar->addWidget(status);
    this->status->setText("Готов.");
    QWidget::setWindowTitle("SoundLab");

    // --- Modern toolbar with usability actions ---------------------------
    QToolBar *toolbar = addToolBar("Главное");
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);

    QAction *actReset = toolbar->addAction("Сброс параметров");
    actReset->setStatusTip("Восстановить значения параметров по умолчанию");
    connect(actReset, &QAction::triggered, this, [this]() {
        ui->LoudnessMultEdit->setText("1");
        ui->DurationEdit->setText("5");
        ui->AzimuthEdit->setText("0");
        ui->SNREdit->setText("20");
        ui->SInterfEdit->setText("20");
        ui->OnlyLeftBox->setChecked(false);
        ui->OnlyRightBox->setChecked(false);
        ui->NoisesFreqEdit->clear();
        ui->NoisesBandEdit->clear();
        ui->NoisesRelampEdit->clear();
        ui->InterfFreqEdit->clear();
        ui->InterfLevelEdit->clear();
        ui->InterfRelampEdit->clear();
        ui->ResultBrowser->clear();
        this->status->setText("Параметры сброшены.");
    });

    QAction *actAbout = toolbar->addAction("О программе");
    actAbout->setStatusTip("Сведения о приложении");
    connect(actAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, "О программе",
                           "<b>SoundLab</b><br/>"
                           "Учебное приложение для лабораторной работы по "
                           "исследованию частотных и слуховых параметров сигнала.<br/><br/>"
                           "Сборка: CMake + Qt 5 + Qwt + OpenAL.");
    });

    // --- Tooltips for the main controls ----------------------------------
    ui->PlayButton->setToolTip("Воспроизвести сигнал текущей вкладки");
    ui->StopButton->setToolTip("Остановить воспроизведение");
    ui->SpectrumButton->setToolTip("Показать сигнал и его спектр");
    ui->LoudnessMultEdit->setToolTip("Множитель громкости (0..1)");
    ui->DurationEdit->setToolTip("Длительность сигнала, с");
    ui->AzimuthEdit->setToolTip("Азимут источника звука, град");

    settings = new QSettings;
    fftwindow = new FFTWindow(0);
    fftwindow->SetSettings(settings);
    connect(fftwindow, SIGNAL(finalize(QString)), SLOT(DFinish(QString)));
    fftwindow->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint)& ~Qt::WindowMinimizeButtonHint);

    tab1params = new tab1p;
    tab2params = new tab2p;
    tab3params = new tab3p;
    pannelparams = new pannelp;
    this->LoadSettings();
    ui->tabWidget->setCurrentIndex(0);
    stopflag = 1;
    stopflag2 = 1;
    stopflag3 = 1;
    switch1 = 0;
    dialog_finished = 0;

    ui->ResultBrowser->setText("Частота_Гц.\tГромкость_дБ.\n");

    ui->ResultBrowser_2->setText("Длительн1_мс.\tДлительн2_мс.\tГромкость1_дБ\tГромкость2_дБ\tВремя1_мс.\tВремя2_мс.\tЧастота1_Гц.\tЧастота2_Гц.\n");

    ui->ResultBrowser_3->setText("Частота_Гц.\tЧастота_м.,_Гц.\tГлубина модуляции/Девиация частоты_Гц.\n");

    bool noer = LoadResponse("Response.txt");
    if (noer == false) this->status->setText("Файл калибровки не найден.");

    double m = 1e99;
    for (int k = 0; k < int(freqs.size()); k++)
    {
        if (values[k] < m) m = values[k];
    }
    for (int k = 0; k < int(freqs.size()); k++)
    {
        values[k] = pow(10.0,(m-values[k])/20.0);
    }

    QFile file("text1.htm");//Загрузка текста с описанием работ из HTML файла.
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->WorkBrowser->setHtml(file.readAll());
    file.close();

    file.setFileName("text2.htm");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->WorkBrowser_2->setHtml(file.readAll());
    file.close();

    file.setFileName("text3.htm");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        ui->WorkBrowser_3->setHtml(file.readAll());
    file.close();
}

MainWindow::~MainWindow()
{
    this->SaveSettings();
    stopflag3 = 0;
    StopThreads();
    delete (fftwindow);
    delete (tab1params);
    delete (tab2params);
    delete (tab3params);

    freqs.clear();
    values.clear();

    pannelparams->noisefreqs.clear();
    pannelparams->noisebands.clear();
    pannelparams->noiseampls.clear();
    pannelparams->interffreqs.clear();
    pannelparams->interfphases.clear();
    pannelparams->interfampls.clear();

    delete (pannelparams);
    delete (settings);
    delete ui;
}

void MainWindow::Play_buffer(short *samples, ALsizei buf_size, double azimuth, double distance, double rotfreq, ALenum format, double volume)
{
    QThread *thread = new QThread;
    PlayThread *plth = new PlayThread;
    plth->SetData(samples, buf_size, sample_rate, format);
    if (format == AL_FORMAT_MONO16)
    {
        //double Xpos = distance*sin(azimuth);
        //double Ypos = distance*cos(azimuth);
        //plth->SetPosition(Xpos, Ypos, 0);
        plth->SetPosition(azimuth, distance, rotfreq);
    }

    plth->SetVolume(volume);

    plth->moveToThread(thread);
    connect(thread, SIGNAL(started()), plth, SLOT(process()));
    connect(plth, SIGNAL(finished()), thread, SLOT(quit()));
    connect(this, SIGNAL(stopAll()), plth, SLOT(stop()));
    connect(plth, SIGNAL(finished()), plth, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(plth, SIGNAL(ErRet(QString)), this, SLOT(GetErr(QString)));
    thread->start();
    //plth->process();
    while (thread->isFinished() == 0)
    {
        QCoreApplication::processEvents();
    }

    StopThreads();

}


//-----------------------------------------------------------------------------------------------------------------------------Basic control


void MainWindow::on_OnlyLeftBox_toggled(bool checked)
{
    if (checked == true) ui->OnlyRightBox->setChecked(0);
}

void MainWindow::on_OnlyRightBox_toggled(bool checked)
{
     if (checked == true) ui->OnlyLeftBox->setChecked(0);
}


void MainWindow::on_LoudnessMultEdit_textChanged(const QString &arg1)
{
    if (arg1.toDouble() > 1) ui->LoudnessMultEdit->setText(QString::number(1));
}

void MainWindow::GetErr(QString err)
{
    this->err = err;
}
void  MainWindow::DFinish(QString err)
{
    if (err == "Noerror")  this->dialog_finished = 1;
}


void MainWindow::StopThreads()  /* принудительная остановка всех потоков */
{
    emit  stopAll();
}


void MainWindow::on_StopButton_clicked()
{
    if (stopflag2 == 0) return;
    stopflag2 = 0;
    stopflag3 = 0;//Остановить все циклы
    StopThreads();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stopflag2 = 1;//Остановить воспроизведение
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
  if (index == 1)
  {
  pannelparams->signalduration = ui->DurationEdit->text().toDouble();
  ui->Duration2Slider_2->setMaximum(int(pannelparams->signalduration*1000000 - ui->Time2Box_2->value()));
  ui->Duration2Box_2->setMaximum(pannelparams->signalduration*1000000 - ui->Time2Box_2->value());
  ui->Periods2Box_2->setMaximum((pannelparams->signalduration - ui->Time2Box_2->value())*ui->Freq2Edit_2->text().toDouble());
  ui->Time2Slider_2->setMaximum(int(pannelparams->signalduration*1000000));
  ui->Time2Box_2->setMaximum(int(pannelparams->signalduration*1000000));
  ui->Periods2Box_2->setMaximum(ui->Duration2Box_2->maximum()/1000*ui->Freq2Edit_2->text().toDouble());
  tab2params->aptime1_2 = ui->Time1Edit_2->text().toDouble();
  tab2params->frequency1_2 = ui->Freq1Edit_2->text().toDouble();
  tab2params->duration1_2 = ui->Duration1Edit_2->text().toDouble();
  }
  if (index == 1) ui->SmoothBox->setDisabled(1);
  else ui->SmoothBox->setDisabled(0);
}

void MainWindow::on_DurationEdit_textChanged(const QString &arg1)
{
    pannelparams->signalduration = arg1.toDouble();
    ui->Duration2Slider_2->setMaximum(int(pannelparams->signalduration*1000000 - ui->Time2Box_2->value()));
    ui->Duration2Box_2->setMaximum(pannelparams->signalduration*1000000 - ui->Time2Box_2->value());
    ui->Periods2Box_2->setMaximum((pannelparams->signalduration - ui->Time2Box_2->value())*ui->Freq2Edit_2->text().toDouble());
    ui->Time2Slider_2->setMaximum(int(pannelparams->signalduration*1000000));
    ui->Time2Box_2->setMaximum(int(pannelparams->signalduration*1000000));
    ui->Periods2Box_2->setMaximum(ui->Duration2Box_2->maximum()/1000*ui->Freq2Edit_2->text().toDouble());
}

void MainWindow::on_AMBox_toggled(bool checked)
{
    if (checked == true)
    {
        ui->FMBox->setChecked(false);
        ui->PMBox->setChecked(false);
    }
}

void MainWindow::on_PMBox_toggled(bool checked)
{
    if (checked == true)
    {
        ui->AMBox->setChecked(false);
        ui->FMBox->setChecked(false);
    }
}

void MainWindow::on_FMBox_toggled(bool checked)
{
    if (checked == true)
    {
        ui->AMBox->setChecked(false);
        ui->PMBox->setChecked(false);
    }
}

// ---------------------------------------------------------------------------------------------------------------------------- First tab

void MainWindow::on_FreqSeqButton_low_clicked()
{
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    stopflag3 = 1;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);//Загружаем параметры интерфейса
    double seconds = pannelparams->signalduration;

    ALsizei buf_size = ceil(seconds * sample_rate*4);
    short *samples = new short[buf_size];
    double *signal_left = new double[buf_size/2];
    double *signal_right = new double[buf_size/2];

    double ampl = 1/pow(10.0,(100.0 - tab1params->loudness)/20.0);

    for (int k = 15; k < 40; k++)
    {
            ui->FrequencyBox->setValue(k);
            tab1params->frequency = k;

        CreateSignal1Stereo(signal_left, signal_right, buf_size/4, tab1params->frequency, tab1params->azimuth, tab1params->rotfreq, tab1params->rotation, pannelparams->left, pannelparams->right, pannelparams->smooth, ampl);

        if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
        {
            vector<noise_param>noises;
            GetNoiseVector(&noises);
            AddCnoise(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
            AddCnoise(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises); //Добавляем шум.
            noises.clear();
        }

        if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
        {
            vector<interf_param>interf;
            GetInterfVector(&interf);
            AddInterf(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
            AddInterf(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);//Добавляем помехи.
            interf.clear();
        }

        if (ui->CompenseBox->isChecked() == 1)
        {
            if (ui->AddInterfBox->isChecked() == 1 || ui->AddNoiseBox->isChecked() == 1)
            {
               attenuator(signal_left, buf_size/4, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
               attenuator(signal_right, buf_size/4, 1.0*sample_rate, &freqs, &values);
               WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2);
               Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
            }
            else
            {
                WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Делаем упрощённую компенсацию (учёт множителя затухания)
                Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler*GetfadeValue(tab1params->frequency, &freqs, &values));
            }
        }
        else
        {
            WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Не делаем компенсацию
            Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
        }

            QCoreApplication::processEvents();
            if (stopflag3 == 0) break;
    }

    delete [] signal_left;
    delete [] signal_right;
    delete [] samples;

    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");

     stopflag = 1;
}



void MainWindow::on_FreqSeqButton_high_clicked()
{

    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    stopflag3 = 1;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);//Загружаем параметры интерфейса
    double seconds = pannelparams->signalduration;

    ALsizei buf_size = ceil(seconds * sample_rate*4);
    short *samples = new short[buf_size];
    double *signal_left = new double[buf_size/2];
    double *signal_right = new double[buf_size/2];

    double ampl = 1/pow(10.0,(100.0 - tab1params->loudness)/20.0);

    for (int k = 12000; k < 22500; k+=500)
    {
            ui->FrequencyBox->setValue(k);
            tab1params->frequency = k;

        CreateSignal1Stereo(signal_left, signal_right, buf_size/4, tab1params->frequency, tab1params->azimuth, tab1params->rotfreq, tab1params->rotation, pannelparams->left, pannelparams->right, pannelparams->smooth, ampl);

        if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
        {
            vector<noise_param>noises;
            GetNoiseVector(&noises);
            AddCnoise(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
            AddCnoise(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises); //Добавляем шум.
            noises.clear();
        }

        if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
        {
            vector<interf_param>interf;
            GetInterfVector(&interf);
            AddInterf(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
            AddInterf(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);//Добавляем помехи.
            interf.clear();
        }

        if (ui->CompenseBox->isChecked() == 1)
        {
            if (ui->AddInterfBox->isChecked() == 1 || ui->AddNoiseBox->isChecked() == 1)
            {
               attenuator(signal_left, buf_size/4, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
               attenuator(signal_right, buf_size/4, 1.0*sample_rate, &freqs, &values);
               WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2);
               Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
            }
            else
            {
                WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Делаем упрощённую компенсацию (учёт множителя затухания)
                Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler*GetfadeValue(tab1params->frequency, &freqs, &values));
            }
        }
        else
        {
            WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Не делаем компенсацию
            Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
        }
            QCoreApplication::processEvents();
            if (stopflag3 == 0) break;
    }

    delete [] signal_left;
    delete [] signal_right;
    delete [] samples;

    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");

     stopflag = 1;

}

void MainWindow::on_LoudSeqButton_clicked()
{

    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    stopflag3 = 1;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);//Загружаем параметры интерфейса
    double seconds = pannelparams->signalduration;

    ALsizei buf_size = ceil(seconds * sample_rate*4);
    short *samples = new short[buf_size];
    double *signal_left = new double[buf_size/2];
    double *signal_right = new double[buf_size/2];

    double ampl;


    for (int k = 20; k < 71; k ++)
    {
            tab1params->loudness = k;
            ui->LoudnessBox->setValue(tab1params->loudness);
            ampl = 1/pow(10.0,(100.0 - tab1params->loudness)/20.0);

        CreateSignal1Stereo(signal_left, signal_right, buf_size/4, tab1params->frequency, tab1params->azimuth, tab1params->rotfreq, tab1params->rotation, pannelparams->left, pannelparams->right, pannelparams->smooth, ampl);

        if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
        {
            vector<noise_param>noises;
            GetNoiseVector(&noises);
            AddCnoise(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
            AddCnoise(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises); //Добавляем шум.
            noises.clear();
        }

        if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
        {
            vector<interf_param>interf;
            GetInterfVector(&interf);
            AddInterf(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
            AddInterf(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);//Добавляем помехи.
            interf.clear();
        }

        if (ui->CompenseBox->isChecked() == 1)
        {
            if (ui->AddInterfBox->isChecked() == 1 || ui->AddNoiseBox->isChecked() == 1)
            {
               attenuator(signal_left, buf_size/4, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
               attenuator(signal_right, buf_size/4, 1.0*sample_rate, &freqs, &values);
               WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2);
               Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
            }
            else
            {
                WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Делаем упрощённую компенсацию (учёт множителя затухания)
                Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler*GetfadeValue(tab1params->frequency, &freqs, &values));
            }
        }
        else
        {
            WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Не делаем компенсацию
            Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
        }

        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;
    }

    delete [] signal_left;
    delete [] signal_right;
    delete [] samples;

    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");

     stopflag = 1;

}

void MainWindow::on_FrequencyBox_valueChanged(double arg1)
{
    ui->FrequencySlider->setValue(int(log10(arg1)*10000.0));
}

void MainWindow::on_FrequencySlider_sliderMoved(int position)
{
    if (position >= 43424)
    {
        ui->FrequencyBox->setValue(22000);
    }
    else
    {
    ui->FrequencyBox->setValue(pow(10,(position/10000.0)));
    }
}

void MainWindow::on_LoudnessBox_valueChanged(double arg1)
{
    ui->LoudnessSlider->setValue(arg1*10);
}

void MainWindow::on_LoudnessSlider_sliderMoved(int position)
{
    ui->LoudnessBox->setValue(position/10.0);
}

void MainWindow::on_HearButton_clicked()
{
    QString out = QString::number(tab1params->frequency);
    out.append("\t");
    out.append(QString::number(tab1params->loudness));
    out.append("\t");
    out.append(" Звук уже слышен");
    ui->ResultBrowser->append(out);
    if (stopflag2 == 0) return;
    stopflag2 = 0;
    stopflag3 = 0;//Остановить все циклы
    StopThreads();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stopflag2 = 1;//Остановить воспроизведение
}

void MainWindow::on_NotHearButton_clicked()
{
    QString out = QString::number(tab1params->frequency);
    out.append("\t");
    out.append(QString::number(tab1params->loudness));
    out.append("\t");
    out.append(" Звук уже не слышен");
    ui->ResultBrowser->append(out);
    if (stopflag2 == 0) return;
    stopflag2 = 0;
    stopflag3 = 0;//Остановить все циклы
    StopThreads();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stopflag2 = 1;//Остановить воспроизведение
}


// ---------------------------------------------------------------------------------------------------------------------------- Second tab

void MainWindow::on_Duration2Slider_2_sliderMoved(int position)
{
    ui->Duration2Box_2->setValue(position/1000.0);
    ui->Time2Box_2->setMaximum(pannelparams->signalduration*1000 - position/1000.0);
}

void MainWindow::on_Duration2Box_2_valueChanged(double arg1)
{
    ui->Duration2Slider_2->setValue(int(arg1*1000));
    ui->Periods2Box_2->setValue(arg1/1000*ui->Freq2Edit_2->text().toDouble());

}

void MainWindow::on_Periods2Box_2_valueChanged(double arg1)
{
    ui->Duration2Box_2->setValue((arg1/ui->Freq2Edit_2->text().toDouble())*1000);
    //ui->Duration2Slider_2->setValue((arg1/ui->Freq2Edit_2->text().toDouble())*1000);
}

void MainWindow::on_Loudness2Slider_2_sliderMoved(int position)
{
    ui->Loudness2Box_2->setValue(position/10.0);
}

void MainWindow::on_Loudness2Box_2_valueChanged(double arg1)
{
    ui->Loudness2Slider_2->setValue(int(arg1*10));
}

void MainWindow::on_Time2Slider_2_sliderMoved(int position)
{
    ui->Time2Box_2->setValue(position/1000.0);
    ui->Duration2Box_2->setMaximum(pannelparams->signalduration*1000 - position/1000.0);
    ui->Periods2Box_2->setMaximum(ui->Duration2Box_2->maximum()/1000*ui->Freq2Edit_2->text().toDouble());
}

void MainWindow::on_Time2Box_2_valueChanged(double arg1)
{
    ui->Time2Slider_2->setValue(arg1*1000);
}

void MainWindow::on_Freq2Edit_2_editingFinished()
{
    ui->Periods2Box_2->setMaximum(ui->Duration2Box_2->maximum()/1000*ui->Freq2Edit_2->text().toDouble());
    ui->Periods2Box_2->setValue(ui->Duration2Box_2->value()/1000*ui->Freq2Edit_2->text().toDouble());
}

void MainWindow::on_Duration1Edit_2_textChanged(const QString &arg1)
{
    double tempd = arg1.toDouble();
    if (tempd < 0) tempd = 0;
    double maxtempd = pannelparams->signalduration*1000 - tab2params->aptime1_2;
    if (tempd > maxtempd) tempd = maxtempd;
    tab2params->duration1_2 = tempd;
    ui->Duration1Edit_2->setText(QString::number(tempd));
    ui->Periods1Edit_2->setText(QString::number(tempd*tab2params->frequency1_2/1000));
}

void MainWindow::on_Periods1Edit_2_textChanged(const QString &arg1)
{
    ui->Duration1Edit_2->setText(QString::number((arg1.toDouble()/tab2params->frequency1_2*1000)));
}

void MainWindow::on_Time1Edit_2_textChanged(const QString &arg1)
{
    double tempd = arg1.toDouble();
    if (tempd < 0) tempd = 0;
    double maxtempd = pannelparams->signalduration*1000 - tab2params->duration1_2;
    if (tempd > maxtempd) tempd = maxtempd;
    ui->Time1Edit_2->setText(QString::number(tempd));
    tab2params->aptime1_2 = tempd;
}

void MainWindow::on_Freq1Edit_2_textChanged(const QString &arg1)
{
    tab2params->frequency1_2 = arg1.toDouble();
    ui->Periods1Edit_2->setText(QString::number(tab2params->duration1_2*tab2params->frequency1_2/1000));
}

void MainWindow::on_HearButton_2_clicked() //Если слышим звук, сохраняем результат.
{
    QString out = QString::number(tab2params->duration1_2);
    out.append("\t");
    out.append(QString::number(tab2params->duration2_2));
    out.append("\t");
    out.append(QString::number(tab2params->loudness1_2));
    out.append("\t");
    out.append(QString::number(tab2params->loudness2_2));
    out.append("\t");
    out.append(QString::number(tab2params->aptime1_2));
    out.append("\t");
    out.append(QString::number(tab2params->aptime2_2));
    out.append("\t");
    out.append(QString::number(tab2params->frequency1_2));
    out.append("\t");
    out.append(QString::number(tab2params->frequency2_2));
    ui->ResultBrowser_2->append(out);
    if (stopflag2 == 0) return;
    stopflag2 = 0;
    stopflag3 = 0;//Остановить все циклы
    StopThreads();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stopflag2 = 1;//Остановить воспроизведение
}

void MainWindow::on_PeriodSeqButton_2_clicked()
{
    ui->DurationEdit->setText(QString::number(1));
    pannelparams->signalduration = 1;
    stopflag3 = 1;
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi2(tab2params);
    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    ALenum format;
    short *samples;
    ALsizei buf_size;

    format = AL_FORMAT_MONO16;
    buf_size = ceil(seconds * sample_rate*2);
    samples = new short[buf_size];
    double *signal = new double[buf_size/2];
    double ampl1 = 1/pow(10.0,(100.0 - tab2params->loudness1_2)/20.0);
    double ampl2 = 1/pow(10.0,(100.0 - tab2params->loudness2_2)/20.0);


    for(int k = 300; k>=10; k = k - 1)
    {
        ui->Periods2Box_2->setValue(k);
        tab2params->duration2_2 = ui->Duration2Box_2->value();

        CreateSignal2Mono(signal,  buf_size/2, tab2params->duration1_2/1000.0, tab2params->duration2_2/1000.0, ampl1, ampl2,  tab2params->aptime1_2/1000.0, tab2params->aptime2_2/1000.0,  tab2params->frequency1_2, tab2params->frequency2_2 );

        if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
        {
            vector<noise_param>noises;
            GetNoiseVector(&noises);
            AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl2*ampl2*tab2params->duration2_2/2000.0, noises);
            noises.clear();
        }

        if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
        {
            vector<interf_param>interf;
            GetInterfVector(&interf);
            AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl2*ampl2*tab2params->duration2_2/2000.0, interf);
            interf.clear();
        }

        if (ui->CompenseBox->isChecked() == 1)
        {
           attenuator(signal, buf_size/2, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
        }

        WriteSamplesMono(samples, signal, buf_size/2);


        double az=0;
        double di=0;
        if (pannelparams->left == 0)
        {
            di = 1;
            az = +90;
        }
        if (pannelparams->right == 0)
        {
            di = 1;
            az = -90;
        }

        Play_buffer(samples, buf_size, az, di, 0, AL_FORMAT_MONO16, pannelparams->loudmultipler);

        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;

    }
    delete [] samples;
    delete [] signal;

    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");

    stopflag = 1;
}

void MainWindow::on_TimeSeqButton_2_clicked()
{
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    stopflag3 = 1;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi2(tab2params);
    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    ALenum format;
    short *samples;
    ALsizei buf_size;

    format = AL_FORMAT_MONO16;
    buf_size = ceil(seconds * sample_rate*2);
    samples = new short[buf_size];
    double *signal = new double[buf_size/2];
    double ampl1 = 1/pow(10.0,(100.0 - tab2params->loudness1_2)/20.0);
    double ampl2 = 1/pow(10.0,(100.0 - tab2params->loudness2_2)/20.0);

    double end1 = tab2params->aptime1_2+tab2params->duration1_2;
    for(int k = 30; k>=0; k = k - 1)
    {
        tab2params->aptime2_2 = end1+k;
        ui->Time2Box_2->setValue(tab2params->aptime2_2);

        CreateSignal2Mono(signal,  buf_size/2, tab2params->duration1_2/1000.0, tab2params->duration2_2/1000.0, ampl1, ampl2,  tab2params->aptime1_2/1000.0, tab2params->aptime2_2/1000.0,  tab2params->frequency1_2, tab2params->frequency2_2 );

        if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
        {
            vector<noise_param>noises;
            GetNoiseVector(&noises);
            AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl2*ampl2*tab2params->duration2_2/2000.0, noises);
            noises.clear();
        }

        if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
        {
            vector<interf_param>interf;
            GetInterfVector(&interf);
            AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl2*ampl2*tab2params->duration2_2*1000/2.0, interf);
            interf.clear();
        }

        if (ui->CompenseBox->isChecked() == 1)
        {
           attenuator(signal, buf_size/2, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
        }

        WriteSamplesMono(samples, signal, buf_size/2);


        double az=0;
        double di=0;
        if (pannelparams->left == 0)
        {
            di = 1;
            az = +90;
        }
        if (pannelparams->right == 0)
        {
            di = 1;
            az = -90;
        }

        Play_buffer(samples, buf_size, az, di, 0, AL_FORMAT_MONO16, pannelparams->loudmultipler);

        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;

    }
    delete [] samples;
    delete [] signal;

    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");

    stopflag = 1;
}

void MainWindow::on_LoudSeqButton_2_clicked()
{
    stopflag3 = 1;
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi2(tab2params);
    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    ALenum format;
    short *samples;
    ALsizei buf_size;

    format = AL_FORMAT_MONO16;
    buf_size = ceil(seconds * sample_rate*2);
    samples = new short[buf_size];
    double *signal = new double[buf_size/2];
    double ampl1 = 1/pow(10.0,(100.0 - tab2params->loudness1_2)/20.0);

    for(int k = 50; k>=0; k = k - 1)
    {
        ui->Loudness2Box_2->setValue(k);
        tab2params->loudness2_2 = k;
        double ampl2 = 1/pow(10.0,(100.0 - tab2params->loudness2_2)/20.0);
        CreateSignal2Mono(signal,  buf_size/2, tab2params->duration1_2/1000.0, tab2params->duration2_2/1000.0, ampl1, ampl2,  tab2params->aptime1_2/1000.0, tab2params->aptime2_2/1000.0,  tab2params->frequency1_2, tab2params->frequency2_2 );

        if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
        {
            vector<noise_param>noises;
            GetNoiseVector(&noises);
            AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl2*ampl2*tab2params->duration2_2/2000.0, noises);
            noises.clear();
        }

        if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
        {
            vector<interf_param>interf;
            GetInterfVector(&interf);
            AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl2*ampl2*tab2params->duration2_2*1000/2.0, interf);
            interf.clear();
        }

        if (ui->CompenseBox->isChecked() == 1) //Если установлен флаг "Компенсация".
        {
           attenuator(signal, buf_size/2, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
        }

        WriteSamplesMono(samples, signal, buf_size/2);

        double az=0;
        double di=0;
        if (pannelparams->left == 0)
        {
            di = 1;
            az = +90;
        }
        if (pannelparams->right == 0)
        {
            di = 1;
            az = -90;
        }

        Play_buffer(samples, buf_size, az, di, 0, AL_FORMAT_MONO16, pannelparams->loudmultipler);

        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;

    }
    delete [] samples;
    delete [] signal;

    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");

    stopflag = 1;
}

// ---------------------------------------------------------------------------------------------------------------------------- Third tab

void MainWindow::on_DeviationSlider_3_sliderMoved(int position)
{
    ui->DeviationBox_3->setValue(position/10.0);
}

void MainWindow::on_DeviationBox_3_valueChanged(double arg1)
{
    ui->DeviationSlider_3->setValue(int(arg1*10));
}

void MainWindow::on_DepthSlider_3_sliderMoved(int position)
{
    ui->DepthBox_3->setValue(position/100.0);
}

void MainWindow::on_DepthBox_3_valueChanged(double arg1)
{
    ui->DepthSlider_3->setValue(int(arg1*100));
}

void MainWindow::on_HearButton_3_clicked()//Если слышим звук, сохраняем результат.
{
    QString out = QString::number(tab3params->frequency_3);
    out.append("\t");
    out.append(QString::number(tab3params->modulationfreq_3));
    out.append("\t");
    out.append(QString::number(tab3params->loudness_3));
    out.append("\t");
    if(tab3params->FM == 1) out.append(QString::number(tab3params->freqdeviation_3));
    else out.append(QString::number(tab3params->moddepth_3));
    ui->ResultBrowser_3->append(out);
    if (stopflag2 == 0) return;
    stopflag2 = 0;
    stopflag3 = 0;//Остановить все циклы
    StopThreads();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stopflag2 = 1;//Остановить воспроизведение
}

void MainWindow::on_AMSeqButton_clicked()
{
    tab3_sequences("AM");
}

void MainWindow::on_PMSeqButton_clicked()
{
    tab3_sequences("PM");
}

void MainWindow::on_FMSeqButton_3_clicked()
{
    tab3_sequences("FM");
}

void MainWindow::tab3_sequences(QString type)
{
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    stopflag3 = 1;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();

    RefreshFromUi3(tab3params);
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    ALsizei buf_size;
    short *samples;


    buf_size = ceil(seconds * sample_rate*2);
    double *signal = new double[buf_size/2];
    samples = new short[buf_size];
    double ampl = 1/pow(10.0,(100.0 - tab3params->loudness_3)/20.0);

    for (int k = 25; k>=0; k--)
        {
            if (type == "AM")
            {
                tab3params->moddepth_3 = k/100.0;
                ui->DepthBox_3->setValue(tab3params->moddepth_3);
                CreateSignal3MonoAM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->moddepth_3, pannelparams->smooth, ampl);
            }
            else if (type == "PM")
            {
                tab3params->moddepth_3 = k/100.0;
                ui->DepthBox_3->setValue(tab3params->moddepth_3);
                CreateSignal3MonoPM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->moddepth_3, pannelparams->smooth,  ampl);
            }
            else if (type == "FM")
            {
                tab3params->freqdeviation_3 = 4*k;
                ui->DeviationBox_3->setValue(tab3params->freqdeviation_3);
                CreateSignal3MonoFM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->freqdeviation_3, pannelparams->smooth, ampl);
            }
            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);
                AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);
                AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
                interf.clear();
            }

            if (ui->CompenseBox->isChecked() == 1)
            {
               attenuator(signal, buf_size/2, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
            }

            WriteSamplesMono(samples, signal, buf_size/2);

            double az=0;
            double di=0;
            if (pannelparams->left == 0)
            {
                di = 1;
                az = +90;
            }
            if (pannelparams->right == 0)
            {
                di = 1;
                az = -90;
            }

            Play_buffer(samples, buf_size, az, di, 0, AL_FORMAT_MONO16, pannelparams->loudmultipler);
            QCoreApplication::processEvents();
            if (stopflag3 == 0) break;
        }
    if(err.size()>1)
    {
        QString temp;
        temp.append("Возникли ошибки: ");
        temp.append(err);
        this->status->setText(temp);
    }
    else this->status->setText("Готов.");
    delete [] samples;
    delete [] signal;
    stopflag = 1;
}

// ---------------------------------------------------------------------------------------------------------------------------- Play all


void MainWindow::on_PlayButton_clicked()//Если нажата кнопка воспроизведения.
{
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();
    switch (ui->tabWidget->currentIndex())
    {
        case 0: {
            RefreshFromUi1(tab1params);
            RefreshPannel(pannelparams);//Загружаем параметры интерфейса
            double seconds = pannelparams->signalduration;

            ALsizei buf_size = ceil(seconds * sample_rate*4);
            short *samples = new short[buf_size];
            double *signal_left = new double[buf_size/2];
            double *signal_right = new double[buf_size/2];

            double ampl = 1/pow(10.0,(100.0 - tab1params->loudness)/20.0);
            CreateSignal1Stereo(signal_left, signal_right, buf_size/4, tab1params->frequency, tab1params->azimuth, tab1params->rotfreq, tab1params->rotation, pannelparams->left, pannelparams->right, pannelparams->smooth, ampl);

            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);
                AddCnoise(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
                AddCnoise(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises); //Добавляем шум.
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);
                AddInterf(signal_left,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
                AddInterf(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);//Добавляем помехи.
                interf.clear();
            }

            if (ui->CompenseBox->isChecked() == 1)
            {
                if (ui->AddInterfBox->isChecked() == 1 || ui->AddNoiseBox->isChecked() == 1)
                {
                   attenuator(signal_left, buf_size/4, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
                   attenuator(signal_right, buf_size/4, 1.0*sample_rate, &freqs, &values);
                   WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2);
                   Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
                }
                else
                {
                    WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Делаем упрощённую компенсацию (учёт множителя затухания)
                    Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler*GetfadeValue(tab1params->frequency, &freqs, &values));
                }
            }
            else
            {
                WriteSamplesStereo(samples, signal_left, signal_right, buf_size/2); //Не делаем компенсацию
                Play_buffer(samples, buf_size, 0, 0, ui->RotationBox->isChecked(), AL_FORMAT_STEREO16, pannelparams->loudmultipler);
            }

            delete [] signal_left;
            delete [] signal_right;
            delete [] samples;

            if(err.size()>1)
            {
                QString temp;
                temp.append("Возникли ошибки: ");
                temp.append(err);
                this->status->setText(temp);
            }
            else this->status->setText("Готов.");

            break;
        }
        case 1:{

            RefreshFromUi2(tab2params);
            RefreshFromUi1(tab1params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;
            ALenum format;
            short *samples;
            ALsizei buf_size;

            format = AL_FORMAT_MONO16;
            buf_size = ceil(seconds * sample_rate*2);
            samples = new short[buf_size];
            double *signal = new double[buf_size/2];
            double ampl1 = 1/pow(10.0,(100.0 - tab2params->loudness1_2)/20.0);
            double ampl2 = 1/pow(10.0,(100.0 - tab2params->loudness2_2)/20.0);
            CreateSignal2Mono(signal,  buf_size/2, tab2params->duration1_2/1000.0, tab2params->duration2_2/1000.0, ampl1, ampl2,  tab2params->aptime1_2/1000.0, tab2params->aptime2_2/1000.0,  tab2params->frequency1_2, tab2params->frequency2_2 );

            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);
                AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl2*ampl2*tab2params->duration2_2/2000.0, noises);
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);
                AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl2*ampl2*tab2params->duration2_2/2000.0, interf);
                interf.clear();
            }

            if (ui->CompenseBox->isChecked() == 1)
            {
               attenuator(signal, buf_size/2, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
            }

            WriteSamplesMono(samples, signal, buf_size/2);
            delete [] signal;

            double az=0;
            double di=0;
            if (pannelparams->left == 0)
            {
                di = 1;
                az = +90;
            }
            if (pannelparams->right == 0)
            {
                di = 1;
                az = -90;
            }

            Play_buffer(samples, buf_size, az, di, 0, AL_FORMAT_MONO16, pannelparams->loudmultipler);

            if(err.size()>1)
            {
                QString temp;
                temp.append("Возникли ошибки: ");
                temp.append(err);
                this->status->setText(temp);
            }
            else this->status->setText("Готов.");
            delete [] samples;

            break;

        }

            case 2:{
            RefreshFromUi3(tab3params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;
            ALsizei buf_size;
            short *samples;


            buf_size = ceil(seconds * sample_rate*2);
            double *signal = new double[buf_size/2];
            samples = new short[buf_size];
            double ampl = 1/pow(10.0,(100.0 - tab3params->loudness_3)/20.0);
            if (ui->AMBox->isChecked() == 1) CreateSignal3MonoAM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->moddepth_3, pannelparams->smooth, ampl);
            else if (ui->PMBox->isChecked() == 1) CreateSignal3MonoPM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->moddepth_3, pannelparams->smooth, ampl);
            else if (ui->FMBox->isChecked() == 1) CreateSignal3MonoFM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->freqdeviation_3, pannelparams->smooth, ampl);

            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);
                AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);
                AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
                interf.clear();
            }

            if (ui->CompenseBox->isChecked() == 1)
            {
               attenuator(signal, buf_size/2, 1.0*sample_rate, &freqs, &values); //Делаем частотную компенсацию сигнала
            }

            WriteSamplesMono(samples, signal, buf_size/2);
            delete [] signal;

            double az=0;
            double di=0;
            if (pannelparams->left == 0)
            {
                di = 1;
                az = +90;
            }
            if (pannelparams->right == 0)
            {
                di = 1;
                az = -90;
            }

            Play_buffer(samples, buf_size, az, di, 0, AL_FORMAT_MONO16, pannelparams->loudmultipler);

            if(err.size()>1)
            {
                QString temp;
                temp.append("Возникли ошибки: ");
                temp.append(err);
                this->status->setText(temp);
            }
            else this->status->setText("Готов.");
            delete [] samples;
            break;
                }
    }
    stopflag = 1;
}

//===============================================================================================================================================================================================//Spectr


void MainWindow::on_SpectrumButton_clicked()
{
    dialog_finished = 0;
    fftwindow->show();
    switch (ui->tabWidget->currentIndex())
    {
        case 0: {
            RefreshFromUi1(tab1params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;

            ALsizei buf_size = ceil(seconds * sample_rate*4);
            double *signal_imag = new double[buf_size/4];
            double *signal_right = new double[buf_size/4];
            double *signal_right_cp = new double[buf_size/4];
            double ampl = 1/pow(10.0,(100.0 - tab1params->loudness)/20.0);
            CreateSignal1Stereo(signal_imag, signal_right, buf_size/4, tab1params->frequency, tab1params->azimuth, tab1params->rotfreq, tab1params->rotation, 0, pannelparams->right, pannelparams->smooth, ampl);

            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);//Добавление шума.
                AddCnoise(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);//Добавление помех.
                AddInterf(signal_right,  buf_size/4, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
                interf.clear();
            }

            for (ALsizei k = 0; k < buf_size/4; k++) signal_right_cp[k] = signal_right[k];
            Fft_transform(signal_right_cp, signal_imag, buf_size/4);//Получение спектра.
            for (ALsizei k = 0; k < buf_size/4; k++) signal_right_cp[k] = pow(signal_right_cp[k]*signal_right_cp[k] + signal_imag[k]*signal_imag[k], 0.5)*(8/(1.0*buf_size));
            signal_right_cp[0]/=2;

            fftwindow->SetData(signal_right, signal_right_cp, 1.0*sample_rate, buf_size/4); //Для отображения спектра и сигнала используется только правый канал.

            while (dialog_finished == 0)
            {
                QCoreApplication::processEvents();
            }

            delete [] signal_imag;
            delete [] signal_right;
            delete [] signal_right_cp;
            break;
        }
        case 1:{

            RefreshFromUi2(tab2params);
            RefreshFromUi1(tab1params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;
            ALenum format;
            ALsizei buf_size;
            format = AL_FORMAT_MONO16;
            buf_size = ceil(seconds * sample_rate*2);
            double *signal = new double[buf_size/2];
            double *spectr = new double[buf_size/2];
            double *signal_imag = new double[buf_size/2];
            double ampl1 = 1/pow(10.0,(100.0 - tab2params->loudness1_2)/20.0);
            double ampl2 = 1/pow(10.0,(100.0 - tab2params->loudness2_2)/20.0);
            CreateSignal2Mono(signal,  buf_size/2, tab2params->duration1_2/1000.0, tab2params->duration2_2/1000.0, ampl1, ampl2,  tab2params->aptime1_2/1000.0, tab2params->aptime2_2/1000.0,  tab2params->frequency1_2, tab2params->frequency2_2 );

            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);
                AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl2*ampl2*tab2params->duration2_2/2000.0, noises);
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);
                AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl2*ampl2*tab2params->duration2_2/2000.0, interf);
                interf.clear();
            }

            for (ALsizei k = 0; k < buf_size/2; k++) spectr[k] = signal[k];
            for (ALsizei k = 0; k < buf_size/2; k++) signal_imag[k] = 0;
            Fft_transform(spectr, signal_imag, buf_size/2);//Получение спектра.
            for (ALsizei k = 0; k < buf_size/2; k++) spectr[k] = pow(spectr[k]*spectr[k] + signal_imag[k]*signal_imag[k], 0.5)*(4/(1.0*buf_size));
            spectr[0]/=2;

            fftwindow->SetData(signal, spectr, 1.0*sample_rate, buf_size/2);
            while (dialog_finished == 0)
            {
                QCoreApplication::processEvents();
            }
            delete [] signal;
            delete [] spectr;
            delete [] signal_imag;
            break;
        }
        case 2:{
            RefreshFromUi3(tab3params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;
            ALsizei buf_size;

            buf_size = ceil(seconds * sample_rate*2);
            double *signal = new double[buf_size/2];
            double *spectr = new double[buf_size/2];
            double *signal_imag = new double[buf_size/2];
            double ampl = 1/pow(10.0,(100.0 - tab3params->loudness_3)/20.0);
            if (ui->AMBox->isChecked() == 1) CreateSignal3MonoAM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->moddepth_3, pannelparams->smooth, ampl);
            else if (ui->PMBox->isChecked() == 1) CreateSignal3MonoPM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->moddepth_3, pannelparams->smooth,  ampl);
            else if (ui->FMBox->isChecked() == 1) CreateSignal3MonoFM(signal,  buf_size/2, tab3params->frequency_3, tab3params->modulationfreq_3, tab3params->freqdeviation_3, pannelparams->smooth, ampl);

            if(pannelparams->noiseampls.size() > 0 && ui->AddNoiseBox->isChecked() == true)
            {
                vector<noise_param>noises;
                GetNoiseVector(&noises);
                AddCnoise(signal,  buf_size/2, 1.0*sample_rate, pannelparams->snr, ampl*ampl*pannelparams->signalduration/2.0, noises);
                noises.clear();
            }

            if(pannelparams->interfampls.size() > 0 && ui->AddInterfBox->isChecked() == true)
            {
                vector<interf_param>interf;
                GetInterfVector(&interf);
                AddInterf(signal,  buf_size/2, 1.0*sample_rate, pannelparams->sinterf, ampl*ampl*pannelparams->signalduration/2.0, interf);
                interf.clear();
            }

            for (ALsizei k = 0; k < buf_size/2; k++) spectr[k] = signal[k];
            for (ALsizei k = 0; k < buf_size/2; k++) signal_imag[k] = 0;
            Fft_transform(spectr, signal_imag, buf_size/2);//Получение спектра.
            for (ALsizei k = 0; k < buf_size/2; k++) spectr[k] = pow(spectr[k]*spectr[k] + signal_imag[k]*signal_imag[k], 0.5)*(4/(1.0*buf_size));
            spectr[0]/=2;

            fftwindow->SetData(signal, spectr, 1.0*sample_rate, buf_size/2);
            while (dialog_finished == 0)
            {
                QCoreApplication::processEvents();
            }

            delete [] signal;
            delete [] spectr;
            delete [] signal_imag;
            break;
        }

    }
    this->lastPath = this->settings->value("lastPath","C://").toString();
}

//=====================================================================================================================================================================================================//


void MainWindow::GetNoiseVector(vector<noise_param> *noises)// Загрузка значений шума из интерфейса.
{
    int n = pannelparams->noiseampls.size();
    if(n<pannelparams->noisefreqs.size()) n = pannelparams->noisefreqs.size();
    if(n<pannelparams->noisebands.size()) n = pannelparams->noisebands.size();

    for(int k = 0; k<n; k++)
    {
        noise_param temp;
        temp.relamp = pannelparams->noiseampls[k];
        temp.medium = pannelparams->noisefreqs[k];
        temp.bandwidth = pannelparams->noisebands[k];
        noises->push_back(temp);
    }
}

void MainWindow::GetInterfVector(vector<interf_param> *interf)// Загрузка значений помех из интерфейса.
{
    int n = pannelparams->interfampls.size();
    if(n<pannelparams->interffreqs.size()) n = pannelparams->interffreqs.size();
    if(n<pannelparams->interfphases.size()) n = pannelparams->interfphases.size();
    for(int k = 0; k<n; k++)
    {
        interf_param temp;
        temp.relamp = pannelparams->interfampls[k];
        temp.frequency = pannelparams->interffreqs[k];
        temp.phase = pannelparams->interfphases[k];
        interf->push_back(temp);
    }
}



void SaveFile(QString *namestr, QStringList *data)
{
    QFile file;
    file.setFileName(*namestr);
    if (!file.open(QIODevice::WriteOnly))return;
    QTextStream out(&file);
    for(int k = 0; k<data->length(); k++) out<<data->at(k)<<"\r\n";
    file.close();
}

void CutPath(QString *path)
{
    int i = path->length();
    while (i > 0)
    {
        if (path->at(i-1) == "\\" || path->at(i-1) == "/") break;
        i--;
    }
    *path = path->left(i);
}

void MainWindow::on_SaveResultButton_clicked()
{
    QString namestr = QFileDialog::getSaveFileName(this, ("Сохранить результат эксперимента в файл"), lastPath , "Text files (*.txt)");
    QStringList temp = ui->ResultBrowser->toPlainText().split("\n");
    if (namestr.length() == 0) return;
    SaveFile(&namestr,&temp);
    CutPath(&namestr);
    lastPath = namestr;
}

void MainWindow::on_SaveResultButton_2_clicked()
{
    QString namestr = QFileDialog::getSaveFileName(this, ("Сохранить результат эксперимента в файл"), lastPath , "Text files (*.txt)");
    QStringList temp = ui->ResultBrowser_2->toPlainText().split("\n");
    if (namestr.length() == 0) return;
    SaveFile(&namestr,&temp);
    CutPath(&namestr);
    lastPath = namestr;
}

void MainWindow::on_SaveResultButton_3_clicked()
{
    QString namestr = QFileDialog::getSaveFileName(this, ("Сохранить результат эксперимента в файл"), lastPath , "Text files (*.txt)");
    QStringList temp = ui->ResultBrowser_3->toPlainText().split("\n");
    if (namestr.length() == 0) return;
    SaveFile(&namestr,&temp);
    CutPath(&namestr);
    lastPath = namestr;
}

void MainWindow::on_DeleteStringButton_clicked()
{
   QStringList temp = ui->ResultBrowser->toPlainText().split("\n");
   if(temp.size() > 1)temp.removeLast();
   ui->ResultBrowser->setText(temp.join("\n"));
}
void MainWindow::on_DeleteStringButton_2_clicked()
{
    QStringList temp = ui->ResultBrowser_2->toPlainText().split("\n");
    if(temp.size() > 1)temp.removeLast();
    ui->ResultBrowser_2->setText(temp.join("\n"));
}

void MainWindow::on_DeleteStringButton_3_clicked()
{
    QStringList temp = ui->ResultBrowser_3->toPlainText().split("\n");
    if(temp.size() > 1)temp.removeLast();
    ui->ResultBrowser_3->setText(temp.join("\n"));
}