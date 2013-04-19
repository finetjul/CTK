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

// CTK includes
#include "ctkVTKPropertyWidget.h"
#include "ui_ctkVTKPropertyWidget.h"

// VTK includes
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

//-----------------------------------------------------------------------------
class ctkVTKPropertyWidgetPrivate: public Ui_ctkVTKPropertyWidget
{
  Q_DECLARE_PUBLIC(ctkVTKPropertyWidget);
protected:
  ctkVTKPropertyWidget* const q_ptr;
public:
  ctkVTKPropertyWidgetPrivate(ctkVTKPropertyWidget& object);
  vtkSmartPointer<vtkProperty> Property;
};

//-----------------------------------------------------------------------------
ctkVTKPropertyWidgetPrivate::ctkVTKPropertyWidgetPrivate(ctkVTKPropertyWidget& object)
  :q_ptr(&object)
{
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::~ctkVTKPropertyWidget()
{
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::ctkVTKPropertyWidget(QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKPropertyWidgetPrivate(*this))
{
  Q_D(ctkVTKPropertyWidget);

  d->setupUi(this);

  connect(d->RepresentationComboBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(setRepresentation(int)));
  connect(d->PointSizeSliderWidget, SIGNAL(valueChanged(double)),
          this, SLOT(setPointSize(double)));
  connect(d->LineWidthSliderWidget, SIGNAL(valueChanged(double)),
          this, SLOT(setLineWidth(double)));
  connect(d->FrontfaceCullingCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(setFrontfaceCulling(bool)));
  connect(d->BackfaceCullingCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(setBackfaceCulling(bool)));

  connect(d->ColorPickerButton, SIGNAL(colorChanged(QColor)),
          this, SLOT(setColor(QColor)));
  connect(d->OpacitySliderWidget, SIGNAL(valueChanged(double)),
          this, SLOT(setOpacity(double)));
  connect(d->EdgeVisibilityCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(setEdgeVisibility(bool)));
  connect(d->EdgeColorPickerButton, SIGNAL(colorChanged(QColor)),
          this, SLOT(setEdgeColor(QColor)));

  connect(d->LightingCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(setLighting(bool)));
  connect(d->InterpolationComboBox, SIGNAL(currentIndexChanged(int)),
          this, SLOT(setInterpolation(int)));
  connect(d->ShadingCheckBox, SIGNAL(toggled(bool)),
          this, SLOT(setShading(bool)));

  this->updateWidgetFromProperty();
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::ctkVTKPropertyWidget(vtkProperty* property, QWidget* parentWidget)
  : Superclass(parentWidget)
  , d_ptr(new ctkVTKPropertyWidgetPrivate(*this))
{
  this->setProperty(property);
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget::setProperty(vtkProperty* property)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() == property)
    {
    return;
    }
  qvtkReconnect(d->Property, property, vtkCommand::ModifiedEvent,
                this, SLOT(updateWidgetFromProperty()));
  d->Property = property;
  d->MaterialPropertyWidget->setProperty(property);
  this->updateWidgetFromProperty();
}

//-----------------------------------------------------------------------------
vtkProperty* ctkVTKPropertyWidget::property()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->Property.GetPointer();
}

//-----------------------------------------------------------------------------
ctkVTKPropertyWidget::GroupsState ctkVTKPropertyWidget::groupsState()const
{
  Q_D(const ctkVTKPropertyWidget);
  ctkVTKPropertyWidget::GroupsState state = 0;
  ctkVTKPropertyWidget* constThis = const_cast<ctkVTKPropertyWidget*>(this);
  if (d->RepresentationCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::RepresentationVisible;
    }
  if (d->RepresentationCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::RepresentationCollapsed;
    }
  if (d->ColorCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::ColorVisible;
    }
  if (d->ColorCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::ColorCollapsed;
    }
  if (d->LightingCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::LightingVisible;
    }
  if (d->LightingCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::LightingCollapsed;
    }
  if (d->MaterialCollapsibleGroupBox->isVisibleTo(constThis) )
    {
    state |= ctkVTKPropertyWidget::MaterialVisible;
    }
  if (d->MaterialCollapsibleGroupBox->collapsed())
    {
    state |= ctkVTKPropertyWidget::MaterialCollapsed;
    }
  return state;
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget::setGroupsState(ctkVTKPropertyWidget::GroupsState state)
{
  Q_D(ctkVTKPropertyWidget);
  d->RepresentationCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::RepresentationVisible);
  d->RepresentationCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::RepresentationCollapsed);
  d->ColorCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::ColorVisible);
  d->ColorCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::ColorCollapsed);
  d->LightingCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::LightingVisible);
  d->LightingCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::LightingCollapsed);
  d->MaterialCollapsibleGroupBox->setVisible(
    state & ctkVTKPropertyWidget::MaterialVisible);
  d->MaterialCollapsibleGroupBox->setCollapsed(
    state & ctkVTKPropertyWidget::MaterialCollapsed);
}

