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
TARGET = harbour-asterism

CONFIG += sailfishapp

INCLUDEPATH += src/
INCLUDEPATH += C:\SailfishOS\mersdk\targets\SailfishOS-2.2.0.29-armv7hl\usr\include\

LIBS += -lipfs

SOURCES += \
    src/harbour-asterism.cpp \
    src/ipfsapi.cpp

HEADERS += \
    src/ipfsapi.h

DISTFILES += \
    qml/harbour-asterism.qml \
    qml/cover/CoverPage.qml \
    harbour-asterism.desktop \
    qml/pages/MainPage.qml \
    qml/pages/WarningDialog.qml \
    qml/pages/InfoView.qml \
    qml/pages/TabPanel.qml \
    qml/pages/Indicator.qml \
    qml/pages/FilesView.qml \
    qml/pages/PeersView.qml \
    qml/pages/CreateDir.qml \
    qml/pages/SettingsView.qml

OTHER_FILES += \
    rpm/harbour-asterism.spec

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
# CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
# TRANSLATIONS += translations/harbour-asterism-de.ts
