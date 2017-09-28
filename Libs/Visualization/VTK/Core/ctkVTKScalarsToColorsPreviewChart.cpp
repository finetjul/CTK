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

#include "ctkVTKScalarsToColorsPreviewChart.h"

#include <vtkAxis.h>
#include <vtkColorTransferFunctionItem.h>
#include <vtkColorTransferFunction.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(ctkVTKScalarsToColorsPreviewChart)

ctkVTKScalarsToColorsPreviewChart::ctkVTKScalarsToColorsPreviewChart()
{
	ForceAxesToBoundsOn();
	SetAutoSize(true);
	SetAutoAxes(true);
	SetHiddenAxisBorder(8);
	SetRenderEmpty(true);
	SetLayoutStrategy(vtkChart::AXES_TO_RECT);
	ZoomWithMouseWheelOff();

	for (int i = 0; i < 4; ++i) {
		GetAxis(i)->SetVisible(true);
		GetAxis(i)->SetNumberOfTicks(0);
		GetAxis(i)->SetBehavior(vtkAxis::AUTO);
		GetAxis(i)->SetLabelsVisible(false);
		GetAxis(i)->SetMargins(1, 1);
		GetAxis(i)->SetTitle("");
	}

	SetInteractive(false);
}

void ctkVTKScalarsToColorsPreviewChart::SetColorTransferFunction(vtkColorTransferFunction* function)
{
  ClearPlots();

  vtkSmartPointer<vtkColorTransferFunctionItem> compositeVisibleItem =
    vtkSmartPointer<vtkColorTransferFunctionItem>::New();
  compositeVisibleItem->SetMaskAboveCurve(false);
  compositeVisibleItem->SetInteractive(false);
  compositeVisibleItem->SetOpacity(1);
  compositeVisibleItem->SelectableOff();
  compositeVisibleItem->SetColorTransferFunction(function);
  AddPlot(compositeVisibleItem);

  if (function == nullptr)
  {
    vtkSmartPointer<vtkColorTransferFunction> ctf =
      vtkSmartPointer<vtkColorTransferFunction>::New();
    compositeVisibleItem->SetColorTransferFunction(ctf);
    AddPlot(compositeVisibleItem);
    return;
  }

  compositeVisibleItem->SetColorTransferFunction(function);
  AddPlot(compositeVisibleItem);
}

ctkVTKScalarsToColorsPreviewChart::~ctkVTKScalarsToColorsPreviewChart()
{
}