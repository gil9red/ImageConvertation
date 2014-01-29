#include "UImageConvertationForm.h"
#include "ui_UImageConvertationForm.h"

class UListWidgetHTMLItem : public QItemDelegate
{
protected:
    virtual void drawDisplay ( QPainter * painter, const QStyleOptionViewItem & option, const QRect & rect, const QString & text ) const
    {
        QTextDocument doc;
        doc.setHtml(text);
        QPixmap pix(rect.size());
        QPainter p(&pix);
        p.setPen(Qt::NoPen);
        if (option.state & QStyle::State_Selected)
            p.setBrush(option.palette.highlight());
        else p.setBrush(option.palette.base());
        p.fillRect (pix.rect(), p.brush());
        doc.drawContents(&p, pix.rect());
        painter->drawPixmap(rect.x(), rect.y(), pix);
    }
};

/// PUBLIC
UImageConvertationForm::UImageConvertationForm(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::UImageConvertationForm)
{
    ui->setupUi(this);

    ui->lWidgetImages->setItemDelegate(new UListWidgetHTMLItem());

    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(ui->label);
    ui->toolBar->addSeparator();
    ui->toolBar->addWidget(ui->lEditUrlPicture);
    ui->toolBar->addWidget(ui->tButtonSelectFile);
    ui->toolBar->addSeparator();

    ui->toolBar_2->addSeparator();
    ui->toolBar_2->addWidget(ui->tButtonDownloadPicture);
    ui->toolBar_2->addWidget(ui->tButtonAbortDownloadPicture);
    ui->toolBar_2->addSeparator();

    ui->toolBar_3->addSeparator();
    ui->toolBar_3->addWidget(ui->tButtonSaveAs);
    ui->toolBar_3->addSeparator();

    ui->toolBar_4->addSeparator();
    ui->toolBar_4->addWidget(ui->tButtonQuit);


    init();

    testingEditUrlPicture(ui->lEditUrlPicture->text());

    networkAccessManager = new QNetworkAccessManager();

    ui->tButtonAbortDownloadPicture->setEnabled(false);
    ui->actionAbortDownloadPicture->setEnabled(false);

    ui->widgetProgressBar->hide();

    ui->tButtonSaveAs->setEnabled(false);
    ui->actionSaveAs->setEnabled(false);

    reply = 0;

    ui->statusBar->addWidget(ui->widgetProgressBar);

    ui->lEditUrlPicture->installEventFilter(this);
    ui->labelImage->installEventFilter(this);

    setContextMenuPolicy(Qt::NoContextMenu);

    selectFolder();
}

UImageConvertationForm::~UImageConvertationForm()
{
    delete ui;
    networkAccessManager->deleteLater();
}

