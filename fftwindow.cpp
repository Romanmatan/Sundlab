#include "fftwindow.h"
#include "ui_fftwindow.h"

FFTWindow::FFTWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FFTWindow)
{
    ui->setupUi(this);
}

FFTWindow::~FFTWindow()
{
    delete ui;
}
