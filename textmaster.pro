QT += core gui
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/include
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
DESTDIR = .

QMAKE_CXXFLAGS += -Wextra

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = textmasterjp
TEMPLATE = app
RC_ICONS = resources/icons/togepi.ico

SOURCES += src/main.cpp \
           src/main_window.cpp \
           src/core/read_charmap.cpp \
           src/core/f_decoder.cpp	

HEADERS += include/main_window.h \
           include/core/read_charmap.h \
           include/core/f_decoder.h

FORMS += forms/main_window.ui

RESOURCES += resources/images.qrc \
             resources/qdarkstyle/style.qrc
