
TEMPLATE = app
CONFIG += c++17
CONFIG += qt

QT += widgets printsupport  core gui widgets

# Source and Header Files
SOURCES += \
    camerawindow.cpp \
    main.cpp \
    base/app_context.cpp \
    base/project.cpp \
    base/layer.cpp \
    base/layermanager.cpp \
    base/toolcontroller.cpp \
    base/workspace.cpp \
    base/tool.cpp \
    layer/bitmaplayer.cpp \
    layer/textlayer.cpp \
    layer/imagelayer.cpp \
    tool/pencil.cpp \
    tool/eraser.cpp \
    tool/brush.cpp \
    tool/fillcolor.cpp \
    tool/eyedropper.cpp \
    tool/text.cpp \
    utility/mouseeventhelper.cpp \
    utility/colorpicker.cpp \
    utility/gradientwidget.cpp \
    utility/coloritem.cpp \
    utility/fontselector.cpp \
    mainwindow.cpp \
    projectwizard.cpp \
    newproject.cpp \
    openproject.cpp \
    projectfromimage.cpp \
    importimage.cpp \
    exportproject.cpp \
    about.cpp \

HEADERS += \
    base/app_context.h \
    base/module.h \
    base/config.h \
    base/dialogs.h \
    base/project.h \
    base/layer.h \
    base/layermanager.h \
    base/toolcontroller.h \
    base/workspace.h \
    base/tool.h \
    camerawindow.h \
    layer/bitmaplayer.h \
    layer/textlayer.h \
    layer/imagelayer.h \
    tool/pencil.h \
    tool/eraser.h \
    tool/brush.h \
    tool/fillcolor.h \
    tool/eyedropper.h \
    tool/text.h \
    utility/mouseeventhelper.h \
    utility/colorpicker.h \
    utility/gradientwidget.h \
    utility/coloritem.h \
    utility/fontselector.h \
    mainwindow.h \
    projectwizard.h \
    newproject.h \
    openproject.h \
    projectfromimage.h \
    importimage.h \
    exportproject.h \
    about.h

FORMS += \
    camerawindow.ui \
    mainwindow.ui \
    projectwizard.ui \
    newproject.ui \
    openproject.ui \
    projectfromimage.ui \
    importimage.ui \
    exportproject.ui \
    about.ui

RESOURCES += \
    icons.qrc \
    theme.qrc \
    splash.qrc


INCLUDEPATH +=  c:/libraries/toupcam/include
LIBS += -LC:/libraries/toupcam/lib  -ltoupcam

win32:LIBS += -lSetupAPI -lOle32 -lUser32 -lGdi32 -lAdvapi32

CONFIG += macx-bundle
