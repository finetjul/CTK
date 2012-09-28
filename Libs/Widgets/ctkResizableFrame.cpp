/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0.txt

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QSizeGrip>

// CTK includes
#include "ctkResizableFrame.h"

//-----------------------------------------------------------------------------
class ctkResizableFramePrivate
{
  Q_DECLARE_PUBLIC(ctkResizableFrame);

protected:
  ctkResizableFrame* const q_ptr;

public:
  ctkResizableFramePrivate(ctkResizableFrame& object);
  ~ctkResizableFramePrivate();
  void init();
  void positionSizeGrip();

  QSizeGrip* SizeGrip;
  QSize OffsetSizeHint;

  QPoint MouseButtonPressGlobalPos;
  QSize CurrentOffset;
};

//-----------------------------------------------------------------------------
ctkResizableFramePrivate::ctkResizableFramePrivate(ctkResizableFrame& object)
  : q_ptr(&object)
  , SizeGrip(0)
{
}

//-----------------------------------------------------------------------------
ctkResizableFramePrivate::~ctkResizableFramePrivate()
{
  this->SizeGrip = 0; // will be deleted automatically
}

//-----------------------------------------------------------------------------
void ctkResizableFramePrivate::init()
{
  Q_Q(ctkResizableFrame);

  this->SizeGrip = new QSizeGrip(q);
  this->SizeGrip->installEventFilter(q);

  this->positionSizeGrip();
}

//-----------------------------------------------------------------------------
void ctkResizableFramePrivate::positionSizeGrip()
{
  Q_Q(ctkResizableFrame);
  if (!this->SizeGrip)
    {
    return;
    }
  int w = this->SizeGrip->sizeHint().width();
  int h = this->SizeGrip->sizeHint().height();
  int x = q->width() - w;
  int y = q->height() - h;
  this->SizeGrip->setGeometry(x, y, w, h);
}

//-----------------------------------------------------------------------------
ctkResizableFrame::ctkResizableFrame(QWidget *parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkResizableFramePrivate(*this))
{
  Q_D(ctkResizableFrame);
  d->init();
}

//-----------------------------------------------------------------------------
ctkResizableFrame::~ctkResizableFrame()
{
}

/*
//------------------------------------------------------------------------------
QSize ctkResizableFrame::minimumSizeHint()const
{
  Q_D(const ctkResizableFrame);
  return d->recomputeSizeHint(d->MinimumSizeHint);
}
*/

//------------------------------------------------------------------------------
QSize ctkResizableFrame::sizeHint()const
{
  Q_D(const ctkResizableFrame);
  QSize originalSizeHint = this->Superclass::sizeHint();
  QSize newSizeHint = originalSizeHint + d->OffsetSizeHint + d->CurrentOffset;
  qDebug() << newSizeHint;
  return newSizeHint;
}

//------------------------------------------------------------------------------
void ctkResizableFrame::resizeEvent(QResizeEvent* event)
{
  Q_D(ctkResizableFrame);
  this->Superclass::resizeEvent(event);
  d->positionSizeGrip();
}

//------------------------------------------------------------------------------
bool ctkResizableFrame::event(QEvent* event)
{
  Q_D(ctkResizableFrame);
  bool res = this->Superclass::event(event);
  if (event->type() == QEvent::ChildAdded &&
      d->SizeGrip)
    {
    d->SizeGrip->raise();
    }
  return res;
}

//------------------------------------------------------------------------------
bool ctkResizableFrame::eventFilter(QObject * watched, QEvent * event)
{
  Q_D(ctkResizableFrame);
  if (watched != d->SizeGrip)
    {
    return false;
    }
  switch(event->type())
    {
    case QEvent::MouseButtonPress:
      {
      d->MouseButtonPressGlobalPos = dynamic_cast<QMouseEvent*>(event)->globalPos();
      d->CurrentOffset = QSize();
      return true;
      break;
      }
    case QEvent::MouseMove:
      {
      QPoint offset = dynamic_cast<QMouseEvent*>(event)->globalPos() - d->MouseButtonPressGlobalPos;
      d->CurrentOffset = QSize(offset.x(), offset.y());
      this->updateGeometry();
      return true;
      break;
      }
    case QEvent::MouseButtonRelease:
      {
      //d->OffsetSizeHint += d->CurrentOffset;
      d->OffsetSizeHint = (d->OffsetSizeHint + d->CurrentOffset).expandedTo(QSize(0,0));
      d->CurrentOffset = QSize();
      return true;
      break;
      }
   default:
      break;
   }
  return false;
}

//#include "moc_ctkResizableFrame.h"
