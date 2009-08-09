/*
    This file is part of Choqok, the KDE micro-blogging client

    Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>

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
#include "mediamanager.h"
#include <kio/job.h>
#include <kio/jobclasses.h>
#include <kdebug.h>
#include <KDE/KLocale>
#include <kicon.h>
#include <QApplication>
#include <QHash>
#include <KEmoticons>
#include <KEmoticonsTheme>
#include <KPixmapCache>

namespace Choqok
{

class MediaManager::Private
{
public:
    Private()
    :emoticons(KEmoticons().theme()),cache("choqok-userimages")
    {}
    KEmoticonsTheme emoticons;
    KPixmapCache cache;
    QHash<KJob*, QString> queue;
    QPixmap defaultImage;
};

MediaManager::MediaManager()
    : QObject( qApp ), d(new Private)
{
  KIcon icon("image-loading");
  d->defaultImage = icon.pixmap(48);
  d->cache.setCacheLimit(20000);
}

MediaManager::~MediaManager()
{
    delete d;
    mSelf = 0L;
    kDebug();
}

MediaManager * MediaManager::mSelf = 0L;

MediaManager * MediaManager::self()
{
    if ( !mSelf )
        mSelf = new MediaManager;
    return mSelf;
}

QPixmap& MediaManager::defaultImage()
{
    return d->defaultImage;
}

QString MediaManager::parseEmoticons(const QString& text)
{
  return d->emoticons.parseEmoticons(text,KEmoticonsTheme::DefaultParse,QStringList() << "(e)");
}

QPixmap * MediaManager::fetchImage( const QString& remoteUrl, ReturnMode mode /*= Sync*/ )
{
    QPixmap *p = new QPixmap();
    if( d->cache.find(remoteUrl,*p) ) {
        return p;
    } else if(mode == Async) {
        if ( d->queue.values().contains( remoteUrl ) ) {
            ///The file is on the way, wait to download complete.
            return 0L;
        }
        KUrl srcUrl(remoteUrl);
        KIO::Job *job = KIO::storedGet( srcUrl, KIO::NoReload, KIO::HideProgressInfo ) ;
        if ( !job ) {
            kDebug() << "Cannot create a FileCopyJob!";
            QString errMsg = i18n( "Cannot create a KDE Job, please check your installation.");
            emit fetchError( remoteUrl, errMsg );
            return 0L;
        }
        d->queue.insert(job, remoteUrl );
        connect( job, SIGNAL( result( KJob* ) ), this, SLOT( slotImageFetched( KJob * ) ) );
        job->start();
    }
    return 0L;
}

void MediaManager::slotImageFetched( KJob * job )
{
    KIO::StoredTransferJob *baseJob = qobject_cast<KIO::StoredTransferJob *>( job );
    QString remote = d->queue.value(job);
    d->queue.remove( job );
    if ( job->error() ) {
        kDebug() << "Job error!" << job->error() << "\t" << job->errorString();
        QString errMsg = i18n( "Cannot download image from %1.",
                               job->errorString() );
        emit fetchError( remote, errMsg );
    } else {
        QPixmap p;
        if( p.loadFromData( baseJob->data() ) ) {
            d->cache.insert( remote, p );
            emit imageFetched( remote, p );
        } else {
            emit fetchError( remote, i18n( "Download failed, Returned file is corrupted" ) );
        }
    }
}

void MediaManager::clearImageCache()
{
  d->cache.discard();
}

QPixmap MediaManager::convertToGrayScale(const QPixmap& pic)
{
    QImage result = pic.toImage();
    for ( int y = 0; y < result.height(); ++y ) {
        for ( int x = 0; x < result.width(); ++x ) {
            int pixel = result.pixel( x, y );
            int gray = qGray( pixel );
            int alpha = qAlpha( pixel );
            result.setPixel( x, y, qRgba( gray, gray, gray, alpha ) );
        }
    }
    return QPixmap::fromImage( result );
}

}
#include "mediamanager.moc"