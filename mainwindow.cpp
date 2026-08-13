#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "fftwindow.h"
#include <combaseapi.h>
#include <QToolBar>
#include <QAction>
#include <QMessageBox>

//int freq = 0;
//bool state = 0;

void MainWindow::LoadSettings()
{
    ui->LoudnessMultEdit->setText(this->settings->value("LoudnessMultEdit","1").toString());
}

void MainWindow::SaveSettings()
{

    this->settings->setValue("LoudnessMultEdit", ui->LoudnessMultEdit->text());
    this->settings->sync();
}

void MainWindow::RefreshPannel(pannelp *pn)
{
    pn->signalduration = ui->DurationEdit->text().toDouble();
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
     ui->OnlyRightBox->isChecked();
    pn->loudmultipler = ui->LoudnessMultEdit->text().toDouble();
    pn->azimuth = ui->AzimuthEdit->text().toDouble();
    pn->distance = ui->DistanceEdit->text().toDouble();
    pn->rotation = ui->RotationBox->isChecked();
    pn->addnoise = ui->AddNoiseBox->isChecked();
    pn->addinterf = ui->AddInterfBox->isChecked();
    pn->snr = ui->SNREdit->text().toDouble();
    pn->sinterf = ui->SInterfEdit->text().toDouble();

    QStringList temp = ui->NoisesFreqEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        pn->noisefreqs.append(temp[i].toDouble());
    }

    temp = ui->NoisesBandEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        pn->noisebands.append(temp[i].toDouble());
    }

    temp = ui->NoisesRelampEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        pn->noiseampls.append(temp[i].toDouble());
    }

    temp = ui->InterfFreqEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        pn->interffreqs.append(temp[i].toDouble());
    }

    temp = ui->InterfLevelEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        pn->interflevels.append(temp[i].toDouble());
    }

    temp = ui->InterfRelampEdit->text().split(";");
    for (int i = 0; i < temp.size(); i++)
    {
        pn->interfampls.append(temp[i].toDouble());
    }
    temp.clear();
}

void MainWindow::RefreshFromUi1(tab1p *p1)
{
    p1->frequency = ui->FrequencyBox->value();
    p1->loudness = ui->LoudnessBox->value();
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
    p2->duration1_2 = ui->Duration2Box_2->value();
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
    this->status = new QLabel(this);
    ui->statusbar->addWidget(status);
    this->status->setText("Готов.");
    QWidget::setWindowTitle("SoundLab");
    fftwindow = new FFTWindow(0);
    fftwindow->setWindowFlags((windowFlags() & ~Qt::WindowContextHelpButtonHint)& ~Qt::WindowMinimizeButtonHint);

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
        ui->DistanceEdit->setText("1000");
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
    ui->DistanceEdit->setToolTip("Расстояние до источника звука, м");

    tab1params = new tab1p;
    tab2params = new tab2p;
    tab3params = new tab3p;
    pannelparams = new pannelp;
    settings = new QSettings;
    this->LoadSettings();
    ui->tabWidget->setCurrentIndex(0);
    stopflag = 1;
    stopflag2 = 1;
    stopflag3 = 1;
    switch1 = 0;
    //err = "";
    //plth = new PlayThread;
    //thread = new QThread;


    //CoInitialize( 0 );
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

    pannelparams->noisefreqs.clear();
    pannelparams->noisebands.clear();
    pannelparams->noiseampls.clear();
    pannelparams->interffreqs.clear();
    pannelparams->interflevels.clear();
    pannelparams->interfampls.clear();

    //delete(plth);
    //delete(thread);
    delete (pannelparams);
    delete (settings);
    delete ui;
}

