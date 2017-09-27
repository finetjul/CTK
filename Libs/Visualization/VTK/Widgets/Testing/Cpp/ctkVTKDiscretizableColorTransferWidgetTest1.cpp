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

// Qt includes
#include <QApplication>
#include <QDebug>
#include <QTimer>

// CTK includes
#include "ctkVTKDiscretizableColorTransferWidget.h"

// VTK includes
#include <QVTKWidget.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkNew.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkTable.h>

// STD includes

//-----------------------------------------------------------------------------
int ctkVTKDiscretizableColorTransferWidgetTest1(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  vtkSmartPointer<vtkDoubleArray> bins = vtkSmartPointer<vtkDoubleArray>::New();
  bins->SetNumberOfComponents(1);
  bins->SetNumberOfTuples(255);
  bins->SetName("image_extents");
  vtkSmartPointer<vtkIntArray> frequencies = vtkSmartPointer<vtkIntArray>::New();
  frequencies->SetNumberOfComponents(1);
  frequencies->SetNumberOfTuples(255);
  frequencies->SetName("Frequency");

  double spacing = 1;
  double bin = 0;

  for (unsigned int j = 0; j < 255; ++j)
  {
    bins->SetTuple1(j, bin);
    bin += spacing;
    frequencies->SetTuple1(j, j);
  }

  vtkNew<vtkTable> table;
  table->AddColumn(bins);
  table->AddColumn(frequencies);

  //Dummy presets
  vtkNew<vtkDiscretizableColorTransferFunction> discretizableCTF;
  discretizableCTF->AddRGBPoint(0.0, 0, 0, 1.0);
  discretizableCTF->AddRGBPoint(255.0, 1.0, 0, 0);
  vtkNew<vtkPiecewiseFunction> piecewiseFunction;
  piecewiseFunction->AddPoint(0.0, 0);
  piecewiseFunction->AddPoint(255.0, 1.0);
  discretizableCTF->SetScalarOpacityFunction(piecewiseFunction.Get());
  discretizableCTF->EnableOpacityMappingOn();

  ctkVTKDiscretizableColorTransferWidget mWidget;
  mWidget.SetColorTransferFunction(discretizableCTF.Get());
  mWidget.SetHistogramTable(table.Get());
  mWidget.show();

  if (argc < 2 || QString(argv[1]) != "-I")
  {
    QTimer::singleShot(1000, &app, SLOT(quit()));
  }

  return app.exec();
}
