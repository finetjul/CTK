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

#ifndef __ctkVTKHistogramChart_h
#define __ctkVTKHistogramChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkNew.h>

class vtkPlotBar;
class vtkScalarsToColors;
class vtkTable;

class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKHistogramChart: public vtkChartXY
{
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
