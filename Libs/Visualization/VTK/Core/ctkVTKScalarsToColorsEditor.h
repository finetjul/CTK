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

#ifndef __ctkVTKScalarsToColorsEditor_h
#define __ctkVTKScalarsToColorsEditor_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkAbstractContextItem.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkVector.h>

class ctkVTKCompositeTransferFunctionChart;
class ctkVTKHistogramChart;
class ctkVTKScalarsToColorsPreviewChart;
class vtkAxis;
class vtkDiscretizableColorTransferFunction;
class vtkScalarsToColors;
class vtkTable;

// This class is a chart that combines a histogram from a data set
// a color bar editor, and an opacity editor.
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKScalarsToColorsEditor
  : public vtkAbstractContextItem
{
public:
  static double BACKGROUND_COLOR[3];

  vtkTypeMacro(ctkVTKScalarsToColorsEditor, vtkAbstractContextItem)
  static ctkVTKScalarsToColorsEditor* New();

  void SetColorTransfertFunction(vtkScalarsToColors* ctf);
  vtkSmartPointer<vtkScalarsToColors> GetColorTransfertFunction();

  void SetDiscretizableColorTransfertFunction(vtkDiscretizableColorTransferFunction* dctf);
  vtkSmartPointer<vtkDiscretizableColorTransferFunction> GetDiscretizableColorTransfertFunction();

  void SetHistogramTable(vtkTable* table, const char* xAxisColumn, const char* yAxisColumn);

  // Paint event for the editor.
  virtual bool Paint(vtkContext2D* painter) override;

  // is a currently selected control point, false otherwise.
  bool GetCurrentControlPointColor(double rgb[3]);

  // Set the color of the current color control point.
  void SetCurrentControlPointColor(const double rgb[3]);

  void SetCurrentRange(double min, double max);
  double* GetCurrentRange();
  void CenterRange(double center);

  void SetGlobalOpacity(double opacity);
  void InvertColorTransferFunction();

protected:
  vtkSmartPointer<ctkVTKCompositeTransferFunctionChart> overlayChart;
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
#endif // __ctkVTKScalarsToColorsEditor_h
