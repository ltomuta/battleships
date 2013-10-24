# Copyright (c) 2011 Nokia Corporation.

TARGET = battleships
TEMPLATE = app

VERSION = 1.0

QT += declarative

HEADERS += \
    myqmlapplicationviewer.h \
    aiplayer.h

SOURCES += \
    main.cpp \
    myqmlapplicationviewer.cpp \
    aiplayer.cpp

RESOURCES += \
    battleships.qrc

OTHER_FILES += \
    qml/*.qml \
    qml/*.js \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog


win32: {
    # Publish the app version to source code.
    DEFINES += APP_VERSION=\\\"$$VERSION\\\"

    RESOURCES += \
        rsc/windows.qrc
}


symbian {
    # Publish the app version to source code.
    DEFINES += APP_VERSION=\"$$VERSION\"
    DEFINES += SUPPORT_2P_GAME
    DEFINES += SHOW_ALL_BT_DEVICES

    TARGET.UID3 = 0xE4B75D6F
    TARGET.EPOCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x020000 0x4000000 #64MB

    CONFIG += mobility
    MOBILITY = connectivity

    contains(DEFINES, SUPPORT_2P_GAME) {
        SOURCES += \
            gameserver.cpp \
            gameclient.cpp \
            serverselector.cpp

        HEADERS += \
            gameserver.h \
            gameclient.h \
            serverselector.h
    }

    contains(DEFINES, SUPPORT_2P_GAME) {
        # Allow network access on Symbian
        TARGET.CAPABILITY += \
            NetworkServices \
            LocalServices \
            UserEnvironment \
            ReadUserData \
            WriteUserData \
            NetworkServices
    }

    RESOURCES += \
        rsc/symbian.qrc

    # To lock the application to landscape orientation
    LIBS += -lcone -leikcore -lavkon

    ICON = icons/battleships.png

    # Target name with Capital letter to look better in Symbian menu
    TARGET = Battleships
}

unix:!symbian:!maemo5 {
    message(Harmattan build)
    DEFINES += Q_WS_HARMATTAN
    DEFINES += SUPPORT_2P_GAME

    # Publish the app version to source code.
    DEFINES += APP_VERSION=\\\"$$VERSION\\\"

    CONFIG += mobility
    MOBILITY = connectivity

    contains(DEFINES, SUPPORT_2P_GAME) {
        SOURCES += \
            gameserver.cpp \
            gameclient.cpp \
            serverselector.cpp

        HEADERS += \
            gameserver.h \
            gameclient.h \
            serverselector.h
    }

    RESOURCES += \
        rsc/harmattan.qrc

    target.path = /opt/usr/bin

    desktopfile.files = qtc_packaging/debian_harmattan/battleships.desktop
    desktopfile.path = /usr/share/applications

    icon.files = icons/battleships.png
    icon.path = /usr/share/icons/hicolor/64x64/apps

    gameclassify.files += qtc_packaging/debian_harmattan/battleships.conf
    gameclassify.path = /usr/share/policy/etc/syspart.conf.d

    INSTALLS += \
        target \
        desktopfile \
        icon
}

