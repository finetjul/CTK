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
#include "ctkVTKScalarsToColorsPreviewChart.h"
#include "ctkVTKScalarsToColorsUtils.h"
#include "ctkVTKHistogramChart.h"
#include "ctkVTKScalarsToColorsEditor.h"
#include <vtkAxis.h>
#include <vtkBrush.h>
#include <vtkContextScene.h>
#include <vtkCompositeControlPointsItem.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>
#include <vtkTable.h>
#include <vtkVector.h>

double ctkVTKScalarsToColorsEditor::BACKGROUND_COLOR[3]
  = {49.0 / 256, 54.0 / 256, 59.0 / 256};

class ctkVTKScalarsToColorsEditor::EventForwarder
{
public:
  EventForwarder(ctkVTKScalarsToColorsEditor* parent)
    :Self(parent)
  {}

  ~EventForwarder()
  {}

  void ForwardEvent(vtkObject* vtkNotUsed(object), unsigned long eventId,
    void* vtkNotUsed(data))
  {
    this->Self->InvokeEvent(eventId);
  }

  // Reference to owner of the EventForwarder
  ctkVTKScalarsToColorsEditor* Self;
};

vtkStandardNewMacro(ctkVTKScalarsToColorsEditor)

ctkVTKScalarsToColorsEditor::ctkVTKScalarsToColorsEditor()
{
  this->PrivateEventForwarder = new EventForwarder(this);

	this->Borders[vtkAxis::LEFT] = 8;
	this->Borders[vtkAxis::BOTTOM] = 1;
	this->Borders[vtkAxis::RIGHT] = 8;
	this->Borders[vtkAxis::TOP] = 8;

  this->LastSceneSize = vtkVector2i(0, 0);

  vtkSmartPointer<vtkBrush> b = vtkSmartPointer<vtkBrush>::New();
  b->SetOpacityF(0);

  //Histogram
  histogramChart = vtkSmartPointer<ctkVTKHistogramChart>::New();
  histogramChart->SetScalarVisibility(true);
  AddItem(histogramChart.GetPointer());

  //Preview
	previewChart = vtkSmartPointer<ctkVTKScalarsToColorsPreviewChart>::New();
  AddItem(previewChart.GetPointer());

  //Editor 
  overlayChart = vtkSmartPointer<ctkVTKCompositeTransferFunctionChart>::New();
	overlayChart->SetBackgroundBrush(b);
  AddItem(overlayChart.GetPointer());

  this->SetColorTransfertFunction(colorTransferFunction);


  //------------------------------------------------------------------------------------
  //Add observers
  vtkSmartPointer<vtkCompositeControlPointsItem> controlPoints =
    overlayChart->GetControlPointsItem();

  controlPoints->AddObserver(vtkControlPointsItem::CurrentPointChangedEvent,
    this->PrivateEventForwarder, &EventForwarder::ForwardEvent);
  controlPoints->AddObserver(vtkControlPointsItem::CurrentPointEditEvent,
    this->PrivateEventForwarder, &EventForwarder::ForwardEvent);
}

ctkVTKScalarsToColorsEditor::~ctkVTKScalarsToColorsEditor()
{
  delete this->PrivateEventForwarder;
}


void ctkVTKScalarsToColorsEditor::SetColorTransfertFunction(
  vtkScalarsToColors* ctf)
{
  if(ctf == nullptr)
  {
    this->SetDiscretizableColorTransfertFunction(nullptr);
    return;
  }

  if (ctf->IsA("vtkDiscretizableColorTransferFunction"))
  {
    this->SetDiscretizableColorTransfertFunction(
      vtkDiscretizableColorTransferFunction::SafeDownCast(ctf));
  }
  else if (ctf->IsA("vtkColorTransferFunction"))
  {
    vtkSmartPointer<vtkColorTransferFunction> newCtf =
      vtkColorTransferFunction::SafeDownCast(ctf);

    vtkSmartPointer<vtkDiscretizableColorTransferFunction> dctf =
      vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
    dctf->vtkColorTransferFunction::DeepCopy(newCtf);

    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();
    opacityFunction->AddPoint(0.0, 0.0);
    opacityFunction->AddPoint(255.0, 1.0);
    dctf->SetScalarOpacityFunction(opacityFunction);
    dctf->EnableOpacityMappingOn();

    this->SetDiscretizableColorTransfertFunction(dctf);
  }
}

