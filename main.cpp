/*
 Embedded Widgets Demo
 Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).*
 Contact:  Qt Software Information (qt-info@nokia.com)**
 This file may be used under the terms of the Embedded Widgets Demo License
 Agreement.
*/

#include <QtGui>
#include <QDebug>

#include "mainwindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    app.setStyle(new QWindowsStyle());
#endif

    for (int i = 0; i < argc; ++i) {
        if (QString(argv[i]) == QString("-h") ||
            QString(argv[i]) == QString("--help") ||
            QString(argv[i]) == QString("-help")) {

            qDebug() << "Usage:";
            qDebug() << " -embedded : show in fullscreen mode";
            qDebug() << " -no-embedded : show in desktop mode";
            return 0;
        }
    }

    MainWindow* mainWindow = new MainWindow();
    mainWindow->show();

    return app.exec();
}
