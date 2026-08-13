#ifndef FFTWINDOW_H
#define FFTWINDOW_H

#include <QDialog>

namespace Ui {
class FFTWindow;
}

class FFTWindow : public QDialog
{
    Q_OBJECT

public:
    explicit FFTWindow(QWidget *parent = nullptr);
    ~FFTWindow();

private:
    Ui::FFTWindow *ui;
};

#endif // FFTWINDOW_H
