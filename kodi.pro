# Nom du projet
TARGET = mon_kodi_clone
TEMPLATE = app

# Qt modules nécessaires
QT += core gui widgets multimedia multimediawidgets

# Standard C++
CONFIG += c++17

# Sources
SOURCES += main.cpp \
           mainwindow.cpp \
           playlistmanager.cpp

# Headers
HEADERS += mainwindow.h \
           playlistmanager.h
