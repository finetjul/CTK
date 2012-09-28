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

// CTK includes
#include "ctkSizeGrip.h"

#include <QDebug>
#include <QEvent>
#include <QLayout>
#include <QStyle>
#include <QStyleOption>
#include <QPainter>

#define SZ_SIZEBOTTOMRIGHT  0xf008
#define SZ_SIZEBOTTOMLEFT   0xf007
#define SZ_SIZETOPLEFT      0xf004
#define SZ_SIZETOPRIGHT     0xf005

//------------------------------------------------------------------------------
static inline bool hasHeightForWidth(QWidget *widget)
{
  if (!widget)
    {
    return false;
    }
  if (QLayout *layout = widget->layout())
    {
    return layout->hasHeightForWidth();
    }
  return widget->sizePolicy().hasHeightForWidth();
}

class ctkSizeGripPrivate
{
  Q_DECLARE_PUBLIC(ctkSizeGrip)
public:
  void init();

  QPoint p;
  QRect r;
  int d;
  int dxMax;
  int dyMax;
  Qt::Corner m_corner;
  bool gotMousePress;
  QWidget *tlw;
};

//-----------------------------------------------------------------------------
void ctkSizeGripPrivate::init()
{
  Q_Q(ctkSizeGrip);
  dxMax = 0;
  dyMax = 0;
  tlw = 0;
  m_corner = q->isLeftToRight() ? Qt::BottomRightCorner : Qt::BottomLeftCorner;
  gotMousePress = false;

#if !defined(QT_NO_CURSOR) && !defined(Q_WS_MAC)
  q->setCursor(m_corner == Qt::TopLeftCorner || m_corner == Qt::BottomRightCorner
               ? Qt::SizeFDiagCursor : Qt::SizeBDiagCursor);
#endif
  q->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed));
  updateTopLevelWidget();
}

//-----------------------------------------------------------------------------
ctkSizeGrip::ctkSizeGrip(QWidget* parent)
  : QWidget(parent)
  , d_ptr(new ctkSizeGripPrivate(*this))
{
  Q_D(ctkSizeGrip);
  d->init();
}


/*!
    Destroys this size grip.
*/
ctkSizeGrip::~ctkSizeGrip()
{
}

/*!
  \reimp
*/
QSize ctkSizeGrip::sizeHint() const
{
    QStyleOption opt(0);
    opt.init(this);
    return (style()->sizeFromContents(QStyle::CT_SizeGrip, &opt, QSize(13, 13), this).
            expandedTo(QApplication::globalStrut()));
}

/*!
    Paints the resize grip.

    Resize grips are usually rendered as small diagonal textured lines
    in the lower-right corner. The paint event is passed in the \a
    event parameter.
*/
void ctkSizeGrip::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_D(ctkSizeGrip);
    QPainter painter(this);
    QStyleOptionSizeGrip opt;
    opt.init(this);
    opt.corner = d->m_corner;
    style()->drawControl(QStyle::CE_SizeGrip, &opt, &painter, this);
}

