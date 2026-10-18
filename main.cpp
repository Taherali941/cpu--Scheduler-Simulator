#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QApplication::setApplicationName("CPU Scheduling Simulator");
    QApplication::setOrganizationName("OS Mini Project");
    MainWindow window;
    window.show();
    return app.exec();
}
