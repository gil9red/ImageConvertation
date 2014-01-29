QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WikiCImage
TEMPLATE = app

DESTDIR = ../bin

SOURCES += main.cpp\
        UImageConvertationForm.cpp \
    UAboutProgram.cpp

HEADERS  += UImageConvertationForm.h \
    UAboutProgram.h

FORMS    += UImageConvertationForm.ui \
    UAboutProgram.ui

RESOURCES += \
    resource.qrc

OTHER_FILES += \
    app.rc

win32 {
    RC_FILE = app.rc
}