/*!
    \fn void ctkSizeGrip::mousePressEvent(QMouseEvent * event)

    Receives the mouse press events for the widget, and primes the
    resize operation. The mouse press event is passed in the \a event
    parameter.
*/
void ctkSizeGrip::mousePressEvent(QMouseEvent * e)
{
    if (e->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(e);
        return;
    }

    Q_D(ctkSizeGrip);
    QWidget *tlw = qt_sizegrip_topLevelWidget(this);
    d->p = e->globalPos();
    d->gotMousePress = true;
    d->r = tlw->geometry();

#ifdef Q_WS_X11
    // Use a native X11 sizegrip for "real" top-level windows if supported.
    if (tlw->isWindow() && X11->isSupportedByWM(ATOM(_NET_WM_MOVERESIZE))
        && !tlw->testAttribute(Qt::WA_DontShowOnScreen) && !hasHeightForWidth(tlw)) {
        XEvent xev;
        xev.xclient.type = ClientMessage;
        xev.xclient.message_type = ATOM(_NET_WM_MOVERESIZE);
        xev.xclient.display = X11->display;
        xev.xclient.window = tlw->winId();
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = e->globalPos().x();
        xev.xclient.data.l[1] = e->globalPos().y();
        if (d->atBottom())
            xev.xclient.data.l[2] = d->atLeft() ? 6 : 4; // bottomleft/bottomright
        else
            xev.xclient.data.l[2] = d->atLeft() ? 0 : 2; // topleft/topright
        xev.xclient.data.l[3] = Button1;
        xev.xclient.data.l[4] = 0;
        XUngrabPointer(X11->display, X11->time);
        XSendEvent(X11->display, QX11Info::appRootWindow(x11Info().screen()), False,
                   SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        return;
    }
#endif // Q_WS_X11
#ifdef Q_WS_WIN
    if (tlw->isWindow() && !tlw->testAttribute(Qt::WA_DontShowOnScreen) && !hasHeightForWidth(tlw)) {
        uint orientation = 0;
        if (d->atBottom())
            orientation = d->atLeft() ? SZ_SIZEBOTTOMLEFT : SZ_SIZEBOTTOMRIGHT;
        else
            orientation = d->atLeft() ? SZ_SIZETOPLEFT : SZ_SIZETOPRIGHT;

        ReleaseCapture();
        PostMessage(tlw->winId(), WM_SYSCOMMAND, orientation, 0);
        return;
    }
#endif // Q_WS_WIN

    // Find available desktop/workspace geometry.
    QRect availableGeometry;
    bool hasVerticalSizeConstraint = true;
    bool hasHorizontalSizeConstraint = true;
    if (tlw->isWindow())
        availableGeometry = QApplication::desktop()->availableGeometry(tlw);
    else {
        const QWidget *tlwParent = tlw->parentWidget();
        // Check if tlw is inside QAbstractScrollArea/QScrollArea.
        // If that's the case tlw->parentWidget() will return the viewport
        // and tlw->parentWidget()->parentWidget() will return the scroll area.
#ifndef QT_NO_SCROLLAREA
        QAbstractScrollArea *scrollArea = qobject_cast<QAbstractScrollArea *>(tlwParent->parentWidget());
        if (scrollArea) {
            hasHorizontalSizeConstraint = scrollArea->horizontalScrollBarPolicy() == Qt::ScrollBarAlwaysOff;
            hasVerticalSizeConstraint = scrollArea->verticalScrollBarPolicy() == Qt::ScrollBarAlwaysOff;
        }
#endif // QT_NO_SCROLLAREA
        availableGeometry = tlwParent->contentsRect();
    }

    // Find frame geometries, title bar height, and decoration sizes.
    const QRect frameGeometry = tlw->frameGeometry();
    const int titleBarHeight = qMax(tlw->geometry().y() - frameGeometry.y(), 0);
    const int bottomDecoration = qMax(frameGeometry.height() - tlw->height() - titleBarHeight, 0);
    const int leftRightDecoration = qMax((frameGeometry.width() - tlw->width()) / 2, 0);

    // Determine dyMax depending on whether the sizegrip is at the bottom
    // of the widget or not.
    if (d->atBottom()) {
        if (hasVerticalSizeConstraint)
            d->dyMax = availableGeometry.bottom() - d->r.bottom() - bottomDecoration;
        else
            d->dyMax = INT_MAX;
    } else {
        if (hasVerticalSizeConstraint)
            d->dyMax = availableGeometry.y() - d->r.y() + titleBarHeight;
        else
            d->dyMax = -INT_MAX;
    }

    // In RTL mode, the size grip is to the left; find dxMax from the desktop/workspace
    // geometry, the size grip geometry and the width of the decoration.
    if (d->atLeft()) {
        if (hasHorizontalSizeConstraint)
            d->dxMax = availableGeometry.x() - d->r.x() + leftRightDecoration;
        else
            d->dxMax = -INT_MAX;
    } else {
        if (hasHorizontalSizeConstraint)
            d->dxMax = availableGeometry.right() - d->r.right() - leftRightDecoration;
        else
            d->dxMax = INT_MAX;
    }
}


/*!
    \fn void ctkSizeGrip::mouseMoveEvent(QMouseEvent * event)
    Resizes the top-level widget containing this widget. The mouse
    move event is passed in the \a event parameter.
*/
void ctkSizeGrip::mouseMoveEvent(QMouseEvent * e)
{
    if (e->buttons() != Qt::LeftButton) {
        QWidget::mouseMoveEvent(e);
        return;
    }

    Q_D(ctkSizeGrip);
    QWidget* tlw = qt_sizegrip_topLevelWidget(this);
    if (!d->gotMousePress || tlw->testAttribute(Qt::WA_WState_ConfigPending))
        return;

#ifdef Q_WS_X11
    if (tlw->isWindow() && X11->isSupportedByWM(ATOM(_NET_WM_MOVERESIZE))
        && tlw->isTopLevel() && !tlw->testAttribute(Qt::WA_DontShowOnScreen) && !hasHeightForWidth(tlw))
        return;
#endif
#ifdef Q_WS_WIN
    if (tlw->isWindow() && GetSystemMenu(tlw->winId(), FALSE) != 0 && internalWinId()
        && !tlw->testAttribute(Qt::WA_DontShowOnScreen) && !hasHeightForWidth(tlw)) {
        MSG msg;
        while(PeekMessage(&msg, winId(), WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
        return;
    }
#endif

    QPoint np(e->globalPos());

    // Don't extend beyond the available geometry; bound to dyMax and dxMax.
    QSize ns;
    if (d->atBottom())
        ns.rheight() = d->r.height() + qMin(np.y() - d->p.y(), d->dyMax);
    else
        ns.rheight() = d->r.height() - qMax(np.y() - d->p.y(), d->dyMax);

    if (d->atLeft())
        ns.rwidth() = d->r.width() - qMax(np.x() - d->p.x(), d->dxMax);
    else
        ns.rwidth() = d->r.width() + qMin(np.x() - d->p.x(), d->dxMax);

    ns = QLayout::closestAcceptableSize(tlw, ns);

    QPoint p;
    QRect nr(p, ns);
    if (d->atBottom()) {
        if (d->atLeft())
            nr.moveTopRight(d->r.topRight());
        else
            nr.moveTopLeft(d->r.topLeft());
    } else {
        if (d->atLeft())
            nr.moveBottomRight(d->r.bottomRight());
        else
            nr.moveBottomLeft(d->r.bottomLeft());
    }

    tlw->setGeometry(nr);
}

/*!
  \reimp
*/
void ctkSizeGrip::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        Q_D(ctkSizeGrip);
        d->gotMousePress = false;
        d->p = QPoint();
    } else {
        QWidget::mouseReleaseEvent(mouseEvent);
    }
}

/*!
  \reimp
*/
void ctkSizeGrip::moveEvent(QMoveEvent * /*moveEvent*/)
{
    Q_D(ctkSizeGrip);
    // We're inside a resize operation; no update necessary.
    if (!d->p.isNull())
        return;

    d->m_corner = d->corner();
#if !defined(QT_NO_CURSOR) && !defined(Q_WS_MAC)
    setCursor(d->m_corner == Qt::TopLeftCorner || d->m_corner == Qt::BottomRightCorner
              ? Qt::SizeFDiagCursor : Qt::SizeBDiagCursor);
#endif
}

/*!
  \reimp
*/
void ctkSizeGrip::showEvent(QShowEvent *showEvent)
{
#ifdef Q_WS_MAC
    d_func()->updateMacSizer(false);
#endif
    QWidget::showEvent(showEvent);
}

/*!
  \reimp
*/
void ctkSizeGrip::hideEvent(QHideEvent *hideEvent)
{
#ifdef Q_WS_MAC
    d_func()->updateMacSizer(true);
#endif
    QWidget::hideEvent(hideEvent);
}


/*!
    \reimp
*/
bool ctkSizeGrip::event(QEvent *event)
{
    return QWidget::event(event);
}

#ifdef Q_WS_WIN
/*! \reimp */
bool ctkSizeGrip::winEvent( MSG *m, long *result )
{
    return QWidget::winEvent(m, result);
}
#endif

QT_END_NAMESPACE

#include "moc_ctkSizeGrip.cpp"

#endif //QT_NO_SIZEGRIP
