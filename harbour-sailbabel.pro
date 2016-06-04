# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-sailbabel

CONFIG += c++11
CONFIG += sailfishapp

QMAKE_CXXFLAGS += -std=c++11

SOURCES += src/harbour-sailbabel.cpp \
    src/dictionary.cpp \
    src/folderlistmodel.cpp

OTHER_FILES += qml/harbour-sailbabel.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-sailbabel.changes.in \
    rpm/harbour-sailbabel.spec \
    rpm/harbour-sailbabel.yaml \
    translations/*.ts \
    harbour-sailbabel.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

TRANSLATIONS += translations/harbour-sailbabel.ts \
  translations/harbour-sailbabel.en.ts \
  translations/harbour-sailbabel.de.ts

DISTFILES += qml/harbour-sailbabel.qml \
    qml/pages/MainPage.qml \
    qml/cover/CoverPage.qml \
    qml/pages/LoadDictionary.qml \
    qml/pages/About.qml \
    qml/pages/Error.qml \
    qml/pages/ChooseDictionary.qml \
    qml/pages/History.qml

images.files = images/cover_background.png
images.path = /usr/share/harbour-sailbabel/images
INSTALLS += images

HEADERS += \
    src/dictionary.hpp \
    src/folderlistmodel.hpp

