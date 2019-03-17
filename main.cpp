#include "pov_display.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    POV_Display w;
    w.show();

    return a.exec();
}
