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

#ifndef __ctkVTKScalarsToColorsPreviewChart_h
#define __ctkVTKScalarsToColorsPreviewChart_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkChartXY.h>
#include <vtkSmartPointer.h>

class vtkColorTransferFunction;

class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKScalarsToColorsPreviewChart
  : public vtkChartXY
{
public:
  vtkTypeMacro(ctkVTKScalarsToColorsPreviewChart, vtkChartXY)
  static ctkVTKScalarsToColorsPreviewChart* New();

  void SetColorTransferFunction(vtkColorTransferFunction* colorTransfer);

protected:
  ctkVTKScalarsToColorsPreviewChart();
  virtual ~ctkVTKScalarsToColorsPreviewChart();
};

#endif /* __ctkVTKScalarsToColorsPreviewChart_h */