void ctkVTKScalarsToColorsEditor::SetDiscretizableColorTransfertFunction(
  vtkDiscretizableColorTransferFunction* colorTransfer)
{
  colorTransferFunction = colorTransfer;

  overlayChart->SetColorTransferFunction(colorTransferFunction);

  previewChart->SetColorTransferFunction(colorTransferFunction);

  histogramChart->SetLookupTable(colorTransferFunction);

  vtkSmartPointer<vtkCompositeControlPointsItem> controlPoints =
    overlayChart->GetControlPointsItem();

  controlPoints->AddObserver(vtkControlPointsItem::CurrentPointChangedEvent,
    this->PrivateEventForwarder, &EventForwarder::ForwardEvent);
  controlPoints->AddObserver(vtkControlPointsItem::CurrentPointEditEvent,
    this->PrivateEventForwarder, &EventForwarder::ForwardEvent);
}

vtkSmartPointer<vtkScalarsToColors> ctkVTKScalarsToColorsEditor::GetColorTransfertFunction()
{
  return colorTransferFunction;
}

vtkSmartPointer<vtkDiscretizableColorTransferFunction>
  ctkVTKScalarsToColorsEditor::GetDiscretizableColorTransfertFunction()
{
  return colorTransferFunction;
}

void ctkVTKScalarsToColorsEditor::SetHistogramTable(vtkTable* table, const char* xAxisColumn, const char* yAxisColumn)
{
  histogramChart->SetHistogramInputData(table, xAxisColumn, yAxisColumn);
  histogramChart->SetLookupTable(colorTransferFunction);
}

void ctkVTKScalarsToColorsEditor::SetCurrentRange(double min, double max)
{
	overlayChart->SetCurrentRange(min, max);
}

void ctkVTKScalarsToColorsEditor::CenterRange(double center)
{
	overlayChart->CenterRange(center);
}

bool ctkVTKScalarsToColorsEditor::GetCurrentControlPointColor(double rgb[3])
{
	return overlayChart->GetCurrentControlPointColor(rgb);
}

void ctkVTKScalarsToColorsEditor::SetCurrentControlPointColor(
    const double rgb[3])
{
	overlayChart->SetCurrentControlPointColor(rgb);
}

void ctkVTKScalarsToColorsEditor::SetGlobalOpacity(int opacity)
{
  ctk::setTransparency(colorTransferFunction, ((double)opacity) / 100.0);
}

void ctkVTKScalarsToColorsEditor::InvertColorTransferFunction()
{
  ctk::reverseColorMap(colorTransferFunction);
}


bool ctkVTKScalarsToColorsEditor::Paint(vtkContext2D* painter)
{
  vtkContextScene* scene = this->GetScene();

  int sceneWidth = scene->GetSceneWidth();
  int sceneHeight = scene->GetSceneHeight();

  if(sceneWidth != this->LastSceneSize.GetX()
    || sceneHeight != this->LastSceneSize.GetY())
  {
    // Update the geometry size cache
    this->LastSceneSize.Set(sceneWidth, sceneHeight);

    // Upper chart (histogram) expands, lower chart (color bar) is fixed height.
    float x = this->Borders[vtkAxis::LEFT];
    float y = this->Borders[vtkAxis::BOTTOM];

    // Add the width of the left axis to x to make room for y labels
    overlayChart->GetAxis(vtkAxis::LEFT)->Update();
    float leftAxisWidth =
      this->overlayChart->GetAxis(vtkAxis::LEFT)->GetBoundingRect(
        painter).GetWidth();

    x += leftAxisWidth;

    float colorBarThickness = 20;
    float plotWidth = sceneWidth - x - this->Borders[vtkAxis::RIGHT];

    vtkRectf colorTransferFunctionChartSize(x, y, plotWidth,
      colorBarThickness);
    previewChart->SetSize(colorTransferFunctionChartSize);
    previewChart->RecalculateBounds();
    float bottomAxisHeight =
      this->overlayChart->GetAxis(vtkAxis::BOTTOM)->GetBoundingRect(
        painter).GetHeight();

    float verticalMargin = bottomAxisHeight;
    y += colorBarThickness + verticalMargin + 5;
    vtkRectf histogramChartSize(x, y, plotWidth,
      sceneHeight - y - this->Borders[vtkAxis::TOP]);

    overlayChart->SetSize(histogramChartSize);
    overlayChart->RecalculateBounds();

    histogramChart->SetSize(histogramChartSize);
    histogramChart->RecalculateBounds();
  }

  return this->Superclass::Paint(painter);
}
