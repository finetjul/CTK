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

#include <ctkVTKHistogramChart.h>
#include <vtkAxis.h>
#include <vtkObjectFactory.h>
#include <vtkPen.h>
#include <vtkPlotBar.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>
#include <vtkTextProperty.h>
#include <vtkTooltipItem.h>

vtkStandardNewMacro(ctkVTKHistogramChart)

ctkVTKHistogramChart::ctkVTKHistogramChart()
{
	this->SetBarWidthFraction(1.0);
	this->ForceAxesToBoundsOn();

	this->SetLayoutStrategy(vtkChart::AXES_TO_RECT);
	this->SetHiddenAxisBorder(10);

	for (int i = 0; i < 4; ++i) {
		this->GetAxis(i)->SetVisible(true);
		this->GetAxis(i)->SetNumberOfTicks(0);
		this->GetAxis(i)->SetLabelsVisible(false);
		this->GetAxis(i)->SetMargins(1, 1);
		this->GetAxis(i)->SetTitle("");
	}

	int fontSize = 12;
	this->GetAxis(vtkAxis::LEFT)->GetLabelProperties()->SetFontSize(fontSize);
	this->GetAxis(vtkAxis::BOTTOM)->GetLabelProperties()->SetFontSize(fontSize);
	this->GetAxis(vtkAxis::RIGHT)->GetLabelProperties()->SetFontSize(fontSize);

	this->GetTooltip()->GetTextProperties()->SetFontSize(fontSize);

	// Set up the plot bar
	this->AddPlot(this->HistogramPlotBar.Get());

	this->HistogramPlotBar->SetColor(0, 0, 255, 0);
	this->HistogramPlotBar->GetPen()->SetLineType(vtkPen::NO_PEN);
	this->HistogramPlotBar->SetSelectable(false);
	this->HistogramPlotBar->SetInteractive(false);

	this->HistogramPlotBar->SetOpacity(0.15);
}

ctkVTKHistogramChart::~ctkVTKHistogramChart()
{
}

void ctkVTKHistogramChart::SetHistogramInputData(vtkTable* table,
    const char* xAxisColumn, const char* yAxisColumn)
{
	this->HistogramPlotBar->SetInputData(table, xAxisColumn, yAxisColumn);
  this->SelectColorArray(xAxisColumn);
	RecalculateBounds();
}

void ctkVTKHistogramChart::SetScalarVisibility(bool visible)
{
	this->HistogramPlotBar->SetScalarVisibility(visible);
}

void ctkVTKHistogramChart::SetLookupTable(vtkScalarsToColors* lut)
{
	this->HistogramPlotBar->SetLookupTable(lut);
}

void ctkVTKHistogramChart::SelectColorArray(const char* arrayName)
{
	this->HistogramPlotBar->SelectColorArray(arrayName);
}
