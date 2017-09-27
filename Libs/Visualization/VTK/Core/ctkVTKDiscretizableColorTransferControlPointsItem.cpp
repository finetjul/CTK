#include <ctkVTKDiscretizableColorTransferControlPointsItem.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkObjectFactory.h>
#include <vtkPiecewiseFunction.h>

vtkStandardNewMacro(ctkVTKDiscretizableColorTransferControlPointsItem)

ctkVTKDiscretizableColorTransferControlPointsItem::ctkVTKDiscretizableColorTransferControlPointsItem()
{
}

ctkVTKDiscretizableColorTransferControlPointsItem::~ctkVTKDiscretizableColorTransferControlPointsItem()
{
}

bool ctkVTKDiscretizableColorTransferControlPointsItem::MouseButtonPressEvent(
  const vtkContextMouseEvent& mouse)
{
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON)
  {
    vtkVector2f vpos = mouse.GetPos();
    this->TransformScreenToData(vpos, vpos);
    double pos[2];
    pos[0] = vpos.GetX();
    pos[1] = vpos.GetY();

    bool pointOnFunction = this->PointNearPiecewiseFunction(pos);
    if (!pointOnFunction)
    {
      this->SetCurrentPoint(-1);
      return false;
    }
  }

	return this->Superclass::MouseButtonPressEvent(mouse);
}

bool ctkVTKDiscretizableColorTransferControlPointsItem::MouseButtonReleaseEvent(const vtkContextMouseEvent &mouse)
{
  // If no point is selected, abort event
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON &&
    this->GetCurrentPoint() < 0)
  {
    return false;
  }

	return this->Superclass::MouseButtonReleaseEvent(mouse);
}


bool ctkVTKDiscretizableColorTransferControlPointsItem::MouseMoveEvent(
		const vtkContextMouseEvent &mouse)
{
  // If no point is selected, abort event
  if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON &&
    this->GetCurrentPoint() < 0)
  {
    return false;
  }

	 return this->Superclass::MouseMoveEvent(mouse);
}

bool ctkVTKDiscretizableColorTransferControlPointsItem::PointNearPiecewiseFunction(
		const double position[2]) {
	double x = position[0];
	double y = 0.0;

	vtkPiecewiseFunction* pwf = this->GetOpacityFunction();
	if (!pwf) {
		return false;
	}

  // Evaluate the piewewise function at the given point and get the y position.
  // If we are within a small distance of the piecewise function, return true.
  // Otherwise, we are too far away from the line, and return false.
	pwf->GetTable(x, x, 1, &y, 1);
	return (fabs(y - position[1]) < 0.05);
}
