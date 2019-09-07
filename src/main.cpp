#include <QApplication>
#include "main_window.h"
#include "core/read_charmap.h"

int main(int argc, char *argv[])
{
    const QString fileName{"charmapjp.txt"};
    CharMapReader *reader = new CharMapReader{&fileName};
    if (!reader->State())
    {
        qDebug() << "Can't load charmapjp.txt!";
        return 1;
    }
    QApplication a(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuBar);
    MainWindow w{Q_NULLPTR, reader};
    w.show();
    return a.exec();
}
