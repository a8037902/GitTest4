/*
 * Copyright (c) 2014-2016 Alex Spataru <alex_spataru@outlook.com>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include "Window.h"
#include <qdebug.h>
#include <QSslSocket>

int main (int argc, char** argv)
{
    QApplication app (argc, argv);
    app.setApplicationVersion ("1.0");
    app.setApplicationName ("Bob's Badass App");

    qDebug() << QSslSocket::sslLibraryBuildVersionString();
     qDebug() << QSslSocket::sslLibraryVersionString();
     qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();
    Window window;
    window.show();

    return app.exec();
}
