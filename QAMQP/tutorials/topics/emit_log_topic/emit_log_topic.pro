DEPTH = ../../..
include($${DEPTH}/qamqp.pri)

TEMPLATE = app
INCLUDEPATH += $${QAMQP_INCLUDEPATH}
LIBS += -L$${DEPTH}/src $${QAMQP_LIBS}
macx:CONFIG -= app_bundle

SOURCES += main.cpp

OBJECTS_DIR = Debug/
MOC_DIR = .moc
