/*
 * ctkVTKScalarsToColorsPreviewChart.cxx
 *
 *  Created on: 25 juil. 2017
 *      Author: a
 */

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

void ctkVTKScalarsToColorsPreviewChart::SetColorTransferFunction(vtkColorTransferFunction* function, double minDataRange, double maxDataRange)
{
	ClearPlots();

	vtkSmartPointer<vtkColorTransferFunctionItem> compositeVisibleItem = vtkSmartPointer<vtkColorTransferFunctionItem>::New();
	compositeVisibleItem->SetMaskAboveCurve(false);
	compositeVisibleItem->SetInteractive(false);
	compositeVisibleItem->SetOpacity(1);
	compositeVisibleItem->SelectableOff();
	compositeVisibleItem->SetColorTransferFunction(function);
	AddPlot(compositeVisibleItem);
}

void ctkVTKScalarsToColorsPreviewChart::SetColorTransferFunction(vtkSmartPointer<vtkColorTransferFunction> function)
{
  this->SetColorTransferFunction(function, function->GetRange()[0], function->GetRange()[1]);
}

ctkVTKScalarsToColorsPreviewChart::~ctkVTKScalarsToColorsPreviewChart()
{
}