//-----------------------------------------------------------------------------
ctkColorPickerButton::ColorDialogOptions ctkVTKPropertyWidget
::colorDialogOptions()const
{
  Q_D(const ctkVTKPropertyWidget);
  return d->ColorPickerButton->dialogOptions();
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget
::setColorDialogOptions(ctkColorPickerButton::ColorDialogOptions options)
{
  Q_D(ctkVTKPropertyWidget);
  d->ColorPickerButton->setDialogOptions(options);
  d->EdgeColorPickerButton->setDialogOptions(options);
}

//-----------------------------------------------------------------------------
void ctkVTKPropertyWidget::updateWidgetFromProperty()
{
  Q_D(ctkVTKPropertyWidget);
  this->setEnabled(d->Property.GetPointer() != 0);
  if (d->Property.GetPointer() == 0)
    {
    return;
    }
  // Warning: Valid as long as the representation matches the combobox indexes.
  d->RepresentationComboBox->setCurrentIndex( d->Property->GetRepresentation() );
  d->PointSizeSliderWidget->setValue( d->Property->GetPointSize() );
  d->LineWidthSliderWidget->setValue( d->Property->GetLineWidth() );
  d->FrontfaceCullingCheckBox->setChecked( d->Property->GetFrontfaceCulling() );
  d->BackfaceCullingCheckBox->setChecked( d->Property->GetBackfaceCulling() );
  double* c = d->Property->GetColor();
  d->ColorPickerButton->setColor(
    QColor::fromRgbF(qMin(c[0],1.), qMin(c[1], 1.), qMin(c[2],1.)));
  d->OpacitySliderWidget->setValue( d->Property->GetOpacity() );
  d->EdgeVisibilityCheckBox->setChecked( d->Property->GetEdgeVisibility() );
  double* ec = d->Property->GetEdgeColor();
  d->EdgeColorPickerButton->setColor(
    QColor::fromRgbF(qMin(ec[0],1.), qMin(ec[1], 1.), qMin(ec[2],1.)));
  d->LightingCheckBox->setChecked( d->Property->GetLighting() );
  d->InterpolationComboBox->setCurrentIndex( d->Property->GetInterpolation() );
  d->ShadingCheckBox->setChecked( d->Property->GetShading() );
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setRepresentation(int newRepresentation)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetRepresentation(newRepresentation);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setPointSize(double newPointSize)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetPointSize(newPointSize);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setLineWidth(double newLineWidth)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetLineWidth(newLineWidth);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setFrontfaceCulling(bool newFrontfaceCulling)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetFrontfaceCulling(newFrontfaceCulling);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setBackfaceCulling(bool newBackfaceCulling)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetBackfaceCulling(newBackfaceCulling);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setColor(const QColor& newColor)
{
  Q_D(ctkVTKPropertyWidget);
  d->MaterialPropertyWidget->setColor(newColor);
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setOpacity(double newOpacity)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetOpacity(newOpacity);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setEdgeVisibility(bool newEdgeVisibility)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetEdgeVisibility(newEdgeVisibility);
    }
}


// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setEdgeColor(const QColor& newColor)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetEdgeColor(
      newColor.redF(), newColor.greenF(), newColor.blueF());
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setLighting(bool newLighting)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetLighting(newLighting);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setInterpolation(int newInterpolation)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetInterpolation(newInterpolation);
    }
}

// --------------------------------------------------------------------------
void ctkVTKPropertyWidget::setShading(bool newShading)
{
  Q_D(ctkVTKPropertyWidget);
  if (d->Property.GetPointer() != 0)
    {
    d->Property->SetShading(newShading);
    }
}
