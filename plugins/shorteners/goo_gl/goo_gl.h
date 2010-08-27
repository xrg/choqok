/*
    This file is part of Choqok, the KDE micro-blogging client

    Copyright (C) 2010 Andrey Esin <gmlastik@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, see http://www.gnu.org/licenses/
*/

#ifndef GOO_GL_H
#define GOO_GL_H

#include <QtCore/QString>
#include <QtCore/QVariant>
#include <shortener.h>

class KJob;
/**
  @author Andrey Esin \<gmlastik@gmail.com\>
*/

class Goo_gl : public Choqok::Shortener
{
    Q_OBJECT
public:
    Goo_gl( QObject* parent, const QVariantList& args );
    ~Goo_gl();
    QString shorten( const QString& url );
    
private:
    qint64 first( const QByteArray &str );
    qint64 second( const QByteArray &str );
    qint64 third( const QList<qint64> &b );
    QByteArray fourth( qint64 l );
    QByteArray authToken( const QString &url );
    QString data;
    bool readyToParse;
  private slots:
    void slotReadyRead(KJob*);
};

#endif //GOO_GL_H