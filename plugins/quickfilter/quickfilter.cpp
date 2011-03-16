/*
    Copyright (C) 2011  Farhad Hedayati-Fard <hf.farhad@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "quickfilter.h"
#include <KGenericFactory>
#include <choqokuiglobal.h>
#include "postwidget.h"
#include "choqoktypes.h"
#include <qmutex.h>
#include <QDomDocument>
#include <microblogwidget.h>
#include <KLineEdit>
#include <QToolBar>
#include <timelinewidget.h>
#include <KMenuBar>
#include <KAction>
#include <KActionCollection>
#include <QLabel>
#include <KCompletionBox>
#include <QPushButton>
#include <quickpost.h>

K_PLUGIN_FACTORY( MyPluginFactory, registerPlugin < QuickFilter > (); )
K_EXPORT_PLUGIN( MyPluginFactory( "choqok_quickfilter" ) )

QuickFilter::QuickFilter(QObject* parent, const QList< QVariant >& args) : Choqok::Plugin(MyPluginFactory::componentData(), parent)
{
  m_authorAction = new KAction(KIcon("document-preview"), i18n("Filter by author"), this);
  m_authorAction->setCheckable(true);
  m_textAction = new KAction(KIcon("document-preview"), i18n("Filter by content"), this);
  m_textAction->setCheckable(true);
  actionCollection()->addAction("filterByAuthor", m_authorAction);
  actionCollection()->addAction("filterByContent", m_textAction);
  connect(m_authorAction, SIGNAL(toggled(bool)), SLOT(showAuthorFilterUiInterface(bool)));
  connect(m_textAction, SIGNAL(toggled(bool)), SLOT(showTextFilterUiInterface(bool)));
  setXMLFile("quickfilterui.rc");
  createUiInterface();
}

QuickFilter::~QuickFilter()
{

}

void QuickFilter::filterByAuthor()
{
  m_filterUser = m_aledit->text();
  if (!m_filterUser.isEmpty()) {
    foreach(Choqok::UI::PostWidget* postwidget, Choqok::UI::Global::mainWindow()->currentMicroBlog()->currentTimeline()->postWidgets()) {
	if (postwidget->currentPost().author.userName != m_filterUser) {
	  postwidget->hide();
      }
    }
  }
  else {
    foreach(Choqok::UI::PostWidget* postwidget, Choqok::UI::Global::mainWindow()->currentMicroBlog()->currentTimeline()->postWidgets()) {
	postwidget->show();
    }
  }
}

void QuickFilter::filterByText()
{
  m_filterText = m_tledit->text();
  if (!m_filterText.isEmpty()) {
    foreach(Choqok::UI::PostWidget* postwidget, Choqok::UI::Global::mainWindow()->currentMicroBlog()->currentTimeline()->postWidgets()) {
	if ( ! postwidget->currentPost().content.contains(m_filterText, Qt::CaseInsensitive) ) {
	  postwidget->hide();
	}
      }
  }
  else {
    foreach(Choqok::UI::PostWidget* postwidget, Choqok::UI::Global::mainWindow()->currentMicroBlog()->currentTimeline()->postWidgets()) {
	postwidget->show();
    }
  }
}

void QuickFilter::createUiInterface()
{
  m_authorToolbar = new QToolBar(Choqok::UI::Global::mainWindow());
  m_textToolbar = new QToolBar(Choqok::UI::Global::mainWindow());
  m_aledit = new KLineEdit(m_authorToolbar);
  m_aledit->setClearButtonShown(true);

  m_tledit = new KLineEdit(m_textToolbar);
  m_tledit->setClearButtonShown(true);

  
  QLabel *alabel = new QLabel(i18n("Author"), m_authorToolbar);
  QLabel *tlabel = new QLabel(i18n("Text"), m_textToolbar);
  m_authorToolbar->addWidget(alabel);
  m_authorToolbar->addWidget(m_aledit);
  QPushButton *authorCloseButton = new QPushButton(KIcon("dialog-close"), QString() , m_authorToolbar);
  authorCloseButton->setMaximumWidth(authorCloseButton->height());
  connect(authorCloseButton, SIGNAL(clicked(bool)), this, SLOT(hideAuthorFilterbar()));
  m_authorToolbar->addWidget(authorCloseButton);
  
  m_textToolbar->addWidget(tlabel);
  m_textToolbar->addWidget(m_tledit);
  QPushButton *textCloseButton = new QPushButton(KIcon("dialog-close"), QString() , m_textToolbar);
  textCloseButton->setMaximumWidth(textCloseButton->height());
  connect(textCloseButton, SIGNAL(clicked(bool)), this, SLOT(hideTextFilterbar()));
  m_textToolbar->addWidget(textCloseButton);

  connect(m_aledit, SIGNAL(editingFinished()), this , SLOT(filterByAuthor()));
  connect(m_aledit, SIGNAL(textChanged(QString)), this, SLOT(updateUser(QString)));
  
  connect(m_tledit, SIGNAL(editingFinished()), this, SLOT(filterByText()));
  connect(m_tledit, SIGNAL(textChanged(QString)), this, SLOT(updateText(QString)));
  
  Choqok::UI::Global::mainWindow()->addToolBar(Qt::BottomToolBarArea, m_authorToolbar);
  Choqok::UI::Global::mainWindow()->addToolBar(Qt::BottomToolBarArea, m_textToolbar);
  m_authorToolbar->hide();
  m_textToolbar->hide();
}

void QuickFilter::showAuthorFilterUiInterface(bool show)
{
    if (show) 
        m_authorToolbar->show();
    else
        m_authorToolbar->hide();
}

void QuickFilter::showTextFilterUiInterface(bool show)
{
    if (show)
        m_textToolbar->show();
    else
        m_textToolbar->hide();
}

void QuickFilter::updateUser(QString user)
{
  if (user.isEmpty()) {
      filterByAuthor();
  }
}

void QuickFilter::updateText(QString text)
{
  if (text.isEmpty()) {
      filterByText();
  }
}

void QuickFilter::hideAuthorFilterbar()
{
    m_authorToolbar->hide();
    m_authorAction->setChecked(false);
}

void QuickFilter::hideTextFilterbar()
{
    m_textToolbar->hide();
    m_textAction->setChecked(false);
}



