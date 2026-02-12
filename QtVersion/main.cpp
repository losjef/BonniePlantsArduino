#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    // Full screen - no task bar
    //w.setWindowState(Qt::WindowFullScreen);

    //Maximized - includes task bar
    w.setWindowState(Qt::WindowMaximized);
    w.show();
    return a.exec();
}
