#include "mainwindow.h"

#include <QApplication>
#include <QFile>

static void LoadStyleSheet(QApplication &app)
{
    QFile file(":/style.qss");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        app.setStyleSheet(QString::fromUtf8(file.readAll()));
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("labSoft");
    a.setOrganizationDomain("labsoft.com");
    a.setApplicationName("soundlab");
    LoadStyleSheet(a);
    MainWindow w;
    w.show();
    return a.exec();
}
