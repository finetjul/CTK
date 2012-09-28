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

#ifndef __ctkResizableFrame_h
#define __ctkResizableFrame_h

// Qt includes
#include <QFrame>
class QResizeEvent;

// CTK includes
#include "ctkWidgetsExport.h"
class ctkResizableFramePrivate;

/**
 * \ingroup Widgets
 * \brief Frame that can be resized by the user.
*/
class CTK_WIDGETS_EXPORT ctkResizableFrame: public QFrame
{
  Q_OBJECT
public:
  typedef QFrame Superclass;

  /** Constructor
   *  /param parent       Parent widget
  */
  ctkResizableFrame(QWidget *parent=0);
  virtual ~ctkResizableFrame();

  //virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;
  virtual bool eventFilter(QObject * watched, QEvent * event);

public Q_SLOTS:

protected:
  QScopedPointer<ctkResizableFramePrivate> d_ptr;

  virtual void resizeEvent(QResizeEvent* event);
  virtual bool event(QEvent* event);
  
private:
  Q_DECLARE_PRIVATE(ctkResizableFrame);
  Q_DISABLE_COPY(ctkResizableFrame);

  //Q_PRIVATE_SLOT(d_ptr, void _q_recomputeCompleterPopupSize())
};

#endif // __ctkResizableFrame_h
