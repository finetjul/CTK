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
#include <QApplication>
#include <QDebug>
#include <QPainter>
#include <QStyledItemDelegate>

// CTK includes
#include "ctkVTKScalarsToColorsComboBox.h"
#include "ctkVTKScalarsToColorsUtils.h"

// VTK includes
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkSmartPointer.h>
#include <vtkScalarsToColors.h>

//Delegate for combo box icons painting
struct Delegate : public QStyledItemDelegate
{
  static const int ICON_WIDTH = 120;
  static const int ICON_HEIGHT = 20;
  Delegate(QObject* parent = nullptr)
    : QStyledItemDelegate(parent)
  {
  }

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    return QSize(ICON_WIDTH, ICON_HEIGHT);
  }
};

//-----------------------------------------------------------------------------
class ctkVTKScalarsToColorsComboBoxPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKScalarsToColorsComboBox);
protected:
  ctkVTKScalarsToColorsComboBox* const q_ptr;
public:
  ctkVTKScalarsToColorsComboBoxPrivate(ctkVTKScalarsToColorsComboBox& object);
  void init();
  
  vtkScalarsToColors* scalarsToColorsFromIndex(int index);

};

// --------------------------------------------------------------------------
// ctkVTKScalarsToColorsComboBoxPrivate methods

// --------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBoxPrivate::ctkVTKScalarsToColorsComboBoxPrivate(ctkVTKScalarsToColorsComboBox& object)
  :q_ptr(&object)
{
}

// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBoxPrivate::init()
{
  Q_Q(ctkVTKScalarsToColorsComboBox);

  // Set delegate for icon painting
  QSize iconSize = QSize(Delegate::ICON_WIDTH, Delegate::ICON_HEIGHT);
  q->setIconSize(iconSize);
  q->setItemDelegate(new Delegate(q));
  q->setMaximumHeight(350);

  // Add default raw
  q->addItem("None");
  q->setDefaultText("Select color transfer function ...");
  q->setCurrentIndex(-1);//Start with default

  // Connect signals and slots
  QObject::connect(q, SIGNAL(currentIndexChanged(int)),
    q, SLOT(onCurrentIndexChanged(int)));

}

// --------------------------------------------------------------------------
vtkScalarsToColors* ctkVTKScalarsToColorsComboBoxPrivate::scalarsToColorsFromIndex(int index)
{
  Q_Q(ctkVTKScalarsToColorsComboBox);

  // Index corresponds to Reset transfer function
  if(index == 0)
  {
    // Set default text
    q->setCurrentIndex(-1);
    return nullptr;
  }

  vtkScalarsToColors* ctf =
     reinterpret_cast<vtkScalarsToColors*>( q->itemData(index).value<void*>());

  return ctf;
}

// --------------------------------------------------------------------------
// ctkVTKScalarsToColorsComboBox methods

// --------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBox::ctkVTKScalarsToColorsComboBox(QWidget* _parent)
  : Superclass(_parent)
  , d_ptr(new ctkVTKScalarsToColorsComboBoxPrivate(*this))
{
  Q_D(ctkVTKScalarsToColorsComboBox);
  d->init();
  
}

// --------------------------------------------------------------------------
ctkVTKScalarsToColorsComboBox::~ctkVTKScalarsToColorsComboBox()
{
}

void ctkVTKScalarsToColorsComboBox::addScalarsToColors(const char* name, vtkSmartPointer<vtkScalarsToColors> ctf)
{
  QImage img = ctk::scalarsToColorsImage(ctf, this->iconSize());

  this->addItem(QPixmap::fromImage(img), name,
    QVariant::fromValue<void*>(ctf.Get()));
}


// --------------------------------------------------------------------------
void ctkVTKScalarsToColorsComboBox::onCurrentIndexChanged(int index)
{
  Q_D(ctkVTKScalarsToColorsComboBox);
  emit currentScalarsToColorsChanged(d->scalarsToColorsFromIndex(index));
}
