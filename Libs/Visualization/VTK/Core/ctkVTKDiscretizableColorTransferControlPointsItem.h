#ifndef __ctkVTKDiscretizableColorTransferControlPointsItem_h
#define __ctkVTKDiscretizableColorTransferControlPointsItem_h

#include "ctkVisualizationVTKCoreExport.h"

#include <vtkCompositeControlPointsItem.h>

class vtkContextMouseEvent;

// Special control points item class that overrides the MouseDoubleClickEvent()
// event handler to do nothing.
class CTK_VISUALIZATION_VTK_CORE_EXPORT ctkVTKDiscretizableColorTransferControlPointsItem
  : public vtkCompositeControlPointsItem {
public:
  vtkTypeMacro(ctkVTKDiscretizableColorTransferControlPointsItem, vtkCompositeControlPointsItem)
	static ctkVTKDiscretizableColorTransferControlPointsItem* New();

	bool MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse) override;

	bool MouseMoveEvent(const vtkContextMouseEvent &mouse) override;
	bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse) override;

protected:
	ctkVTKDiscretizableColorTransferControlPointsItem();
	virtual ~ctkVTKDiscretizableColorTransferControlPointsItem();

	// Utility function to determine whether a position is near the piecewise
	// function.
	bool PointNearPiecewiseFunction(const double pos[2]);

private:
	ctkVTKDiscretizableColorTransferControlPointsItem(
			const ctkVTKDiscretizableColorTransferControlPointsItem&); // Not implemented.
	void operator=(const ctkVTKDiscretizableColorTransferControlPointsItem&); // Not implemented.
};

#endif
