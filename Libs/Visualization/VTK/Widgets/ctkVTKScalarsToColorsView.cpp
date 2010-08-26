/*=========================================================================

  Library:   CTK

  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// Qt includes

// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsView.h"

// VTK includes
#include <vtkAxis.h>
#include <vtkChartXY.h>
#include <vtkColorTransferFunction.h>
#include <vtkColorTransferFunctionItem.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkLookupTable.h>
#include <vtkLookupTableItem.h>
#include <vtkPiecewiseControlPointsItem.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseFunctionItem.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.widgets.ctkVTKScalarsToColorsView");
//----------------------------------------------------------------------------

class ctkVTKScalarsToColorsViewPrivate: public ctkPrivate<ctkVTKScalarsToColorsView>
{
public:
  ctkVTKScalarsToColorsViewPrivate();
  void init();
  void updateChart();
};

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsViewPrivate methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsViewPrivate::ctkVTKScalarsToColorsViewPrivate()
{
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsViewPrivate::init()
{
  CTK_P(ctkVTKScalarsToColorsView);
  vtkChartXY* chart = p->chart();
  chart->SetAutoAxes(false);
  chart->SetHiddenAxisBorder(0);
  chart->GetAxis(0)->SetVisible(false);
  chart->GetAxis(1)->SetVisible(false);
  chart->GetAxis(2)->SetVisible(false);
  chart->GetAxis(3)->SetVisible(false);
}

// ----------------------------------------------------------------------------
void ctkVTKScalarsToColorsViewPrivate::updateChart()
{

}

// ----------------------------------------------------------------------------
// ctkVTKScalarsToColorsView methods

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsView::ctkVTKScalarsToColorsView(QWidget* parentWidget)
  :ctkVTKChartView(parentWidget)
{
  CTK_INIT_PRIVATE(ctkVTKScalarsToColorsView);
  CTK_D(ctkVTKScalarsToColorsView);
  d->init();
}

// ----------------------------------------------------------------------------
ctkVTKScalarsToColorsView::~ctkVTKScalarsToColorsView()
{
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addLookupTable(vtkLookupTable* lut)
{
  CTK_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkLookupTableItem> item =
    vtkSmartPointer<vtkLookupTableItem>::New();
  item->SetLookupTable(lut);
  this->addPlot(item);
  d->updateChart();
  return item;
}
// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addColorTransferFunction(vtkColorTransferFunction* colorTF)
{
  CTK_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkColorTransferFunctionItem> item =
    vtkSmartPointer<vtkColorTransferFunctionItem>::New();
  item->SetColorTransferFunction(colorTF);
  this->addPlot(item);
  d->updateChart();
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addOpacityFunction(vtkPiecewiseFunction* opacityTF)
{
  CTK_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkPiecewiseFunctionItem> item =
    vtkSmartPointer<vtkPiecewiseFunctionItem>::New();
  item->SetPiecewiseFunction(opacityTF);
  QColor defaultColor = this->palette().highlight().color();
  item->SetColor(defaultColor.redF(), defaultColor.greenF(), defaultColor.blueF());
  item->SetMaskAboveCurve(true);
  this->addPlot(item);
  vtkSmartPointer<vtkPiecewiseControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkPiecewiseControlPointsItem>::New();
  controlPointsItem->SetPiecewiseFunction(opacityTF);
  this->addPlot(controlPointsItem);
  d->updateChart();
  return item;
}

// ----------------------------------------------------------------------------
vtkPlot* ctkVTKScalarsToColorsView::addCompositeFunction(vtkColorTransferFunction* colorTF, vtkPiecewiseFunction* opacityTF)
{
  CTK_D(ctkVTKScalarsToColorsView);
  vtkSmartPointer<vtkCompositeTransferFunctionItem> item =
    vtkSmartPointer<vtkCompositeTransferFunctionItem>::New();
  item->SetColorTransferFunction(colorTF);
  item->SetOpacityFunction(opacityTF);
  item->SetMaskAboveCurve(true);
  this->addPlot(item);
  vtkSmartPointer<vtkPiecewiseControlPointsItem> controlPointsItem =
    vtkSmartPointer<vtkPiecewiseControlPointsItem>::New();
  controlPointsItem->SetPiecewiseFunction(opacityTF);
  this->addPlot(controlPointsItem);

  d->updateChart();
  return item;
}
