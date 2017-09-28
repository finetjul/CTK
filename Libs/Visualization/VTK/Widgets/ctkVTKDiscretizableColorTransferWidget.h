#ifndef __ctkVTKDiscretizableColorTransferWidget_h
#define __ctkVTKDiscretizableColorTransferWidget_h

#include "ctkVisualizationVTKWidgetsExport.h"

#include <QWidget>
#include <vtkSmartPointer.h>

class ctkVTKScalarsToColorsComboBox;
class ctkVTKScalarsToColorsEditor;
class vtkContextView;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkPiecewiseFunction;
class vtkScalarsToColors;
class vtkTable;
class QComboBox;
class QCheckBox;
class QLineEdit;
class QSpinBox;
class QToolButton;
class QVTKWidget;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDiscretizableColorTransferWidget: public QWidget
{
Q_OBJECT
public:
	explicit ctkVTKDiscretizableColorTransferWidget(QWidget* parent_ = nullptr);
  virtual ~ctkVTKDiscretizableColorTransferWidget();

  void SetColorTransferFunction(vtkScalarsToColors* ctf);
  void SetHistogramTable(vtkTable* histogram);

signals:
	void dynamicChanged(double min, double max);
	void toogleVolumeRendering(bool enable);

  void currentScalarsToColorsModified();
  void currentScalarsToColorsChanged(vtkScalarsToColors* ctf);

public slots:
	void transparencyChanged(int value100);
	void onScalarOpacityFunctionChanged();

  void onCurrentPointChanged();
	void onCurrentPointEdit();
  void onCurrentPointModified();

	void onResetRangeClicked();
	void onCenterRangeClicked();
	void onInvertClicked();
	void onRangeEditorReturn();

  void onPaletteIndexChanged(vtkScalarsToColors* ctf);
  void onHistogramDataModified(vtkTable* histogramTable);

private:
	QIcon getColorIcon(QColor color);

  ctkVTKScalarsToColorsComboBox* scalarsToColorsSelector;
  vtkSmartPointer<ctkVTKScalarsToColorsEditor> scalarsToColorsEditor;
  vtkSmartPointer<vtkContextView> histogramView;
  vtkSmartPointer<vtkEventQtSlotConnect> eventLink;

  vtkSmartPointer<vtkScalarsToColors> colorTransferFunction;

  vtkSmartPointer<vtkTable> histogramTable;

	vtkPiecewiseFunction* scalarOpacityFunction = nullptr;

	QToolButton* gradientOpacityButton = nullptr;

	QVTKWidget* qvtk;

	//Option part
	QComboBox* lutSelector;
	QToolButton* nanButton;
	QCheckBox* discretizeCheck;
	QSpinBox* classesDiscret;

	QLineEdit* minRange;
	QLineEdit* maxRange;

	double dataRange[2];
	double dataMean;
};
#endif // __ctkVTKDiscretizableColorTransferWidget_h
