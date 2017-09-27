#ifndef muratvtkChartHistogram_h
#define muratvtkChartHistogram_h

#include <vtkChartXY.h>

#include <vtkNew.h>
#include <vtkTransform2D.h>

class vtkContextMouseEvent;

class vtkPiecewiseFunction;
class vtkPiecewiseFunctionItem;
class vtkPlotBar;
class vtkScalarsToColors;
class vtkTable;

class vtkCallbackCommand;


class ctkVTKHistogramChart: public vtkChartXY {
public:
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

#endif // tomvizvtkChartHistogram_h
