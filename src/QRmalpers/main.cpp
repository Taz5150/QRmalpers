#include "QRmalpers.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon("./res/malpers_raw.png"));
    QRmalpers w;
    w.show();
    return app.exec();
}
