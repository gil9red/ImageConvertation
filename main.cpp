#include "UImageConvertationForm.h"
#include <QApplication>

class UTextCodec
{
public:
    static void initTextCodecs(const char *codecName)
    {
        QTextCodec *codec = QTextCodec::codecForName(codecName);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForTr(codec);
    }
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/IconProgram"));
    app.setApplicationName("WikiCImage");
    app.setApplicationVersion("1.0.0");

    UTextCodec::initTextCodecs("cp1251");

    UImageConvertationForm w;
    w.setWindowTitle(app.applicationName());
    w.show();

    QStringList filters;
    foreach (QByteArray filter, QImageReader::supportedImageFormats())
        filters << filter;

    QFileSystemModel fileSystemModel;
    fileSystemModel.setNameFilters(filters);
    qDebug() << fileSystemModel.nameFilters();
//    fileSystemModel.setNameFilterDisables(false);
    fileSystemModel.setRootPath(QDir::currentPath());
    QTreeView v;
    v.setModel(&fileSystemModel);
    v.show();

    return app.exec();
}