/// PRIVATE
void UImageConvertationForm::init()
{
    connect(ui->tButtonSelectFile, SIGNAL(clicked()), this, SLOT(selectFilePicture()));
    connect(ui->tButtonDownloadPicture, SIGNAL(clicked()), this, SLOT(downloadUrlImage()));
    connect(ui->tButtonAbortDownloadPicture, SIGNAL(clicked()), this, SLOT(abortDownload()));
    connect(ui->lEditUrlPicture, SIGNAL(textChanged(QString)), this, SLOT(testingEditUrlPicture(QString)));
    connect(ui->tButtonSaveAs, SIGNAL(clicked()), this, SLOT(saveAs()));
    connect(ui->tButtonQuit, SIGNAL(clicked()), qApp, SLOT(quit()));

    connect(ui->actionQuit, SIGNAL(triggered()), qApp, SLOT(quit()));
    connect(ui->actionSelectFile, SIGNAL(triggered()), this, SLOT(selectFilePicture()));
    connect(ui->actionSelectFolder, SIGNAL(triggered()), this, SLOT(selectFolder()));
    connect(ui->actionDownloadPicture, SIGNAL(triggered()), this, SLOT(downloadUrlImage()));
    connect(ui->actionAbortDownloadPicture, SIGNAL(triggered()), this, SLOT(abortDownload()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(ui->actionAboutProgram, SIGNAL(triggered()), this, SLOT(about()));
}

/// PUBLIC SLOTS
void UImageConvertationForm::selectFilePicture()
{
    ui->labelErrors->hide();

    QString string;

    foreach(QByteArray byte, QImageReader::supportedImageFormats())
        string += "*." + byte + "; ";

    QString filters = QString("%1 (%2)")
            .arg("Файлы изображений")
            .arg(string);

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Открыть изображение"),
                                                    QString(),
                                                    filters);

    if(fileName.isEmpty()
            || fileName.isNull())
        return;

    ui->lEditUrlPicture->setText(fileName);

    ui->tButtonSaveAs->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
}
void UImageConvertationForm::saveAs()
{
    QString filters;

    foreach(QByteArray byte, QImageWriter::supportedImageFormats())
    {
        QString format = byte;
        filters += QString("%1 (%2)\n")
                .arg(format.toUpper())
                .arg("*." + format);
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Сохранить как..."),
                                                    QFileInfo(ui->lEditUrlPicture->text()).baseName(),
                                                    filters);

    if(fileName.isEmpty()
            || fileName.isNull())
        return;

    ui->labelImage->pixmap()->save(fileName);
}
void UImageConvertationForm::downloadUrlImage()
{
    ui->labelErrors->hide();

    qApp->processEvents();

    QString fileName = ui->lEditUrlPicture->text();

    // если это путь к файлу
    if(QFileInfo(fileName).isFile())
    {
        QPixmap picture(fileName);
        ui->labelImage->setPixmap(picture);
    }else
    {
        ui->tButtonDownloadPicture->setEnabled(false);
        ui->actionDownloadPicture->setEnabled(false);

        ui->tButtonAbortDownloadPicture->setEnabled(true);
        ui->actionAbortDownloadPicture->setEnabled(true);

        ui->progressBarDownload->show();

        QNetworkRequest request;
        request.setUrl(QUrl(fileName));
        reply = networkAccessManager->get(request);

        connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(downloadProgress(qint64, qint64)));
    }
}
void UImageConvertationForm::abortDownload()
{
    if(reply)
    {
        reply->abort();
        delete reply;
        reply = 0;

        ui->tButtonDownloadPicture->setEnabled(true);
        ui->actionDownloadPicture->setEnabled(true);

        ui->tButtonAbortDownloadPicture->setEnabled(false);
        ui->actionAbortDownloadPicture->setEnabled(false);

        ui->progressBarDownload->hide();
    }
}
void UImageConvertationForm::about()
{
    UAboutProgram *about = new UAboutProgram();
    about->exec();
    about->deleteLater();
}
void UImageConvertationForm::selectFolder()
{
    QString path/* = QFileDialog::getExistingDirectory(this)*/;

    path = "C:\\Qt Projects\\UImageConvertation\\UImageConvertation\\UImageConvertation\\resource";

    QDir dir(path);

//    QImageReader::supportedImageFormats()
    QFileInfoList fileList = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot);

    foreach (QFileInfo fileInfo, fileList)
    {
        // если найденный файл не поддерживается
        if(!QImageReader::supportedImageFormats().contains(fileInfo.suffix().toUtf8()))
                continue;

        QPixmap icon(fileInfo.filePath());

        QTextDocument document;
        QString text = QString("%1<br>%2<br>%3<br>%4<br>%5")
                .arg(QString("<b>Имя:</b> %1").arg(fileInfo.fileName()))
                .arg(QString("<b>Путь:</b> %1").arg(fileInfo.filePath()))
                .arg(QString("<b>Формат:</b> %1").arg(fileInfo.suffix().toUpper()))
                .arg(QString("<b>Размер:</b> %1x%2 пикселей").arg(icon.width()).arg(icon.height()))
                .arg(QString("<b>Размер:</b> %1 КБ").arg(qreal(fileInfo.size()) / 1024, 0, 'g', 2).replace('.', ','));
        document.setHtml(text);
        document.adjustSize();
//        qDebug() << document.pageSize() << document.size();
        QListWidgetItem *item = new QListWidgetItem();
        item->setText(document.toHtml());
        item->setTextAlignment(Qt::AlignTop | Qt::AlignLeft);
        item->setIcon(QIcon(icon));
        item->setSizeHint(QSize(/*item->sizeHint()*/document.size().width(), document.size().height()));

        Qt::CheckState checkState = Qt::Checked;
        Qt::ItemFlags flags = Qt::ItemIsSelectable
                | Qt::ItemIsEnabled
                | Qt::ItemIsUserCheckable
                | Qt::ItemIsEditable;

        item->setCheckState(checkState);
        item->setFlags(flags);

        ui->lWidgetImages->addItem(item);
    }
}

