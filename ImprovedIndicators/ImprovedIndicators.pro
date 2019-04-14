TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

HEADERS += \
    simulator.h \
    pso.h \
    cs.h \
    antibody.h \
    particle.h \
    sharperatio.h \
    sterlingratio.h \
    csvreader.h \
    signalize.h
