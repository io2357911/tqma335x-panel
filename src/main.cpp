#include <QApplication>
#include "mainwindow.h"

int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainWindow window;
#ifdef __arm__
    window.showFullScreen();
#else
    window.show();
#endif

    return app.exec();
}