/// PRIVATE SLOTS
void UImageConvertationForm::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{       
    qApp->processEvents();

    if(bytesReceived <= 0
            && bytesTotal <= 0)
    {
        ui->labelErrors->show();
        ui->progressBarDownload->hide();

        ui->tButtonAbortDownloadPicture->setEnabled(false);
        ui->actionAbortDownloadPicture->setEnabled(false);

        ui->tButtonDownloadPicture->setEnabled(true);
        ui->actionDownloadPicture->setEnabled(true);

        ui->labelErrors->setText(tr("Что то пошло не так... ошибка, однако =(..."));
        return;
    }

    qint64 procent = (bytesReceived * 100) / bytesTotal;

    ui->progressBarDownload->setValue(procent);

    if(reply->error() != QNetworkReply::NoError)
    {
        ui->labelErrors->show();
        ui->progressBarDownload->hide();
        ui->labelErrors->setText(QString("Error: %1")
                             .arg(reply->errorString()));

    }else
        ui->labelErrors->hide();

    // если закончили загрузку
    if(bytesReceived == bytesTotal)
    {
        showPicture();

        reply->deleteLater();

        ui->tButtonDownloadPicture->setEnabled(true);
        ui->actionDownloadPicture->setEnabled(true);

        ui->tButtonAbortDownloadPicture->setEnabled(false);
        ui->actionAbortDownloadPicture->setEnabled(false);

        ui->progressBarDownload->hide();
    }
}

void UImageConvertationForm::showPicture()
{
    QImage image;
    QImageReader reader(reply);

    if (reply->error() == QNetworkReply::NoError)
        reader.read(&image);
    else
    {
        ui->labelErrors->show();
        ui->labelErrors->setText(QString("Reply error: %1")
                             .arg(reply->error()));
    }

    ui->labelImage->setPixmap(QPixmap::fromImage(image));

    ui->tButtonSaveAs->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
}

void UImageConvertationForm::testingEditUrlPicture(QString text)
{
    ui->tButtonDownloadPicture->setEnabled(!text.isEmpty());
    ui->actionDownloadPicture->setEnabled(!text.isEmpty());
}

/// PROTECTED
bool UImageConvertationForm::eventFilter(QObject *object, QEvent *event)
{
    if(event->type() == QEvent::DragEnter
            && (object == ui->lEditUrlPicture
                || object == ui->labelImage))
    {
        QDropEvent *dropEvent = static_cast <QDropEvent *> (event);
        const QMimeData *mimeData = dropEvent->mimeData();

        if(!mimeData->hasUrls())
            return QMainWindow::eventFilter(object, event);

        int count = mimeData->urls().count();

        if(count != 1)
            return QMainWindow::eventFilter(object, event);

        QUrl url = mimeData->urls().first();

        if(!url.isLocalFile())
            return QMainWindow::eventFilter(object, event);

        QList <QByteArray> formats = QImageReader::supportedImageFormats();

        QString path = url.toLocalFile();
        QString suffix = QFileInfo(path).suffix();

        if(formats.contains(suffix.toLocal8Bit()))
            dropEvent->acceptProposedAction();
    }

    if(object == ui->lEditUrlPicture)
    {
        if(event->type() == QEvent::Drop)
        {
            QDropEvent *dropEvent = static_cast <QDropEvent *> (event);
            const QMimeData *mimeData = dropEvent->mimeData();

            ui->lEditUrlPicture->setText(mimeData->urls().first().toLocalFile());
            dropEvent->acceptProposedAction();
        }

    }else if(object == ui->labelImage)
    {
        if(event->type() == QEvent::Drop)
        {
            QDropEvent *dropEvent = static_cast <QDropEvent *> (event);
            const QMimeData *mimeData = dropEvent->mimeData();

            ui->lEditUrlPicture->setText(mimeData->urls().first().toLocalFile());
            downloadUrlImage();
            ui->tButtonSaveAs->setEnabled(true);
            ui->actionSaveAs->setEnabled(true);

            dropEvent->acceptProposedAction();
        }
    }

    return QMainWindow::eventFilter(object, event);
}

