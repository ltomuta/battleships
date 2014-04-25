/*
 * Copyright (c) 2011-2014 Microsoft Mobile.
 */

#include <QtGui>
#include "myqmlapplicationviewer.h"

// Lock orientation in Symbian
#ifdef Q_OS_SYMBIAN
#include <eikenv.h>
#include <eikappui.h>
#include <aknenv.h>
#include <aknappui.h>
#endif


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set the version number for the application,
    // APP_VERSION is defined in .pro file.
    app.setApplicationVersion(APP_VERSION);

    MyQmlApplicationViewer viewer;
    viewer.setResizeMode(QDeclarativeView::SizeRootObjectToView);
    viewer.setSource(QUrl("qrc:/qml/main.qml"));

#ifdef Q_OS_SYMBIAN
    // Lock orientation in Symbian
    CAknAppUi* appUi =
            dynamic_cast<CAknAppUi*> (CEikonEnv::Static()->AppUi());
    TRAP_IGNORE(
        if (appUi) {
            appUi->SetOrientationL(CAknAppUi::EAppUiOrientationPortrait);
        }
    );
#endif

#if defined(Q_WS_HARMATTAN) || defined(Q_OS_SYMBIAN)
    viewer.setGeometry(QApplication::desktop()->screenGeometry());
    viewer.showFullScreen();
#else
    viewer.setFixedSize(QSize(480, 854));
    //viewer.setFixedSize(QSize(360, 640));
    viewer.show();
#endif

    viewer.connectSignalsAndSlots();

    return app.exec();
}
