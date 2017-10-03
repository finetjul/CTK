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
// CTK includes
#include "ctkLogger.h"
#include "ctkVTKScalarsToColorsUtils.h"

// VTK includes
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkScalarsToColors.h>

//----------------------------------------------------------------------------
static ctkLogger logger("org.commontk.visualization.vtk.core.ctkVTKScalarsToColorsUtils");
//----------------------------------------------------------------------------

void ctk::remapColorScale(vtkDiscretizableColorTransferFunction* colorTransferFunction,
  vtkDiscretizableColorTransferFunction* rescaledColorTransferFunction, double minRescale, double maxRescale)
{
  //The code for internal and external colortables could be in the same loops but to make
  // this clear about having or not set the pointer, we do it like this
  //Color
  double rescaleWidth = maxRescale - minRescale;
  double * r = colorTransferFunction->GetRange();
  //Map to the histogram color map and to the external
  //  rescaledColorTransferFunction->SetEnableModifiedEvents(false);
  rescaledColorTransferFunction->RemoveAllPoints();

  for (int i = 0; i < colorTransferFunction->GetSize(); i++)
  {
    double val[6];
    colorTransferFunction->GetNodeValue(i, val);
    double normalized = (val[0] - r[0]) / (r[1] - r[0]);

    double newPostHisto = minRescale + normalized * rescaleWidth;
    rescaledColorTransferFunction->AddRGBPoint(newPostHisto, val[1], val[2], val[3], val[4], val[5]);
  }

  //Opacity
  if (rescaledColorTransferFunction->GetScalarOpacityFunction())
  {
    rescaledColorTransferFunction->GetScalarOpacityFunction()->RemoveAllPoints();
  }
  else
  {
    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();
    rescaledColorTransferFunction->SetScalarOpacityFunction(opacityFunction);
  }

  if (!colorTransferFunction->GetScalarOpacityFunction())
  {
    vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
      vtkSmartPointer<vtkPiecewiseFunction>::New();
    colorTransferFunction->SetScalarOpacityFunction(opacityFunction);
  }

  for (int i = 0; i < colorTransferFunction->GetScalarOpacityFunction()->GetSize(); i++)
  {
    double val[4];
    colorTransferFunction->GetScalarOpacityFunction()->GetNodeValue(i, val);
    double normalized = (val[0] - r[0]) / (r[1] - r[0]);

    double newPostHisto = minRescale + normalized * rescaleWidth;
    rescaledColorTransferFunction->GetScalarOpacityFunction()->AddPoint(newPostHisto, val[1], val[2], val[3]);
  }
  //  rescaledColorTransferFunction->SetDiscretize(colorTransferFunction->GetDiscretize());
  //  rescaledColorTransferFunction->SetNumberOfValues(colorTransferFunction->GetNumberOfValues());
    //rescaledColorTransferFunction->SetEnableModifiedEvents(true);
  rescaledColorTransferFunction->Build();
}

/**
* Simple copy from one to another
*/
void ctk::remapColorScale(vtkDiscretizableColorTransferFunction* colorTransferFunction, double minRescale, double maxRescale)
{
  vtkSmartPointer<vtkDiscretizableColorTransferFunction> rescaled = vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
  ctk::remapColorScale(colorTransferFunction, rescaled, minRescale, maxRescale);
  ctk::remapColorScale(rescaled, colorTransferFunction, minRescale, maxRescale);
}

/**
* Simply swap values
*/
void ctk::reverseColorMap(vtkDiscretizableColorTransferFunction* ctf)
{
  if (ctf == nullptr)
  {
    return;
  }

  int size = ctf->GetSize();
  for (int i = 0; i < size / 2; i++)
  {
    double val[6];
    ctf->GetNodeValue(i, val);

    double valRev[6];
    ctf->GetNodeValue(size - 1 - i, valRev);

    std::swap(val[0], valRev[0]);

    ctf->SetNodeValue(i, valRev);
    ctf->SetNodeValue(size - 1 - i, val);
  }
  ctf->Modified();
}

void ctk::setTransparency(vtkDiscretizableColorTransferFunction* ctf, double transparency)
{
  if (ctf == nullptr)
  {
    return;
  }

  //Opacity
  for (int i = 0; i < ctf->GetScalarOpacityFunction()->GetSize(); i++)
  {
    double val[4];
    ctf->GetScalarOpacityFunction()->GetNodeValue(i, val);

    val[1] *= transparency;

    val[1] = val[1] < 1e-6 ? 1e-6 : val[1];
    val[1] = val[1] > 1.0 ? 1.0 : val[1];

    ctf->GetScalarOpacityFunction()->SetNodeValue(i, val);
  }
  ctf->Modified();
}
