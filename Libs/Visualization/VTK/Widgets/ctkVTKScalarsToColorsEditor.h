#ifndef __ctkVTKScalarsToColorsEditor_h
#define __ctkVTKScalarsToColorsEditor_h

#include "ctkVisualizationVTKWidgetsExport.h"

#include <vtkAbstractContextItem.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkVector.h>

class vtkAxis;

class vtkColorTransferControlPointsItem;
class vtkColorTransferFunction;
class vtkColorTransferFunctionItem;
class vtkPiecewiseFunction;
class vtkScalarsToColors;
class vtkTable;

class vtkCompositeControlPointsItem;
class vtkCompositeTransferFunctionItem;
class vtkDiscretizableColorTransferFunction;
class vtkCallbackCommand;


class ctkVTKHistogramChart;
class ctkVTKCompositeTransfertFunctionChart;
class ctkVTKScalarsToColorsPreviewChart;

// This class is a chart that combines a histogram from a data set
// a color bar editor, and an opacity editor.
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsEditor: public vtkAbstractContextItem {
public:
  vtkTypeMacro(
    ctkVTKScalarsToColorsEditor,
    vtkAbstractContextItem)
    static ctkVTKScalarsToColorsEditor* New();

  void SetColorTransfertFunction(vtkSmartPointer<vtkScalarsToColors> ctf);
  vtkSmartPointer<vtkScalarsToColors> GetColorTransfertFunction();

  void SetDiscretizableColorTransfertFunction(vtkSmartPointer<vtkDiscretizableColorTransferFunction> dctf);
  void SetHistogramTable(vtkTable* table, const char* xAxisColumn, const char* yAxisColumn);

	// Paint event for the editor.
  virtual bool Paint(vtkContext2D* painter) override;

	// is a currently selected control point, false otherwise.
	bool GetCurrentControlPointColor(double rgb[3]);

	// Set the color of the current color control point.
	void SetCurrentControlPointColor(const double rgb[3]);

	void SetCurrentRange(double min, double max);
	void CenterRange(double center);

protected:
  vtkSmartPointer<ctkVTKCompositeTransfertFunctionChart> overlayChart;
  vtkSmartPointer<ctkVTKScalarsToColorsPreviewChart> previewChart;
  vtkSmartPointer<ctkVTKHistogramChart> histogramChart;

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> colorTransferFunction;

	double dataRangeMin;
	double dataRangeMax;

private:

  ctkVTKScalarsToColorsEditor();
  ~ctkVTKScalarsToColorsEditor() override;

  // Cached geometry of the chart
  vtkVector2i LastSceneSize;

	float Borders[4];

  class EventForwarder;
    EventForwarder* PrivateEventForwarder;
};
#endif // tomvizvtkChartHistogramColorOpacityEditor_h
