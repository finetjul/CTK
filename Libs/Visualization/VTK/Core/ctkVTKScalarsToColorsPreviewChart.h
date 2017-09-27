/*
* ctkVTKScalarsToColorsPreviewChart.h
*
*  Created on: 25 juil. 2017
*      Author: a
*/

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

  void SetColorTransferFunction(vtkSmartPointer<vtkColorTransferFunction> colorTransfer);
  void SetColorTransferFunction(vtkColorTransferFunction* function, double minDataRange, double maxDataRange);

protected:
  ctkVTKScalarsToColorsPreviewChart();
  virtual ~ctkVTKScalarsToColorsPreviewChart();
};

#endif /* __ctkVTKScalarsToColorsPreviewChart_h */
