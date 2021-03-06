/*
    This file is part of Choqok, the KDE micro-blogging client

    Copyright (C) 2010-2012 Andrey Esin <gmlastik@gmail.com>

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

#include "imstatus.h"
#include "imstatussettings.h"
#include "imqdbus.h"
#include <KAction>
#include <KActionCollection>
#include <KAboutData>
#include <KGenericFactory>
#include <choqokuiglobal.h>
#include <quickpost.h>
#include <KMessageBox>

K_PLUGIN_FACTORY ( MyPluginFactory, registerPlugin < IMStatus > (); )
K_EXPORT_PLUGIN ( MyPluginFactory ( "choqok_imstatus" ) )

IMStatus::IMStatus ( QObject* parent, const QList<QVariant>& )
        : Choqok::Plugin ( MyPluginFactory::componentData(), parent )
{
    QTimer::singleShot ( 500, this, SLOT ( update() ) );
}

IMStatus::~IMStatus()
{
}

void IMStatus::update()
{
    if ( Choqok::UI::Global::quickPostWidget() != 0 ) {
        connect ( Choqok::UI::Global::quickPostWidget(), SIGNAL ( newPostSubmitted ( Choqok::JobResult, Choqok::Post* ) ),
                  this, SLOT ( slotIMStatus ( Choqok::JobResult, Choqok::Post* ) ) );
    } else {
        QTimer::singleShot ( 500, this, SLOT ( update() ) );
    }
}

void IMStatus::slotIMStatus ( Choqok::JobResult res, Choqok::Post* newPost )
{
    if ( res == Choqok::Success ) {
        IMStatusSettings::self()->readConfig();
        QString statusMessage = IMStatusSettings::templtate();
        statusMessage.replace ( QString ( "%status%" ), newPost->content, Qt::CaseInsensitive );
        statusMessage.replace ( QString ( "%username%" ), newPost->author.userName, Qt::CaseInsensitive );
        statusMessage.replace ( QString ( "%fullname%" ), newPost->author.realName, Qt::CaseInsensitive );
        statusMessage.replace ( QString ( "%time%" ), newPost->creationDateTime.toString ( "hh:mm:ss" ), Qt::CaseInsensitive );
        statusMessage.replace ( QString ( "%url%" ), newPost->link, Qt::CaseInsensitive );
        statusMessage.replace ( QString ( "%client%" ), QString ( "Choqok" ), Qt::CaseInsensitive );
        if ( !IMStatusSettings::repeat() && !newPost->repeatedFromUsername.isEmpty() ) return;
        if ( !IMStatusSettings::reply() && !newPost->replyToUserName.isEmpty() ) return;
        IMQDBus im ( IMStatusSettings::imclient(), statusMessage );
    }
}
