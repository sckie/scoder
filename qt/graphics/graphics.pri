INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

QT += \
    multimedia \
    multimediawidgets \

HEADERS += \
    $$PWD/qscitembase.h \
    $$PWD/qscitemline.h \
    $$PWD/qscitemfolder.h \
    $$PWD/qscitemdrawer.h \
    $$PWD/qscitemmovie.h \
    $$PWD/qscitemrect.h \
    $$PWD/qscitemvideo.h

SOURCES += \
    $$PWD/qscitembase.cpp \
    $$PWD/qscitemline.cpp \
    $$PWD/qscitemfolder.cpp \
    $$PWD/qscitemdrawer.cpp \
    $$PWD/qscitemmovie.cpp \
    $$PWD/qscitemrect.cpp \
    $$PWD/qscitemvideo.cpp
