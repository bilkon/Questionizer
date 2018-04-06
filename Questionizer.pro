#-------------------------------------------------
#
# Project created by QtCreator 2018-04-03T15:43:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Questionizer
TEMPLATE = app


SOURCES += main.cpp\
        questionwidget.cpp \
    translationwidget.cpp \
    quizwidget.cpp \
    highlighter.cpp \
    quizmodel.cpp

HEADERS  += questionwidget.h \
    translationwidget.h \
    quizwidget.h \
    highlighter.h \
    quizmodel.h

FORMS    += questionwidget.ui \
    translationwidget.ui \
    quizwidget.ui
