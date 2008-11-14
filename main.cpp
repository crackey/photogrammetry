#include <QtGui>

#include "photogrammetry.h"

int main(int argc, char **argv)
{
    //  std::locale::global(std::locale(""));
    setlocale(LC_ALL,"");
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());

    Photogrammetry *win = new Photogrammetry;
    win->show();
    
    return app.exec();
}
