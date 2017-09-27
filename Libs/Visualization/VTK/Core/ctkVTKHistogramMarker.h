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

#ifndef __ctkVTKHistogramMarker_h
#define __ctkVTKHistogramMarker_h

#include <vtkObject.h>
#include <vtkSmartPointer.h>

class vtkTable;

class ctkVTKHistogramMarker: public vtkObject
{
public:

	static ctkVTKHistogramMarker* New();

	ctkVTKHistogramMarker();
	virtual ~ctkVTKHistogramMarker();

	void SetPosition(double pos);

	vtkSmartPointer<vtkTable> getTable();

private:
	vtkSmartPointer<vtkTable> m_table;

};

#endif /* __ctkVTKHistogramMarker_h */
