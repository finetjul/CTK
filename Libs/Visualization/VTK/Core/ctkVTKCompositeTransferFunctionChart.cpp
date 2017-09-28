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

#include "ctkVTKCompositeTransferFunctionChart.h"
#include "ctkVTKDiscretizableColorTransferControlPointsItem.h"
#include "ctkVTKHistogramMarker.h"
#include "ctkVTKScalarsToColorsUtils.h"
#include <vtkAxis.h>
#include <vtkColorTransferFunction.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkCompositeTransferFunctionItem.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkObjectFactory.h>
#include <vtkPen.h>
#include <vtkTable.h>
#include <vtkTransform2D.h>

vtkStandardNewMacro(ctkVTKCompositeTransferFunctionChart)

ctkVTKCompositeTransferFunctionChart::ctkVTKCompositeTransferFunctionChart()
{
  Transform = vtkSmartPointer<vtkTransform2D>::New();

  minMarker = vtkSmartPointer<ctkVTKHistogramMarker>::New();
  maxMarker = vtkSmartPointer<ctkVTKHistogramMarker>::New();

  minDataMarker = vtkSmartPointer<ctkVTKHistogramMarker>::New();
  maxDataMarker = vtkSmartPointer<ctkVTKHistogramMarker>::New();

  ForceAxesToBoundsOn();
  SetAutoSize(true);
  SetAutoAxes(false);
  SetHiddenAxisBorder(0);
  SetLayoutStrategy(vtkChart::AXES_TO_RECT);
  SetRenderEmpty(true);
  ZoomWithMouseWheelOff();
  SetHiddenAxisBorder(10);

  for (int i = 0; i < 4; ++i)
  {
    GetAxis(i)->SetVisible(true);
    GetAxis(i)->SetNumberOfTicks(0);
    GetAxis(i)->SetBehavior(vtkAxis::AUTO);
    GetAxis(i)->SetLabelsVisible(false);
    GetAxis(i)->SetMargins(1, 1);
    GetAxis(i)->SetTitle("");
  }

  GetAxis(vtkAxis::BOTTOM)->SetBehavior(vtkAxis::FIXED);

  compositeHiddenItem = nullptr;
  controlPoints = nullptr;
}

void ctkVTKCompositeTransferFunctionChart::SetColorTransferFunction(vtkDiscretizableColorTransferFunction* function)
{
  if (function == nullptr)
  {
    vtkSmartPointer<vtkDiscretizableColorTransferFunction> emptyCtf =
      vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
    this->SetColorTransferFunction(emptyCtf, 0, 255);
    return;
  }

  this->SetColorTransferFunction(function, function->GetRange()[0], function->GetRange()[1]);
}

