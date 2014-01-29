#ifndef UABOUTPROGRAM_H
#define UABOUTPROGRAM_H

#include <QtGui>
#include <QtCore>

namespace Ui {
class UAboutProgram;
}

class UAboutProgram: public QDialog
{
    Q_OBJECT
    
public:
    UAboutProgram(QWidget *parent = 0);
    ~UAboutProgram();

private:
    Ui::UAboutProgram *ui;
};

#endif // UABOUTPROGRAM_H
