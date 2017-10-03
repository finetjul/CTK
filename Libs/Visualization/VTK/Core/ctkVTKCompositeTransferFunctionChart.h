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

#ifndef __ctkVTKCompositeTransferFunctionChart_h
#define __ctkVTKCompositeTransferFunctionChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkSmartPointer.h>

class ctkVTKDiscretizableColorTransferControlPointsItem;
class ctkVTKHistogramMarker;
class vtkCompositeControlPointsItem;
class vtkCompositeTransferFunctionItem;
class vtkDiscretizableColorTransferFunction;
class vtkTransform2D;

class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKCompositeTransferFunctionChart
  : public vtkChartXY
{
public:
  vtkTypeMacro(ctkVTKCompositeTransferFunctionChart, vtkChartXY)
  static ctkVTKCompositeTransferFunctionChart* New();


  void SetColorTransferFunction(vtkDiscretizableColorTransferFunction* function);
  void SetColorTransferFunction(vtkDiscretizableColorTransferFunction* function,
    double dataRangeMin, double dataRangeMax);

  // is a currently selected control point, false otherwise.
  bool GetCurrentControlPointColor(double rgb[3]);

  // Set the color of the current color control point.
  void SetCurrentControlPointColor(const double rgb[3]);

  /**
  *  Set/Get the current range
  *
  *  Set will clamp values into the current dataRange
  */
  void SetCurrentRange(double min, double max);
  double* GetCurrentRange();


  /**
  * Center the current position to the given point
  */
  void CenterRange(double center);

  vtkCompositeControlPointsItem* GetControlPointsItem();

  bool IsProcessingColorTransferFunction() const;

  bool MouseMoveEvent(const vtkContextMouseEvent &mouse) VTK_OVERRIDE;
  bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse) VTK_OVERRIDE;
  bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse) VTK_OVERRIDE;

private:
  void updateMarkerPosition(const vtkContextMouseEvent& m);

protected:
  vtkSmartPointer<vtkCompositeTransferFunctionItem> compositeHiddenItem;
  vtkSmartPointer<ctkVTKDiscretizableColorTransferControlPointsItem> controlPoints;

  vtkSmartPointer<vtkTransform2D> Transform;
  vtkSmartPointer<vtkDiscretizableColorTransferFunction> workingFunction;

  vtkPlot *MinPlot;
  vtkPlot *MaxPlot;
  vtkPlot *MinLinePlot;
  vtkPlot *MaxLinePlot;
  vtkPlot *MinDataPlot;
  vtkPlot *MaxDataPlot;
  vtkSmartPointer<ctkVTKHistogramMarker> MinMarker;
  vtkSmartPointer<ctkVTKHistogramMarker> MaxMarker;
  vtkSmartPointer<ctkVTKHistogramMarker> MinDataMarker;
  vtkSmartPointer<ctkVTKHistogramMarker> MaxDataMarker;

  enum class RangeMoving {
    NONE, MIN, MAX
  } rangeMoving = RangeMoving::NONE;
  double DataRange[2];
  double CurrentRange[2];

  bool ProcessingColorTransferFunction;

private:
  ctkVTKCompositeTransferFunctionChart();
  virtual ~ctkVTKCompositeTransferFunctionChart();
};

#endif
