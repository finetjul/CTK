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

#ifndef __ctkVTKScalarsToColorsComboBox_h
#define __ctkVTKScalarsToColorsComboBox_h

// CTK includes
#include "ctkComboBox.h"
#include "ctkVisualizationVTKWidgetsExport.h"

// VTK includes
#include <vtkSmartPointer.h>

class ctkVTKScalarsToColorsComboBoxPrivate;
class vtkScalarsToColors;

/// \ingroup Visualization_VTK_Widgets
///
/// QComboBox linked to vtkScalarsToColors objects
class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKScalarsToColorsComboBox
  : public ctkComboBox
{
  Q_OBJECT

public:
  /// Superclass typedef
  typedef ctkComboBox Superclass;

  /// Constructors
  explicit ctkVTKScalarsToColorsComboBox(QWidget* parent = 0);
  virtual ~ctkVTKScalarsToColorsComboBox();

  void addScalarsToColors(const char* name, vtkSmartPointer<vtkScalarsToColors> ctf);

signals:
  void currentScalarsToColorsChanged(vtkScalarsToColors*);

protected slots:
  void onCurrentIndexChanged(int);

protected:
  QScopedPointer<ctkVTKScalarsToColorsComboBoxPrivate> d_ptr;

  std::vector<vtkSmartPointer<vtkScalarsToColors>> scalarsToColorsVector;

private:
  Q_DECLARE_PRIVATE(ctkVTKScalarsToColorsComboBox);
  Q_DISABLE_COPY(ctkVTKScalarsToColorsComboBox);
};

#endif
