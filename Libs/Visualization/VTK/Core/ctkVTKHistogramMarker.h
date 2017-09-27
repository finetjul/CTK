/*
 * ctkVTKHistogramMarker.h
 *
 *  Created on: 18 août 2017
 *      Author: a
 */

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
