#include "neobes.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    neobes w;
    w.show();
    return a.exec();
}
