#include "ui/guimanager.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    guimanager w;
    w.show();
    return a.exec();
}
