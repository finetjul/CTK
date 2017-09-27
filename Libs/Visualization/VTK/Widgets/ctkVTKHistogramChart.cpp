#include <ctkVTKHistogramChart.h>
#include <vtkAxis.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vtkContext2D.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkObjectFactory.h>
#include <vtkPen.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPiecewiseFunctionItem.h>
#include <vtkPlot.h>
#include <vtkPlotBar.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>
#include <vtkTextProperty.h>
#include <vtkTooltipItem.h>
#include <vtkTransform2D.h>

vtkStandardNewMacro(ctkVTKHistogramChart)

ctkVTKHistogramChart::ctkVTKHistogramChart() {
	this->SetBarWidthFraction(1.0);
	this->ForceAxesToBoundsOn();

  //this->SetAutoSize(true);
  //this->SetAutoAxes(false);
  //this->SetHiddenAxisBorder(0);
	this->SetLayoutStrategy(vtkChart::AXES_TO_RECT);
  //this->SetRenderEmpty(true);
  //this->ZoomWithMouseWheelOff();
	this->SetHiddenAxisBorder(10);

	for (int i = 0; i < 4; ++i) {
		this->GetAxis(i)->SetVisible(true);
		this->GetAxis(i)->SetNumberOfTicks(0);
		this->GetAxis(i)->SetLabelsVisible(false);
		this->GetAxis(i)->SetMargins(1, 1);
		//this->GetAxis(i)->SetBehavior(vtkAxis::AUTO);
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

ctkVTKHistogramChart::~ctkVTKHistogramChart() {
}

void ctkVTKHistogramChart::SetHistogramInputData(vtkTable* table,
		const char* xAxisColumn, const char* yAxisColumn) {
	this->HistogramPlotBar->SetInputData(table, xAxisColumn, yAxisColumn);
  this->SelectColorArray(xAxisColumn);
	RecalculateBounds();
}

void ctkVTKHistogramChart::SetScalarVisibility(bool visible) {
	this->HistogramPlotBar->SetScalarVisibility(visible);
}

void ctkVTKHistogramChart::SetLookupTable(vtkScalarsToColors* lut)
{
	this->HistogramPlotBar->SetLookupTable(lut);
}

void ctkVTKHistogramChart::SelectColorArray(const char* arrayName) {
	this->HistogramPlotBar->SelectColorArray(arrayName);
}
