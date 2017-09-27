/*
 * ctkVTKHistogramMarker.cxx
 *
 *  Created on: 18 août 2017
 *      Author: a
 */

#include <ctkVTKHistogramMarker.h>
#include <vtkPlotLine.h>

#include <vtkTable.h>
#include <vtkFloatArray.h>

vtkStandardNewMacro(ctkVTKHistogramMarker)

ctkVTKHistogramMarker::ctkVTKHistogramMarker() {

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

vtkSmartPointer<vtkTable> ctkVTKHistogramMarker::getTable() {
	return m_table;
}

void ctkVTKHistogramMarker::SetPosition(double pos) {
	m_table->SetValue(0, 0, pos);
	m_table->SetValue(1, 0, pos);
	m_table->Modified();
}

ctkVTKHistogramMarker::~ctkVTKHistogramMarker() {

	// TODO Auto-generated destructor stub
}
