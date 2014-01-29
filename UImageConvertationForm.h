#ifndef UIMAGECONVERTATIONFORM_H
#define UIMAGECONVERTATIONFORM_H

#include <QtGui>
#include <QtCore>
#include <QtNetwork>

namespace Ui {
class UImageConvertationForm;
}

#include "UAboutProgram.h"

class UImageConvertationForm: public QMainWindow
{
    Q_OBJECT
    
public:
    UImageConvertationForm(QWidget *parent = 0);
    ~UImageConvertationForm();
    
private:
    void init();

private:
    Ui::UImageConvertationForm *ui;
    QNetworkAccessManager *networkAccessManager;
    QNetworkReply *reply;

public slots:
    void selectFilePicture();
    void saveAs();
    void downloadUrlImage();
    void abortDownload();
    void about();
    void selectFolder();

private slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void showPicture();
    void testingEditUrlPicture(QString text);

protected:
    bool eventFilter(QObject *object, QEvent *event);
};

#endif // UIMAGECONVERTATIONFORM_H
