/*
    This file is part of Choqok, the KDE micro-blogging client
    Copyright (C) 2011-2012  Mehrdad Momeny <mehrdad.momeny@gmail.com>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "notifysettings.h"
#include <QMap>
#include <QDesktopWidget>
#include <KConfigGroup>
#include <KGlobal>
#include <KSharedConfig>
#include <KSharedConfigPtr>
#include <accountmanager.h>

class NotifySettings::Private
{
public:
    QMap<QString, QStringList> accounts;
    KConfigGroup *accountsConf;
    KConfigGroup *conf;
    int interval;
    QPoint position;
    QColor foregroundColor;
    QColor backgroundColor;
    QFont font;
};

NotifySettings::NotifySettings(QObject* parent)
:QObject(parent), d(new Private)
{
    d->conf = new KConfigGroup(KGlobal::config(), QString::fromLatin1( "BetterNotify Plugin" ));
    d->accountsConf = new KConfigGroup(KGlobal::config(), QString::fromLatin1( "BetterNotify Accounts Config" ));
    load();
}

NotifySettings::~NotifySettings()
{
    save();
    delete d->accountsConf;
}

QMap< QString, QStringList > NotifySettings::accounts()
{
    return d->accounts;
}

void NotifySettings::setAccounts(QMap< QString, QStringList > accounts)
{
    d->accounts = accounts;
}

void NotifySettings::load()
{
    d->accounts.clear();
    d->accountsConf->sync();
    foreach(Choqok::Account* acc, Choqok::AccountManager::self()->accounts()) {
        d->accounts.insert( acc->alias(), d->accountsConf->readEntry(acc->alias(), QStringList()));
    }
    d->conf->sync();
    d->interval = d->conf->readEntry("Interval", 7);
    QRect screenRect(QDesktopWidget().screenGeometry());
    QPoint defaultPosition = QPoint(screenRect.center().x()-NOTIFICATION_WIDTH/2, 30);
    d->position = d->conf->readEntry("NotifyPosition", defaultPosition);
    screenRect.adjust(0, 0, -1*NOTIFICATION_WIDTH, -1*NOTIFICATION_HEIGHT);
    if(!screenRect.contains( d->position )){
        d->position = defaultPosition;
    }
    d->backgroundColor = d->conf->readEntry("NotifyBackground", QColor(0, 0, 0));
    d->foregroundColor = d->conf->readEntry("NotifyForeground", QColor(255, 255, 255));
    d->font = d->conf->readEntry("NotifyFont", QFont());
}

void NotifySettings::save()
{
    foreach(Choqok::Account* acc, Choqok::AccountManager::self()->accounts()) {
        d->accountsConf->writeEntry(acc->alias(), d->accounts.value(acc->alias()));
    }
    d->conf->writeEntry("Interval", d->interval);
    d->conf->writeEntry("NotifyPosition", d->position);
    d->conf->writeEntry("NotifyBackground", d->backgroundColor);
    d->conf->writeEntry("NotifyForeground", d->foregroundColor);
    d->conf->writeEntry("NotifyFont", d->font);
    d->accountsConf->sync();
}

int NotifySettings::notifyInterval() const
{
    return d->interval;
}

void NotifySettings::setNotifyInterval(int interval)
{
    d->interval = interval;
}

QPoint NotifySettings::position() const
{
    return d->position;
}

void NotifySettings::setPosition(const QPoint& position)
{
    d->position = position;
}

QColor NotifySettings::backgroundColor() const
{
    return d->backgroundColor;
}

void NotifySettings::setBackgroundColor(const QColor& color)
{
    d->backgroundColor = color;
}

QFont NotifySettings::font() const
{
    return d->font;
}

void NotifySettings::setFont(const QFont& font)
{
    d->font = font;
}

QColor NotifySettings::foregroundColor() const
{
    return d->foregroundColor;
}

void NotifySettings::setForegroundColor(const QColor& color)
{
    d->foregroundColor = color;
}
