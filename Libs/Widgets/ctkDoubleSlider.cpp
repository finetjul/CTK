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

// QT includes
#include <QDebug>
#include <QHBoxLayout>
#include <QHelpEvent>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QToolTip>

// CTK includes
#include "ctkDoubleSlider.h"
#include "ctkValueProxy.h"

// STD includes
#include <limits>

//-----------------------------------------------------------------------------
// ctkSlider

//-----------------------------------------------------------------------------
class ctkSlider: public QSlider
{
public:
  ctkSlider(QWidget* parent);
  using QSlider::initStyleOption;
};

//-----------------------------------------------------------------------------
ctkSlider::ctkSlider(QWidget* parent): QSlider(parent)
{
}

//-----------------------------------------------------------------------------
// ctkDoubleSliderPrivate

//-----------------------------------------------------------------------------
class ctkDoubleSliderPrivate
{
  Q_DECLARE_PUBLIC(ctkDoubleSlider);
protected:
  ctkDoubleSlider* const q_ptr;
public:
  ctkDoubleSliderPrivate(ctkDoubleSlider& object);
  int toInt(double value)const;
  double fromInt(int value)const;
  double safeFromInt(int value)const;
  void init();
  void updateOffset(double value);

  ctkSlider*    Slider;
  QString       HandleToolTip;
  double      Minimum;
  double      Maximum;
  bool        SettingRange;
  // we should have a Offset and SliderPositionOffset (and MinimumOffset?)
  double      Offset;
  double      SingleStep;
  double      PageStep;
  double      Value;
  QWeakPointer<ctkValueProxy> Proxy;
};

// --------------------------------------------------------------------------
ctkDoubleSliderPrivate::ctkDoubleSliderPrivate(ctkDoubleSlider& object)
  :q_ptr(&object)
{
  this->Slider = 0;
  this->Minimum = 0.;
  this->Maximum = 100.;
  this->SettingRange = false;
  this->Offset = 0.;
  this->SingleStep = 1.;
  this->PageStep = 10.;
  this->Value = 0.;
}

// --------------------------------------------------------------------------
void ctkDoubleSliderPrivate::init()
{
  Q_Q(ctkDoubleSlider);
  this->Slider = new ctkSlider(q);
  this->Slider->installEventFilter(q);
  QHBoxLayout* l = new QHBoxLayout(q);
  l->addWidget(this->Slider);
  l->setContentsMargins(0,0,0,0);
  
  this->Minimum = this->Slider->minimum();
  this->Maximum = this->Slider->maximum();
  // this->Slider->singleStep is always 1
  this->SingleStep = this->Slider->singleStep();
  this->PageStep = this->Slider->pageStep();
  this->Value = this->Slider->value();

  q->connect(this->Slider, SIGNAL(valueChanged(int)), q, SLOT(onValueChanged(int)));
  q->connect(this->Slider, SIGNAL(sliderMoved(int)), q, SLOT(onSliderMoved(int)));
  q->connect(this->Slider, SIGNAL(sliderPressed()), q, SIGNAL(sliderPressed()));
  q->connect(this->Slider, SIGNAL(sliderReleased()), q, SIGNAL(sliderReleased()));
  q->connect(this->Slider, SIGNAL(rangeChanged(int,int)),
             q, SLOT(onRangeChanged(int,int)));

  q->setSizePolicy(this->Slider->sizePolicy());
  q->setAttribute(Qt::WA_WState_OwnSizePolicy, false);
}
  
// --------------------------------------------------------------------------
int ctkDoubleSliderPrivate::toInt(double doubleValue)const
{
  double tmp = doubleValue / this->SingleStep;
  static const double minInt = std::numeric_limits<int>::min();
  static const double maxInt = std::numeric_limits<int>::max();
#ifndef QT_NO_DEBUG
  if (tmp < minInt || tmp > maxInt)
    {
    qWarning() << __FUNCTION__ << ": value " << doubleValue
              << " for singleStep " << this->SingleStep
              << " is out of integer bounds !";
    }
#endif
  tmp = qBound(minInt, tmp, maxInt);
  int intValue = qRound(tmp);
  //qDebug() << __FUNCTION__ << doubleValue << tmp << intValue;
  return intValue;
}

// --------------------------------------------------------------------------
double ctkDoubleSliderPrivate::fromInt(int intValue)const
{
  double doubleValue = this->SingleStep * (this->Offset + intValue) ;
  //qDebug() << __FUNCTION__ << intValue << doubleValue;
  return doubleValue;
}

