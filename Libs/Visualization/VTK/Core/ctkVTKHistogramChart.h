#ifndef __ctkVTKHistogramChart_h
#define __ctkVTKHistogramChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkNew.h>
#include <vtkTransform2D.h>

class vtkPlotBar;
class vtkScalarsToColors;
class vtkTable;

class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKHistogramChart: public vtkChartXY {
public:
  vtkTypeMacro(ctkVTKHistogramChart, vtkChartXY)
	static ctkVTKHistogramChart* New();

	// Set input for histogram
	virtual void SetHistogramInputData(vtkTable* table, const char* xAxisColumn,
			const char* yAxisColumn);

	// Set scalar visibility in the histogram plot bar
	virtual void SetScalarVisibility(bool visible);

	// Set lookup table
	virtual void SetLookupTable(vtkScalarsToColors* lut);

	// Set the color array name
	virtual void SelectColorArray(const char* arrayName);

protected:
	vtkNew<vtkPlotBar> HistogramPlotBar;

private:
	ctkVTKHistogramChart();
	virtual ~ctkVTKHistogramChart();
};

#endif // __ctkVTKHistogramChart_h
