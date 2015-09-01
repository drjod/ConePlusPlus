#include <QApplication>
#include "wizard.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    wizard gui;
    gui.show();

    return app.exec();
}



