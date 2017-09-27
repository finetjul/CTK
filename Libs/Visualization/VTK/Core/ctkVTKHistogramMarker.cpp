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

#include <ctkVTKHistogramMarker.h>

#include <vtkTable.h>
#include <vtkFloatArray.h>

vtkStandardNewMacro(ctkVTKHistogramMarker)

ctkVTKHistogramMarker::ctkVTKHistogramMarker()
{
	m_table = vtkSmartPointer<vtkTable>::New();

	vtkSmartPointer<vtkFloatArray> arrX = vtkSmartPointer<vtkFloatArray>::New();
	arrX->SetName("X Axis");
	m_table->AddColumn(arrX);

	vtkSmartPointer<vtkFloatArray> arrC = vtkSmartPointer<vtkFloatArray>::New();
	arrC->SetName("YAxis");
	m_table->AddColumn(arrC);

	m_table->SetNumberOfRows(2);
	m_table->SetValue(0, 0, 55);
	m_table->SetValue(0, 1, 0);

	m_table->SetValue(1, 0, 55);
	m_table->SetValue(1, 1, 1);
}

vtkSmartPointer<vtkTable> ctkVTKHistogramMarker::getTable()
{
	return m_table;
}

void ctkVTKHistogramMarker::SetPosition(double pos)
{
	m_table->SetValue(0, 0, pos);
	m_table->SetValue(1, 0, pos);
	m_table->Modified();
}

ctkVTKHistogramMarker::~ctkVTKHistogramMarker()
{
}
