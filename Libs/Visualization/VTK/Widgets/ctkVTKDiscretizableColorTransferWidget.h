#ifndef __ctkVTKDiscretizableColorTransferWidget_h
#define __ctkVTKDiscretizableColorTransferWidget_h

#include <QWidget>
#include <QMetaType>

#include <vtkNew.h>
#include <vtkSmartPointer.h>

#include "ctkVisualizationVTKWidgetsExport.h"
#include "ctkVTKObject.h"

#include <vtkDiscretizableColorTransferFunction.h>

class vtkContextView;
class vtkEventQtSlotConnect;
class vtkPiecewiseFunction;
class vtkObject;
class vtkTable;
class QToolButton;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QLineEdit;

class QVTKWidget;
class QComboBox;

class vtkImageAccumulate;

class ctkVTKScalarsToColorsComboBox;
class ctkVTKScalarsToColorsEditor;

class CTK_VISUALIZATION_VTK_WIDGETS_EXPORT ctkVTKDiscretizableColorTransferWidget: public QWidget{
Q_OBJECT
public:
	explicit ctkVTKDiscretizableColorTransferWidget(QWidget* parent_ = nullptr);
	~ctkVTKDiscretizableColorTransferWidget() override;

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
	void onColorFunctionChanged();

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
  vtkNew<ctkVTKScalarsToColorsEditor> scalarsToColorsEditor;
	vtkNew<vtkContextView> histogramView;
	vtkNew<vtkEventQtSlotConnect> eventLink;

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
