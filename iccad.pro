#-------------------------------------------------
#
# Project created by QtCreator 2014-10-07T15:44:23
#
#-------------------------------------------------

# test
CONFIG += qt warn_on c++11
QT       += core gui widgets

TARGET = iccad
TEMPLATE = app

test {
    QT += testlib
    INCLUDEPATH += /home/user/qt5/qtbase/tests/auto/other/modeltest/
    DEPENDPATH += /home/user/qt5/qtbase/tests/auto/other/modeltest/
    SOURCES += \
        /home/user/qt5/qtbase/tests/auto/other/modeltest/modeltest.cpp \
        /home/user/qt5/qtbase/tests/auto/other/modeltest/dynamictreemodel.cpp
    #    /home/user/qt5/qtbase/tests/auto/other/modeltest/tst_modeltest.cpp
    HEADERS += \
        /home/user/qt5/qtbase/tests/auto/other/modeltest/modeltest.h \
        /home/user/qt5/qtbase/tests/auto/other/modeltest/dynamictreemodel.h
}


SOURCES += main.cpp \
        mainwindow.cpp \
    settings.cpp \
    tablemodelglobalvariables.cpp \
    sourcesyntaxhighlighter.cpp \
    tableglobalvariablesproxymodel.cpp \
    treemodeltranfsormations.cpp \
    editorchooseglobalvariable.cpp \
    texttermformatting.cpp

HEADERS  += mainwindow.h \
    settings.h \
    base_types.hpp \
    tablemodelglobalvariables.h \
    sourcesyntaxhighlighter.h \
    tableglobalvariablesproxymodel.h \
    treemodeltranfsormations.h \
    editorchooseglobalvariable.h \
    texttermformatting.h

FORMS    += mainwindow.ui

RESOURCES += \
    iccad.qrc

LIBS += -linsitucd

QMAKE_CXXFLAGS += -isystem "$(QTDIR)"
QMAKE_CXXFLAGS += -isystem "$(QTDIR)/include"
QMAKE_CXXFLAGS += -isystem "$(QTDIR)/include/QtCore"
QMAKE_CXXFLAGS += -isystem "$(QTDIR)/include/QtGui"
