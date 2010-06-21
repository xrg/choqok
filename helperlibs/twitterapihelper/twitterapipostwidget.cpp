/*
    This file is part of Choqok, the KDE micro-blogging client

    Copyright (C) 2008-2010 Mehrdad Momeny <mehrdad.momeny@gmail.com>

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

#include "twitterapipostwidget.h"
#include <microblog.h>
#include <klocalizedstring.h>
#include "twitterapiaccount.h"
#include <kicon.h>
#include <KPushButton>
#include "twitterapimicroblog.h"
#include <KDebug>
#include <mediamanager.h>
#include <choqokappearancesettings.h>
#include "twitterapishowthread.h"
#include <textbrowser.h>

const QRegExp TwitterApiPostWidget::mUserRegExp("([\\s\\W]|^)@([a-zA-Z0-9_]+){1,64}");
const QRegExp TwitterApiPostWidget::mHashtagRegExp("([\\s]|^)#([a-zA-Z0-9_]+)");
const KIcon TwitterApiPostWidget::unFavIcon(Choqok::MediaManager::convertToGrayScale(KIcon("rating").pixmap(16)) );

class TwitterApiPostWidget::Private
{
public:
    Private(Choqok::Account* account)
        :isBasePostShowed(false)
    {
        mBlog = qobject_cast<TwitterApiMicroBlog*>( account->microblog() );
    }
    KPushButton *btnFav;
    bool isBasePostShowed;
    TwitterApiMicroBlog *mBlog;
};

TwitterApiPostWidget::TwitterApiPostWidget(Choqok::Account* account, const Choqok::Post &post, QWidget* parent)
    : PostWidget(account, post, parent), d(new Private(account))
{
    mainWidget()->document()->addResource( QTextDocument::ImageResource, QUrl("icon://thread"),
                             KIcon("go-top").pixmap(10) );
}

TwitterApiPostWidget::~TwitterApiPostWidget()
{
    delete d;
}

void TwitterApiPostWidget::initUi()
{
    Choqok::UI::PostWidget::initUi();

    KPushButton *btnRe = addButton( "btnReply",i18nc( "@info:tooltip", "Reply" ), "edit-undo" );
    connect( btnRe, SIGNAL(clicked(bool)), SLOT(slotReply()) );

    if( !currentPost().isPrivate ) {
        d->btnFav = addButton( "btnFavorite",i18nc( "@info:tooltip", "Favorite" ), "rating" );
        d->btnFav->setCheckable(true);
        connect( d->btnFav, SIGNAL(clicked(bool)), SLOT(setFavorite()) );
        updateFavStat();
    }
}

QString TwitterApiPostWidget::prepareStatus(const QString& text)
{
    QString res = Choqok::UI::PostWidget::prepareStatus(text);
    res.replace(mUserRegExp,"\\1@<a href='user://\\2'>\\2</a>");

    return res;
}

QString TwitterApiPostWidget::generateSign()
{
    QString sign;
    QString profUrl = currentAccount()->microblog()->profileUrl(currentAccount(),
                                                                currentPost().author.userName);
    sign = "<b><a href='user://"+currentPost().author.userName+"' title=\"" +
    currentPost().author.description + "\">" + currentPost().author.userName +
    "</a> - </b>";
    //<img src=\"icon://web\" />
    sign += "<a href=\"" + currentPost().link +
    "\" title=\"" + currentPost().creationDateTime.toString( Qt::DefaultLocaleLongDate ) + "\">%1</a>";
    if ( currentPost().isPrivate ) {
        if( currentPost().replyToUserName.compare( currentAccount()->username(), Qt::CaseInsensitive ) == 0 ) {
            sign.prepend( "From " );
        } else {
            sign.prepend( "To " );
        }
    } else {
        if( !currentPost().source.isNull() )
            sign += " - " + currentPost().source;
        if ( !currentPost().replyToPostId.isEmpty() ) {
            QString link = currentAccount()->microblog()->postUrl( currentAccount(), currentPost().replyToUserName,
                                                                   currentPost().replyToPostId );
            QString showConMsg = i18n("Show Conversation");
            QString threadlink = "thread://" + currentPost().postId;
            sign += " - " +
            i18n("<a href='replyto://%1'>in reply to</a>&nbsp;<a href=\"%2\" title=\"%2\">%3</a>",
                currentPost().replyToPostId, link, webIconText) + ' ';
            sign += "<a title=\""+ showConMsg +"\" href=\"" + threadlink + "\"><img src=\"icon://thread\" /></a>";
        }
    }

    //ReTweet detection:
    if( !currentPost().repeatedByUsername.isEmpty() ){
        QString retweet;
        QPixmap px = Choqok::MediaManager::convertToGrayScale(KIcon("retweet").pixmap(12));
        mainWidget()->document()->addResource(QTextDocument::ImageResource, QUrl("icon://repeated"),
                                              px);
        retweet += " | <a href='user://"+ currentPost().repeatedByUsername
                +  "'><img src='icon://repeated' title='"
                +  d->mBlog->generateRepeatedByUserTooltip(currentPost().repeatedByUsername) + "'/></a>";
        sign.append(retweet);
    }
    sign.prepend("<p dir='ltr'>");
    sign.append( "</p>" );
    return sign;
}

void TwitterApiPostWidget::slotReply()
{
    if(currentPost().isPrivate){
        TwitterApiAccount *account= qobject_cast<TwitterApiAccount*>( currentAccount() );
        d->mBlog->showDirectMessageDialog( account, currentPost().author.userName );
    } else {
        emit reply( QString("@%1").arg(currentPost().author.userName), currentPost().postId );
    }
}

void TwitterApiPostWidget::setFavorite()
{
    TwitterApiMicroBlog *mic = d->mBlog;
    if(currentPost().isFavorited){
        connect(mic, SIGNAL(favoriteRemoved(Choqok::Account*,QString)),
                this, SLOT(slotSetFavorite(Choqok::Account*,QString)) );
        mic->removeFavorite(currentAccount(), currentPost().postId);
    } else {
        connect(mic, SIGNAL(favoriteCreated(Choqok::Account*,QString)),
                   this, SLOT(slotSetFavorite(Choqok::Account*,QString)) );
        mic->createFavorite(currentAccount(), currentPost().postId);
    }
}

void TwitterApiPostWidget::slotSetFavorite(Choqok::Account *theAccount, const QString& postId)
{
    if(currentAccount() == theAccount && postId == currentPost().postId){
        kDebug()<<postId;
        Choqok::Post tmp = currentPost();
        tmp.isFavorited = !tmp.isFavorited;
        setCurrentPost(tmp);
        updateFavStat();
        disconnect(d->mBlog, SIGNAL(favoriteRemoved(Choqok::Account*,QString)),
                   this, SLOT(slotSetFavorite(Choqok::Account*,QString)) );
        disconnect(d->mBlog, SIGNAL(favoriteCreated(Choqok::Account*,QString)),
                   this, SLOT(slotSetFavorite(Choqok::Account*,QString)) );
        ///TODO Notify!
    }
}

void TwitterApiPostWidget::updateFavStat()
{
    if(currentPost().isFavorited){
        d->btnFav->setChecked(true);
        d->btnFav->setIcon(KIcon("rating"));
    } else {
        d->btnFav->setChecked(false);
        d->btnFav->setIcon(unFavIcon);
    }
}

void TwitterApiPostWidget::checkAnchor(const QUrl & url)
{
    QString scheme = url.scheme();
    if( scheme == "replyto" ) {
        if(d->isBasePostShowed) {
            setContent( prepareStatus(currentPost().content) );
            updateUi();
            d->isBasePostShowed = false;
            return;
        } else {
            connect(currentAccount()->microblog(), SIGNAL(postFetched(Choqok::Account*,Choqok::Post*)),
                    this, SLOT(slotBasePostFetched(Choqok::Account*,Choqok::Post*)) );
            Choqok::Post *ps = new Choqok::Post;
            ps->postId = url.host();
            currentAccount()->microblog()->fetchPost(currentAccount(), ps);
        }
    } else if (scheme == "thread") {
        TwitterApiShowThread *wd = new TwitterApiShowThread(currentAccount(), currentPost(), NULL);
        wd->resize(this->width(), wd->height());
        connect(wd, SIGNAL(forwardReply(QString,QString)),
                this, SIGNAL(reply(QString,QString)));
        connect(wd, SIGNAL(forwardResendPost(QString)),
                this, SIGNAL(resendPost(QString)));
        wd->show();
    } else {
        Choqok::UI::PostWidget::checkAnchor(url);
    }

}

void TwitterApiPostWidget::slotBasePostFetched(Choqok::Account* theAccount, Choqok::Post* post)
{
    if(theAccount == currentAccount() && post && post->postId == currentPost().replyToPostId){
        kDebug();
        disconnect( currentAccount()->microblog(), SIGNAL(postFetched(Choqok::Account*,Choqok::Post*)),
                   this, SLOT(slotBasePostFetched(Choqok::Account*,Choqok::Post*)) );
        if(d->isBasePostShowed)
            return;
        d->isBasePostShowed = true;
        QString color;
        if( Choqok::AppearanceSettings::isCustomUi() ) {
            color = Choqok::AppearanceSettings::readForeColor().lighter().name();
        } else {
            color = this->palette().dark().color().name();
        }
        QString baseStatusText = "<p style=\"margin-top:10px; margin-bottom:10px; margin-left:20px;\
        margin-right:20px; -qt-block-indent:0; text-indent:0px\"><span style=\" color:" + color + ";\">";
        baseStatusText += "<b><a href='user://"+ post->author.userName +"'>" +
        post->author.userName + "</a> :</b> ";

        baseStatusText += prepareStatus( post->content ) + "</p>";
        setContent( content().prepend( baseStatusText ) );
        updateUi();
        delete post;
    }
}

void TwitterApiPostWidget::repeatPost()
{
    d->mBlog->repeatPost(currentAccount(), currentPost().postId);
}


#include "twitterapipostwidget.moc"
