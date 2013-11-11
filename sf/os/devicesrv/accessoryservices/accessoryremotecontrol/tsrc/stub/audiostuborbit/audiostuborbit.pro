TEMPLATE = app
TARGET = audiostuborbit
CONFIG += hb
HEADERS += audiostuborbit.h \
		   audioclientstubremconobserver.h
SOURCES += main.cpp \
           audiostuborbit.cpp \
           audioclientstubremconobserver.cpp
           
# Libs
LIBS += -ltspclientmapper \
		-lremconcoreapi \
		-lremconinterfacebase

symbian:TARGET.UID3 = 0xE51D7B4B
