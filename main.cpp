#include <QCoreApplication>
#include "internetchecker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    internetChecker* netChecker = new internetChecker();
    netChecker->check();
    //return a.exec();
}