void MainWindow::Play_buffer(short *samples, ALsizei buf_size, double loudnessmult, double loudness, double azimuth, double distance, ALenum format)
{
    QThread *thread = new QThread;
    PlayThread *plth = new PlayThread;
    plth->SetData(samples, buf_size, sample_rate, format);
    if (format == AL_FORMAT_MONO16)
    {
    double Xpos = distance*sin(azimuth);
    double Ypos = distance*cos(azimuth);
    plth->SetPosition(Xpos, Ypos, 0);
    }
    ALfloat volume = ALfloat(loudnessmult/pow(10.0,(100.0 - loudness)/20.0));    
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


void MainWindow::StopThreads()  /* принудительная остановка всех потоков */
{
    emit  stopAll();
}

void MainWindow::on_SpectrumButton_clicked()
{
    fftwindow->show();
}

void MainWindow::on_StopButton_clicked()
{
    if (stopflag2 == 0) return;
    stopflag2 = 0;
    stopflag3 = 0;
    StopThreads();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stopflag2 = 1;
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
  }
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


void CreateSignal1(short *samples, ALsizei buf_size, double frequency, bool left, bool right, ALenum format)
{
    ALsizei i=0;
    short mult = 0;
    if (format == AL_FORMAT_STEREO16)
    {
        for(; i<buf_size; ++i)
        {
            samples[2*i] = 32767 * sin( (2.f * M_PI * frequency)/sample_rate * i )*left;
            samples[2*i+1] = 32767 * sin( (2.f * M_PI * frequency)/sample_rate * i )*right;
            //frequency += 0.001;
        }
        mult = 2;
    }
    else if (format == AL_FORMAT_MONO16)
    {
        for(; i<buf_size; ++i)
        {
            samples[i] = 32767 * sin( (2.f * M_PI * frequency)/sample_rate * i );
        }
        mult = 1;
    }

    for(i=0; i<ALsizei(sample_rate/frequency*20)*mult; i++)
    {
         samples[i] *= i/(1.0*sample_rate/frequency*20)/mult;
    }

    for(i=buf_size-sample_rate/frequency*20*mult; i<buf_size; i++)
    {
         samples[i] *= (buf_size-i)/(1.0*sample_rate/frequency*20)/mult;
    }

}



void MainWindow::on_FreqSeqButton_low_clicked()
{
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    stopflag3 = 1;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();
    RefreshFromUi1(tab1params);
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    short formmult;
    ALenum format;
    if (pannelparams->left != pannelparams->right)
    {
        format = AL_FORMAT_STEREO16;
        formmult = 2;
    }
    else
    {
        format = AL_FORMAT_MONO16;
        formmult = 1;
    }
    ALsizei buf_size = ceil(seconds * sample_rate*formmult);
    short *samples = (short*)malloc(sizeof(short) * buf_size);
    for (int k = 15; k < 25; k++)
    {        
        ui->FrequencyBox->setValue(k);
        CreateSignal1(samples, buf_size, k, pannelparams->left, pannelparams->right, format);
        Play_buffer(samples, buf_size, pannelparams->loudmultipler, tab1params->loudness, pannelparams->azimuth, pannelparams->distance, format);
        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;
    }
    free(samples);

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
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    short formmult;
    ALenum format;
    if (pannelparams->left != pannelparams->right)
    {
        format = AL_FORMAT_STEREO16;
        formmult = 2;
    }
    else
    {
        format = AL_FORMAT_MONO16;
        formmult = 1;
    }
    ALsizei buf_size = ceil(seconds * sample_rate*formmult);
    short *samples = (short*)malloc(sizeof(short) * buf_size);
    for (int k = 14000; k < 21000; k+=500)
    {
        ui->FrequencyBox->setValue(k);
        CreateSignal1(samples, buf_size, k, pannelparams->left, pannelparams->right, format);
        Play_buffer(samples, buf_size, pannelparams->loudmultipler, tab1params->loudness, pannelparams->azimuth, pannelparams->distance, format);
        //QThread::sleep(1);
        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;
    }
    free(samples);

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
    RefreshPannel(pannelparams);
    double seconds = pannelparams->signalduration;
    short formmult;
    ALenum format;
    if (pannelparams->left != pannelparams->right)
    {
        format = AL_FORMAT_STEREO16;
        formmult = 2;
    }
    else
    {
        format = AL_FORMAT_MONO16;
        formmult = 1;
    }
    ALsizei buf_size = ceil(seconds * sample_rate*formmult);
    short *samples = (short*)malloc(sizeof(short) * buf_size);
    for (int k = 20; k < 71; k ++)
    {
        tab1params->loudness = k;
        ui->LoudnessBox->setValue(tab1params->loudness);
        CreateSignal1(samples, buf_size, tab1params->frequency, pannelparams->left, pannelparams->right, format);
        Play_buffer(samples, buf_size, pannelparams->loudmultipler, tab1params->loudness, pannelparams->azimuth, pannelparams->distance, format);
        //QThread::sleep(1);
        QCoreApplication::processEvents();
        if (stopflag3 == 0) break;
    }
    free(samples);

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
    out.append(" Звук уже слышен\n");
    ui->ResultBrowser->append(out);
}

void MainWindow::on_NotHearButton_clicked()
{
    QString out = QString::number(tab1params->frequency);
    out.append("\t");
    out.append(QString::number(tab1params->loudness));
    out.append("\t");
    out.append(" Звук уже не слышен\n");
    ui->ResultBrowser->append(out);
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

void MainWindow::on_DeleteStringButton_clicked()
{
    ui->ResultBrowser->clear();
}


void CreateSignal2(short *samples, ALsizei buf_size, double frequency, bool left, bool right, ALenum format)
{
    ALsizei i=0;
    short mult = 0;
    if (format == AL_FORMAT_STEREO16)
    {
        for(; i<buf_size; ++i)
        {
            samples[2*i] = 32767 * sin( (2.f * M_PI * frequency)/sample_rate * i )*left;
            samples[2*i+1] = 32767 * sin( (2.f * M_PI * frequency)/sample_rate * i )*right;
            //frequency += 0.001;
        }
        mult = 2;
    }
    else if (format == AL_FORMAT_MONO16)
    {
        for(; i<buf_size; ++i)
        {
            samples[i] = 32767 * sin( (2.f * M_PI * frequency)/sample_rate * i );
        }
        mult = 1;
    }

    for(i=0; i<ALsizei(sample_rate/frequency*20)*mult; i++)
    {
         samples[i] *= i/(1.0*sample_rate/frequency*20)/mult;
    }

    for(i=buf_size-sample_rate/frequency*20*mult; i<buf_size; i++)
    {
         samples[i] *= (buf_size-i)/(1.0*sample_rate/frequency*20)/mult;
    }

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


// ---------------------------------------------------------------------------------------------------------------------------- Play all


void MainWindow::on_PlayButton_clicked()
{
    if (stopflag == 0 || stopflag2 == 0) return;
    stopflag = 0;
    this->status->setText("Воспроизведение.");
    QCoreApplication::processEvents();
    switch (ui->tabWidget->currentIndex())
    {
        case 0: {
            RefreshFromUi1(tab1params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;
            short formmult;
            ALenum format;
            if (pannelparams->left != pannelparams->right)
            {
                format = AL_FORMAT_STEREO16;
                formmult = 2;
            }
            else
            {
                format = AL_FORMAT_MONO16;
                formmult = 1;
            }
            ALsizei buf_size = ceil(seconds * sample_rate*2*formmult);
            short *samples = (short*)malloc(sizeof(short) * buf_size);
            CreateSignal1(samples, buf_size/2, tab1params->frequency, pannelparams->left, pannelparams->right, format);
            Play_buffer(samples, buf_size, pannelparams->loudmultipler, tab1params->loudness, pannelparams->azimuth, pannelparams->distance, format);

            if(err.size()>1)
            {
                QString temp;
                temp.append("Возникли ошибки: ");
                temp.append(err);
                this->status->setText(temp);
            }
            else this->status->setText("Готов.");
            free(samples);
            break;
        }
        case 1:{
/*
            RefreshFromUi2(tab2params);
            RefreshPannel(pannelparams);
            double seconds = pannelparams->signalduration;
            short formmult;
            ALenum format;
            if (pannelparams->left != pannelparams->right)
            {
                format = AL_FORMAT_STEREO16;
                formmult = 2;
            }
            else
            {
                format = AL_FORMAT_MONO16;
                formmult = 1;
            }
            ALsizei buf_size = ceil(seconds * sample_rate*2*formmult);
            short *samples = (short*)malloc(sizeof(short) * buf_size);

            CreateSignal2(samples, buf_size/2, tab2params->frequency1_2, pannelparams->left, pannelparams->right, format);
            Play_buffer(samples, buf_size, pannelparams->loudmultipler, tab2params->loudness, pannelparams->azimuth, pannelparams->distance, format);

            if(err.size()>1)
            {
                QString temp;
                temp.append("Возникли ошибки: ");
                temp.append(err);
                this->status->setText(temp);
            }
            else this->status->setText("Готов.");
            free(samples);
*/
            break;
        }
    }
    stopflag = 1;
}