void ctkVTKCompositeTransferFunctionChart::SetColorTransferFunction(vtkDiscretizableColorTransferFunction* function,
  double dataRangeMin, double dataRangeMax)
{
  workingFunction = function;
  ClearPlots();

  //Build the histogram chart
  compositeHiddenItem = vtkSmartPointer<vtkCompositeTransferFunctionItem>::New();
  compositeHiddenItem->SetMaskAboveCurve(true);
  compositeHiddenItem->SetOpacity(0);
  compositeHiddenItem->SetInteractive(false);

  if (controlPoints != nullptr)
    controlPoints->RemoveAllObservers();

  controlPoints = vtkSmartPointer<ctkVTKDiscretizableColorTransferControlPointsItem>::New();

  controlPoints->SetUseOpacityPointHandles(false);
  controlPoints->SetEndPointsRemovable(false);
  controlPoints->SetInteractive(false);
  controlPoints->SetEndPointsXMovable(false);

  controlPoints->SetColorTransferFunction(function);
  controlPoints->SetOpacityFunction(function->GetScalarOpacityFunction());
  controlPoints->SetPointsFunction(vtkCompositeControlPointsItem::ColorAndOpacityPointsFunction);

  compositeHiddenItem->SetColorTransferFunction(function);
  compositeHiddenItem->SetOpacityFunction(function->GetScalarOpacityFunction());
  dataRange[0] = dataRangeMin;
  dataRange[1] = dataRangeMax;

  //Add the Min/Max Markers
  minPlot = AddPlot(vtkChart::LINE);
  maxPlot = AddPlot(vtkChart::LINE);
  minLinePlot = AddPlot(vtkChart::LINE);
  maxLinePlot = AddPlot(vtkChart::LINE);

  minPlot->SetInputData(minMarker->getTable(), 0, 1);
  maxPlot->SetInputData(maxMarker->getTable(), 0, 1);
  minLinePlot->SetInputData(minMarker->getTable(), 0, 1);
  maxLinePlot->SetInputData(maxMarker->getTable(), 0, 1);

  minPlot->SetColor(187, 187, 187, 100);
  minPlot->SetWidth(11.0);

  maxPlot->SetColor(187, 187, 187, 100);
  maxPlot->SetWidth(11.0);

  minLinePlot->SetColor(255, 255, 255, 255);
  minLinePlot->SetWidth(1.0);

  maxLinePlot->SetColor(255, 255, 255, 255);
  maxLinePlot->SetWidth(1.0);
  //
  minDataPlot = AddPlot(vtkChart::LINE);
  maxDataPlot = AddPlot(vtkChart::LINE);
  minDataPlot->SetInputData(minDataMarker->getTable(), 0, 1);
  maxDataPlot->SetInputData(maxDataMarker->getTable(), 0, 1);

  minDataPlot->SetColor(0, 0, 0, 0);
  maxDataPlot->SetColor(0, 0, 0, 0);

  currentRange[0] = dataRange[0];
  minMarker->SetPosition(currentRange[0]);

  currentRange[1] = dataRange[1];
  maxMarker->SetPosition(currentRange[1]);

  minDataMarker->SetPosition(dataRange[0]);
  maxDataMarker->SetPosition(dataRange[1]);

  AddPlot(compositeHiddenItem);
  AddPlot(controlPoints);

  minPlot->Modified();
  maxPlot->Modified();
  minDataPlot->Modified();
  maxDataPlot->Modified();

  GetAxis(vtkAxis::BOTTOM)->SetUnscaledMaximum(1);

  GetAxis(vtkAxis::BOTTOM)->SetMinimumLimit(dataRange[0]);
  GetAxis(vtkAxis::BOTTOM)->SetMinimum(currentRange[0]);

  GetAxis(vtkAxis::BOTTOM)->SetMaximumLimit(dataRange[1]);
  GetAxis(vtkAxis::BOTTOM)->SetMaximum(currentRange[1]);

  GetAxis(vtkAxis::BOTTOM)->SetRange(dataRange[0], dataRange[1]);

  //Disable zooming
  this->SetActionToButton(ZOOM, -1);
}

ctkVTKCompositeTransferFunctionChart::~ctkVTKCompositeTransferFunctionChart()
{
}

void ctkVTKCompositeTransferFunctionChart::updateMarkerPosition(const vtkContextMouseEvent& m)
{
  vtkVector2f pos;
  this->Transform->InverseTransformPoints(m.GetScenePos().GetData(), pos.GetData(), 1);

  if (rangeMoving == RangeMoving::MIN)
  {
    double newValue = (double)pos.GetX();
    if (newValue < dataRange[0])
    {
      currentRange[0] = dataRange[0];
    }
    else if (newValue < currentRange[1])
    {
      currentRange[0] = newValue;
    }
    //		currentRange[0] = vtkMath::ClampValue((double) pos.GetX(), dataRange[0], currentRange[1]);
    minMarker->SetPosition(currentRange[0]);
    if (workingFunction)
    {
      ctk::remapColorScale(workingFunction, currentRange[0], currentRange[1]);
    }
    minPlot->Modified();
  }
  else if (rangeMoving == RangeMoving::MAX)
  {
    double newValue = (double)pos.GetX();
    if (newValue > dataRange[1])
    {
      currentRange[1] = dataRange[1];
    }
    else if (newValue > currentRange[0])
    {
      currentRange[1] = newValue;
    }
    //		currentRange[1] = vtkMath::ClampValue((double) pos.GetX(), currentRange[0], dataRange[1]);
    maxMarker->SetPosition(currentRange[1]);
    if (workingFunction)
    {
      ctk::remapColorScale(workingFunction, currentRange[0], currentRange[1]);
    }
    maxPlot->Modified();
  }

  this->Scene->SetDirty(true);

  this->InvokeEvent(vtkCommand::CursorChangedEvent);
}

bool ctkVTKCompositeTransferFunctionChart::MouseMoveEvent(const vtkContextMouseEvent &mouse)
{
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON && rangeMoving != RangeMoving::NONE)
  {
    updateMarkerPosition(mouse);
  }

  return this->Superclass::MouseMoveEvent(mouse);
}

