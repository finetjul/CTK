/*
* ctkVTKCompositeTransfertFunctionChart.h
*
*  Created on: 25 juil. 2017
*      Author: a
*/

#ifndef __ctkVTKCompositeTransfertFunctionChart_h
#define __ctkVTKCompositeTransfertFunctionChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkCommand.h>

class ctkVTKDiscretizableColorTransferControlPointsItem;
class ctkVTKHistogramMarker;

class vtkCompositeControlPointsItem;
class vtkCompositeTransferFunctionItem;
class vtkTransform2D;
class vtkDiscretizableColorTransferFunction;

class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKCompositeTransfertFunctionChart
  : public vtkChartXY
{
public:
  vtkTypeMacro(ctkVTKCompositeTransfertFunctionChart, vtkChartXY)
  static ctkVTKCompositeTransfertFunctionChart* New();

  virtual bool MouseMoveEvent(const vtkContextMouseEvent &mouse) override;
  virtual bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse) override;
  virtual bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse) override;

  void SetColorTransferFunction(vtkDiscretizableColorTransferFunction* function);
  void SetColorTransferFunction(vtkDiscretizableColorTransferFunction* function, double dataRangeMin, double dataRangeMax);

  // is a currently selected control point, false otherwise.
  bool GetCurrentControlPointColor(double rgb[3]);

  // Set the color of the current color control point.
  void SetCurrentControlPointColor(const double rgb[3]);

  /**
  *  Set the current range
  *
  *  it will clamp values into the current dataRange
  */
  void SetCurrentRange(double min, double max);

  /**
  * Center the current position to the given point
  */
  void CenterRange(double center);

  vtkCompositeControlPointsItem* GetControlPointsItem();

private:
  void updateMarkerPosition(const vtkContextMouseEvent& m);
protected:

  vtkSmartPointer<vtkCompositeTransferFunctionItem> compositeHiddenItem;
  vtkSmartPointer<ctkVTKDiscretizableColorTransferControlPointsItem> controlPoints;

  vtkNew<vtkTransform2D> Transform;
  vtkSmartPointer<vtkDiscretizableColorTransferFunction> workingFunction;

  vtkPlot *minPlot;
  vtkPlot *maxPlot;
  vtkPlot *minLinePlot;
  vtkPlot *maxLinePlot;
  vtkPlot *minDataPlot;
  vtkPlot *maxDataPlot;
  vtkSmartPointer<ctkVTKHistogramMarker> minMarker;
  vtkSmartPointer<ctkVTKHistogramMarker> maxMarker;
  vtkSmartPointer<ctkVTKHistogramMarker> minDataMarker;
  vtkSmartPointer<ctkVTKHistogramMarker> maxDataMarker;

  enum class RangeMoving {
    NONE, MIN, MAX
  } rangeMoving = RangeMoving::NONE;
  double dataRange[2];
  double currentRange[2];

private:
  ctkVTKCompositeTransfertFunctionChart();
  virtual ~ctkVTKCompositeTransfertFunctionChart();
};

#endif