// --------------------------------------------------------------------------
double ctkDoubleSliderPrivate::safeFromInt(int intValue)const
{
  return qBound(this->Minimum, this->fromInt(intValue), this->Maximum);
}

// --------------------------------------------------------------------------
void ctkDoubleSliderPrivate::updateOffset(double value)
{
  this->Offset = (value / this->SingleStep) - this->toInt(value);
}

//-----------------------------------------------------------------------------
// ctkDoubleSlider

// --------------------------------------------------------------------------
ctkDoubleSlider::ctkDoubleSlider(QWidget* _parent) : Superclass(_parent)
  , d_ptr(new ctkDoubleSliderPrivate(*this))
{
  Q_D(ctkDoubleSlider);
  d->init();
}

// --------------------------------------------------------------------------
ctkDoubleSlider::ctkDoubleSlider(Qt::Orientation _orientation, QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkDoubleSliderPrivate(*this))
{
  Q_D(ctkDoubleSlider);
  d->init();
  this->setOrientation(_orientation);
}

// --------------------------------------------------------------------------
ctkDoubleSlider::~ctkDoubleSlider()
{
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setMinimum(double min)
{
  Q_D(ctkDoubleSlider);
  this->setRange(min, qMax(min, d->Maximum));
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setMaximum(double max)
{
  Q_D(ctkDoubleSlider);
  this->setRange(qMin(d->Minimum, max), max);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setRange(double min, double max)
{
  Q_D(ctkDoubleSlider);
  d->Minimum = min;
  d->Maximum = max;
  
  if (d->Minimum >= d->Value)
    {
    d->updateOffset(d->Minimum);
    }
  if (d->Maximum <= d->Value)
    {
    d->updateOffset(d->Maximum);
    }
  d->SettingRange = true;
  d->Slider->setRange(d->toInt(min), d->toInt(max));
  d->SettingRange = false;
  emit this->rangeChanged(d->Minimum, d->Maximum);
  /// In case QSlider::setRange(...) didn't notify the value
  /// has changed.
  this->setValue(this->value());
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::minimum()const
{
  Q_D(const ctkDoubleSlider);
  return d->Minimum;
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::maximum()const
{
  Q_D(const ctkDoubleSlider);
  return d->Maximum;
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::sliderPosition()const
{
  Q_D(const ctkDoubleSlider);
  return d->safeFromInt(d->Slider->sliderPosition());
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setSliderPosition(double newSliderPosition)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setSliderPosition(d->toInt(newSliderPosition));
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::value()const
{
  Q_D(const ctkDoubleSlider);
  double val = d->Value;
  if (d->Proxy)
    {
    val = d->Proxy.data()->valueFromProxyValue(val);
    }
  return val;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setValue(double newValue)
{
  Q_D(ctkDoubleSlider);
  if (d->Proxy)
    {
    newValue = d->Proxy.data()->proxyValueFromValue(newValue);
    }

  newValue = qBound(d->Minimum, newValue, d->Maximum);
  d->updateOffset(newValue);
  int newIntValue = d->toInt(newValue);
  if (newIntValue != d->Slider->value())
    {
    // d->Slider will emit a valueChanged signal that is connected to
    // ctkDoubleSlider::onValueChanged
    d->Slider->setValue(newIntValue);
    }
  else
    {
    double oldValue = d->Value;
    d->Value = newValue;
    // don't emit a valuechanged signal if the new value is quite 
    // similar to the old value.
    if (qAbs(newValue - oldValue) > (d->SingleStep * 0.000000001))
      {
      emit this->valueChanged(this->value());
      }
    }
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::singleStep()const
{
  Q_D(const ctkDoubleSlider);
  return d->SingleStep;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setSingleStep(double newStep)
{
  Q_D(ctkDoubleSlider);
  d->SingleStep = newStep;
  d->updateOffset(d->Value);
  // update the new values of the QSlider
  bool oldBlockSignals = d->Slider->blockSignals(true);
  d->Slider->setRange(d->toInt(d->Minimum), d->toInt(d->Maximum));
  d->Slider->setValue(d->toInt(d->Value));
  d->Slider->setPageStep(d->toInt(d->PageStep));
  d->Slider->blockSignals(oldBlockSignals);
  Q_ASSERT(qFuzzyCompare(d->Value,d->safeFromInt(d->Slider->value())));
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::pageStep()const
{
  Q_D(const ctkDoubleSlider);
  return d->PageStep;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setPageStep(double newStep)
{
  Q_D(ctkDoubleSlider);
  d->PageStep = newStep;
  d->Slider->setPageStep(d->toInt(d->PageStep));
}

// --------------------------------------------------------------------------
double ctkDoubleSlider::tickInterval()const
{
  Q_D(const ctkDoubleSlider);
  // No need to apply Offset
  return d->SingleStep * d->Slider->tickInterval();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTickInterval(double newTickInterval)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setTickInterval(d->toInt(newTickInterval));
}

// --------------------------------------------------------------------------
QSlider::TickPosition ctkDoubleSlider::tickPosition()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->tickPosition();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTickPosition(QSlider::TickPosition newTickPosition)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setTickPosition(newTickPosition);
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::hasTracking()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->hasTracking();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setTracking(bool enable)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setTracking(enable);
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::invertedAppearance()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->invertedAppearance();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setInvertedAppearance(bool invertedAppearance)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setInvertedAppearance(invertedAppearance);
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::invertedControls()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->invertedControls();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setInvertedControls(bool invertedControls)
{
  Q_D(ctkDoubleSlider);
  d->Slider->setInvertedControls(invertedControls);
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::triggerAction( QAbstractSlider::SliderAction action)
{
  Q_D(ctkDoubleSlider);
  d->Slider->triggerAction(action);
}

// --------------------------------------------------------------------------
Qt::Orientation ctkDoubleSlider::orientation()const
{
  Q_D(const ctkDoubleSlider);
  return d->Slider->orientation();
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setOrientation(Qt::Orientation newOrientation)
{
  Q_D(ctkDoubleSlider);
  if (this->orientation() == newOrientation)
    {
    return;
    }
  if (!testAttribute(Qt::WA_WState_OwnSizePolicy))
    {
    QSizePolicy sp = this->sizePolicy();
    sp.transpose();
    this->setSizePolicy(sp);
    this->setAttribute(Qt::WA_WState_OwnSizePolicy, false);
    }
  // d->Slider will take care of calling updateGeometry
  d->Slider->setOrientation(newOrientation);
}

// --------------------------------------------------------------------------
QString ctkDoubleSlider::handleToolTip()const
{
  Q_D(const ctkDoubleSlider);
  return d->HandleToolTip;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::setHandleToolTip(const QString& toolTip)
{
  Q_D(ctkDoubleSlider);
  d->HandleToolTip = toolTip;
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onValueChanged(int newValue)
{
  Q_D(ctkDoubleSlider);
  double doubleNewValue = d->safeFromInt(newValue);
/*
  qDebug() << "onValueChanged: " << newValue << "->"<< d->fromInt(newValue+d->Offset) 
           << " old: " << d->Value << "->" << d->toInt(d->Value) 
           << "offset:" << d->Offset << doubleNewValue;
*/
  if (d->Value == doubleNewValue)
    {
    return;
    }
  d->Value = doubleNewValue;
  emit this->valueChanged(this->value());
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onSliderMoved(int newPosition)
{
  Q_D(const ctkDoubleSlider);
  emit this->sliderMoved(d->safeFromInt(newPosition));
}

// --------------------------------------------------------------------------
void ctkDoubleSlider::onRangeChanged(int min, int max)
{
  Q_D(const ctkDoubleSlider);
  if (!d->SettingRange)
    {
    this->setRange(d->fromInt(min), d->fromInt(max));
    }
}

// --------------------------------------------------------------------------
bool ctkDoubleSlider::eventFilter(QObject* watched, QEvent* event)
{
  Q_D(ctkDoubleSlider);
  if (watched == d->Slider)
    {
    switch(event->type())
      {
      case QEvent::ToolTip:
        {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QStyleOptionSlider opt;
        d->Slider->initStyleOption(&opt);
        QStyle::SubControl hoveredControl =
          d->Slider->style()->hitTestComplexControl(
            QStyle::CC_Slider, &opt, helpEvent->pos(), this);
        if (!d->HandleToolTip.isEmpty() &&
            hoveredControl == QStyle::SC_SliderHandle)
          {
          QToolTip::showText(helpEvent->globalPos(), d->HandleToolTip.arg(this->value()));
          event->accept();
          return true;
          }
        }
      default:
        break;
      }
    }
  return this->Superclass::eventFilter(watched, event);
}

//----------------------------------------------------------------------------
void ctkDoubleSlider::setValueProxy(ctkValueProxy* proxy)
{
  Q_D(ctkDoubleSlider);
  if (d->Proxy.data() == proxy)
    {
    return;
    }

  d->Proxy = proxy;
}

//----------------------------------------------------------------------------
ctkValueProxy* ctkDoubleSlider::valueProxy() const
{
  Q_D(const ctkDoubleSlider);
  return d->Proxy.data();
}