bool ctkVTKCompositeTransferFunctionChart_inRange(double min, double max, double value)
{
  return value >= min && value < max;
}

bool ctkVTKCompositeTransferFunctionChart::MouseButtonPressEvent(const vtkContextMouseEvent& mouse)
{
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON)
  {
    if (rangeMoving == RangeMoving::NONE)
    {
      CalculateUnscaledPlotTransform(compositeHiddenItem->GetXAxis(), compositeHiddenItem->GetYAxis(), this->Transform.Get());

      //Need to compute the position of the cursor toward the marker
      double minCoord[2] = {currentRange[0], 0};
      double pixelMin[2];
      this->Transform->TransformPoints(minCoord, pixelMin, 1);

      double maxCoord[2] = {currentRange[1], 0};
      double pixelMax[2];
      this->Transform->TransformPoints(maxCoord, pixelMax, 1);

      double catchWidth = 5;
      //If min and max get close prefer min over max
      if (ctkVTKCompositeTransferFunctionChart_inRange(pixelMin[0] - catchWidth, pixelMin[0] + catchWidth, mouse.GetPos().GetX()))
      {
        rangeMoving = RangeMoving::MIN;
      }
      else if (ctkVTKCompositeTransferFunctionChart_inRange(pixelMax[0] - catchWidth, pixelMax[0] + catchWidth, mouse.GetPos().GetX()))
      {
        rangeMoving = RangeMoving::MAX;
      }
    }
  }
  return this->Superclass::MouseButtonPressEvent(mouse);
}

bool ctkVTKCompositeTransferFunctionChart::MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse)
{
  rangeMoving = RangeMoving::NONE;
  return this->Superclass::MouseButtonReleaseEvent(mouse);
}

bool ctkVTKCompositeTransferFunctionChart::GetCurrentControlPointColor(double rgb[3])
{
  vtkColorTransferFunction* ctf = this->controlPoints->GetColorTransferFunction();
  if (!ctf)
  {
    return false;
  }

  vtkIdType currentIdx = this->controlPoints->GetCurrentPoint();
  if (currentIdx < 0)
  {
    return false;
  }

  double xrgbms[6];
  ctf->GetNodeValue(currentIdx, xrgbms);
  rgb[0] = xrgbms[1];
  rgb[1] = xrgbms[2];
  rgb[2] = xrgbms[3];

  return true;
}

void ctkVTKCompositeTransferFunctionChart::SetCurrentControlPointColor(const double rgb[3])
{
  vtkColorTransferFunction* ctf = this->controlPoints->GetColorTransferFunction();
  if (!ctf)
  {
    return;
  }

  vtkIdType currentIdx = this->controlPoints->GetCurrentPoint();
  if (currentIdx < 0)
  {
    return;
  }

  double xrgbms[6];
  ctf->GetNodeValue(currentIdx, xrgbms);
  xrgbms[1] = rgb[0];
  xrgbms[2] = rgb[1];
  xrgbms[3] = rgb[2];
  ctf->SetNodeValue(currentIdx, xrgbms);
}

void ctkVTKCompositeTransferFunctionChart::SetCurrentRange(double min, double max)
{
  //check if min < max;
  min = vtkMath::ClampValue(min, dataRange[0], dataRange[1]);
  max = vtkMath::ClampValue(max, dataRange[0], dataRange[1]);
  if (min < max)
  {
    currentRange[0] = min < dataRange[0] ? dataRange[0] : min;
    currentRange[1] = max > dataRange[1] ? dataRange[1] : max;
    minMarker->SetPosition(currentRange[0]);
    maxMarker->SetPosition(currentRange[1]);
  }

  if (workingFunction)
  {
    ctk::remapColorScale(workingFunction, currentRange[0], currentRange[1]);
  }
  minPlot->Modified();
  maxPlot->Modified();

  this->Scene->SetDirty(true);

  this->InvokeEvent(vtkCommand::CursorChangedEvent);
}

void ctkVTKCompositeTransferFunctionChart::CenterRange(double center)
{
  double width = currentRange[1] - currentRange[0];
  double minCenter = dataRange[0] + width / 2.0;
  double maxCenter = dataRange[1] - width / 2.0;

  center = vtkMath::ClampValue(center, minCenter, maxCenter);
  double newMin = center - width / 2;
  double newMax = newMin + width;

  SetCurrentRange(newMin, newMax);
}

vtkCompositeControlPointsItem*
ctkVTKCompositeTransferFunctionChart::GetControlPointsItem()
{
  return controlPoints;
}
