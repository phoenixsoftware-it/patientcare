TEMPLATE = app
INCLUDEPATH += .

TARGET = patientcare

include(ewiframework/third_party/common/common.pri)
include(ewiframework/third_party/svgslideswitch/svgslideswitch.pri)
include(ewiframework/third_party/scrolldial/scrolldial.pri)
include(ewiframework/third_party/5waybutton/5waybutton.pri)
include(ewiframework/third_party/multislider/multislider.pri)
include(ewiframework/third_party/basicgraph/basicgraph.pri)
include(ewiframework/third_party/svgtoggleswitch/svgtoggleswitch.pri)

CONFIG += release
CONFIG -= debug

RESOURCES += patientcare.qrc \
    ewiframework/third_party/skins/beryl_multislider.qrc \
    ewiframework/third_party/skins/beryl_svgslideswitch.qrc \
    ewiframework/third_party/skins/beryl_scrolldial.qrc \
    ewiframework/third_party/skins/beryl_5waybutton.qrc \
    ewiframework/third_party/skins/beryl_scrollwheel.qrc \
    ewiframework/third_party/skins/beryl_svgtoggleswitch.qrc


HEADERS += \
    mainwindow.h \
    datagenerator.h \
    patientcarecontroller.h \
    lazytimer.h

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    datagenerator.cpp \
    patientcarecontroller.cpp \
    lazytimer.cpp

FORMS = patientcare.ui

target.path = /usr/bin
INSTALLS += target
QT *= svg
