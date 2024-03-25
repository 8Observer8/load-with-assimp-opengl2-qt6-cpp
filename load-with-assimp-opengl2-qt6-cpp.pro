QT += core gui openglwidgets widgets

win32: LIBS += -lopengl32

INCLUDEPATH += $$PWD/libs/assimp-5.2.5-mingw-64-bit/include
LIBS += -L$$PWD/libs/assimp-5.2.5-mingw-64-bit/lib
LIBS += -lassimp

CONFIG += c++11

SOURCES += \
    main.cpp
