/*
* ctkVTKCompositeTransfertFunctionChart.h
*
*  Created on: 25 juil. 2017
*      Author: a
*/

#ifndef __ctkVTKCompositeTransfertFunctionChart_h
#define __ctkVTKCompositeTransfertFunctionChart_h

#include <vtkChartXY.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkCommand.h>

class vtkCompositeTransferFunctionItem;
class vtkTransform2D;
class vtkDiscretizableColorTransferFunction;
class vtkCompositeControlPointsItem;

class ctkVTKHistogramMarker;
//class vtkMuratCustomCompositeControlPointsItem;

class ctkVTKCompositeTransfertFunctionChart : public vtkChartXY {
public:
  static ctkVTKCompositeTransfertFunctionChart* New();

  bool MouseDoubleClickEvent(const vtkContextMouseEvent& mouse) override;

  virtual bool MouseMoveEvent(const vtkContextMouseEvent &mouse) override;
  virtual bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse)
    override;
  virtual bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse)
    override;

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

  vtkSmartPointer<vtkCompositeControlPointsItem> GetControlPointsItem()
  {
    return controlPoints;
  }

private:
  void updateMarkerPosition(const vtkContextMouseEvent& m);
protected:

  vtkSmartPointer<vtkCompositeTransferFunctionItem> compositeHiddenItem;
  vtkSmartPointer<vtkCompositeControlPointsItem> controlPoints;

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
