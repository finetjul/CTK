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

#ifndef __ctkSizeGrip_h
#define __ctkSizeGrip_h

#include <QtGui/qwidget.h>
class ctkSizeGripPrivate;

class CTK_WIDGETS_EXPORT ctkSizeGrip: public QWidget
{
  Q_OBJECT
  Q_PROPERTY(Qt::Orientations orientations READ orientations WRITE setOrientations)

public:
  explicit ctkSizeGrip(QWidget *parent);
  explicit ctkSizeGrip(QWidget* widgetToResize, QWidget *parent);

  Qt::Orientations orientations()const;
  void setOrientations(Qt::Orientations orientations);

  QWidget* widgetToResize()const;
  void setWidgetToResize(QWidget* target);
  QSize widgetSizeHint()const;

  virtual QSize sizeHint() const;

public Q_SLOTS:

protected:
    void paintEvent(QPaintEvent* );
    void mousePressEvent(QMouseEvent* );
    void mouseMoveEvent(QMouseEvent* );
    void mouseReleaseEvent(QMouseEvent* mouseEvent);
    void moveEvent(QMoveEvent* moveEvent);
    void showEvent(QShowEvent* showEvent);
    void hideEvent(QHideEvent* hideEvent);
    bool eventFilter(QObject* , QEvent* );
    bool event(QEvent* );
#ifdef Q_WS_WIN
    bool winEvent(MSG *m, long *result);
#endif

public:

private:
  Q_DECLARE_PRIVATE(ctkSizeGrip)
  Q_DISABLE_COPY(ctkSizeGrip)
//  Q_PRIVATE_SLOT(d_func(), void _q_showIfNotHidden())
};

#endif // __ctkSizeGrip_h
