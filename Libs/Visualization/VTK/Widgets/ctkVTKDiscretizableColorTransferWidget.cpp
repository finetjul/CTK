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

#include "ctkVTKDiscretizableColorTransferWidget.h"

// CTK includes
#include "ctkDoubleSlider.h"
#include "ctkVTKScalarsToColorsComboBox.h"
#include "ctkVTKScalarsToColorsEditor.h"
#include "ctkVTKScalarsToColorsUtils.h"
#include "ui_ctkVTKDiscretizableColorTransferWidget.h"

// Qt includes
#include <QColorDialog>
#include <QCheckBox>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QIcon>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

// VTK includes
#include <QVTKWidget.h>
#include <vtkCallbackCommand.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkControlPointsItem.h>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkDoubleArray.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkIntArray.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>

// ----------------------------------------------------------------------------
class ctkVTKDiscretizableColorTransferWidgetPrivate :
  public Ui_ctkVTKDiscretizableColorTransferWidget
{
  Q_DECLARE_PUBLIC(ctkVTKDiscretizableColorTransferWidget);
protected:
  ctkVTKDiscretizableColorTransferWidget* const q_ptr;

public:
  ctkVTKDiscretizableColorTransferWidgetPrivate(ctkVTKDiscretizableColorTransferWidget& object);

  void setupUi(QWidget* widget);

  static QIcon getColorIcon(const QColor& color);

  vtkSmartPointer<ctkVTKScalarsToColorsEditor> scalarsToColorsEditor;
  vtkSmartPointer<vtkContextView> histogramView;
  vtkSmartPointer<vtkEventQtSlotConnect> eventLink;

  vtkSmartPointer<vtkScalarsToColors> colorTransferFunction;

  vtkSmartPointer<vtkTable> histogramTable;

  vtkPiecewiseFunction* scalarOpacityFunction;

  QToolButton* gradientOpacityButton;

  //Option part
  QToolButton* nanButton;
  QCheckBox* discretizeCheck;
  QSpinBox* classesDiscret;
  QWidget* optionPanel;

  double dataRange[2];
  double dataMean;

  double opacitySliderValue;

  vtkSmartPointer<vtkCallbackCommand> rangeModifiedCallBack;

  static void onHistogramRangeValuesChanged(vtkObject *caller, unsigned long eid,
    void *clientdata, void *calldata);
};

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidgetPrivate
::ctkVTKDiscretizableColorTransferWidgetPrivate(ctkVTKDiscretizableColorTransferWidget& object)
  : q_ptr(&object)
{
  this->scalarsToColorsSelector = nullptr;
  this->scalarOpacityFunction = nullptr;
  this->gradientOpacityButton = nullptr;

  //Option part
  this->nanButton = nullptr;
  this->discretizeCheck = nullptr;
  this->classesDiscret = nullptr;

  this->dataRange[0] = VTK_DOUBLE_MAX;
  this->dataRange[1] = VTK_DOUBLE_MIN;
  this->dataMean = 0.;

  this->opacitySliderValue = 0.;

  rangeModifiedCallBack = vtkSmartPointer<vtkCallbackCommand>::New();
  rangeModifiedCallBack->SetClientData(this);
  rangeModifiedCallBack->SetCallback(this->onHistogramRangeValuesChanged);
}

void ctkVTKDiscretizableColorTransferWidgetPrivate::onHistogramRangeValuesChanged(vtkObject *caller, unsigned long eid,
  void *clientData, void *callData)
{
  if (eid != vtkCommand::CursorChangedEvent)
  {
    return;
  }

  ctkVTKDiscretizableColorTransferWidgetPrivate* Self =
    reinterpret_cast<ctkVTKDiscretizableColorTransferWidgetPrivate*>(clientData);

  double* newRange = Self->scalarsToColorsEditor->GetCurrentRange();
  Self->rangeSlider->setMinimumValue(newRange[0]);
  Self->rangeSlider->setMaximumValue(newRange[1]);
}

//-----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKDiscretizableColorTransferWidget);

  this->Ui_ctkVTKDiscretizableColorTransferWidget::setupUi(widget);

  this->scalarsToColorsEditor = vtkSmartPointer<ctkVTKScalarsToColorsEditor>::New();
  this->histogramView = vtkSmartPointer<vtkContextView> ::New();
  this->eventLink = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  this->histogramView->GetScene()->AddItem(this->scalarsToColorsEditor.Get());
  this->histogramView->SetInteractor(this->scalarsToColorsView->GetInteractor());
  this->scalarsToColorsView->SetRenderWindow(this->histogramView->GetRenderWindow());

  this->histogramView->GetRenderWindow()->Render();
  this->histogramView->GetRenderer()->SetBackground(
    ctkVTKScalarsToColorsEditor::BACKGROUND_COLOR);

  this->opacitySliderValue = opacitySlider->value();

  this->scalarsToColorsSelector->addScalarsToColors(nullptr, q->tr("Reset"));
  this->scalarsToColorsSelector->setCurrentIndex(-1);

  eventLink->Connect(scalarsToColorsEditor.Get(), vtkControlPointsItem::CurrentPointEditEvent,
    q, SLOT(onCurrentPointEdit()));
  eventLink->Connect(scalarsToColorsEditor.Get(), vtkControlPointsItem::CurrentPointChangedEvent,
    q, SLOT(onCurrentPointChanged()));

  scalarsToColorsEditor->AddObserver(vtkCommand::CursorChangedEvent, rangeModifiedCallBack.Get());

  QObject::connect(scalarsToColorsSelector, SIGNAL(currentScalarsToColorsChanged(vtkScalarsToColors*)),
    q, SLOT(onPaletteIndexChanged(vtkScalarsToColors*)));

  QObject::connect(opacitySlider, SIGNAL(valueChanged(double)), q, SLOT(onGlobalOpacitySliderValueChanged(double)));

  QObject::connect(resetRangeButton, SIGNAL(clicked()), q, SLOT(onResetRangeClicked()));

  QObject::connect(centerRangeButton, SIGNAL(clicked()), q, SLOT(onCenterRangeClicked()));

  QObject::connect(invertColorTransferFunctionButton, SIGNAL(clicked()), q, SLOT(onInvertClicked()));

  QObject::connect(rangeSlider, SIGNAL(valuesChanged(double, double)), q, SLOT(onRangeSliderValuesChanged(double, double)));

  //option panel part
  optionPanel = new QWidget(q, Qt::Popup);
  auto panelLayout = new QGridLayout(optionPanel);

  optionButton->setIcon(q->style()->standardIcon(
    QStyle::SP_FileDialogDetailedView, nullptr, optionButton));

  QLabel* nanLabel = new QLabel("NaN values :");
  nanButton = new QToolButton;
  panelLayout->addWidget(nanLabel, 0, 0);
  panelLayout->addWidget(nanButton, 0, 1);

  discretizeCheck = new QCheckBox;
  discretizeCheck->setText("Discretize");
  classesDiscret = new QSpinBox;
  classesDiscret->setMinimum(1);
  classesDiscret->setMaximum(255);
  classesDiscret->setEnabled(discretizeCheck->isChecked());
  panelLayout->addWidget(discretizeCheck, 1, 0);
  panelLayout->addWidget(classesDiscret, 1, 1);

  QObject::connect(optionButton, SIGNAL(clicked()), q, SLOT(onOptionButtonClicked()));

  QObject::connect(nanButton, SIGNAL(clicked()), q, SLOT(onNaNButtonClicked()));

  QObject::connect(discretizeCheck, SIGNAL(stateChanged(int)), q, SLOT(onDiscretizeCheckBoxStateChanged(int)));

  QObject::connect(classesDiscret, SIGNAL(valueChanged(int)), q, SLOT(onDiscreteClassesSpinBoxValueChanged(int)));
}

// ----------------------------------------------------------------------------
QIcon ctkVTKDiscretizableColorTransferWidgetPrivate::getColorIcon(const QColor& color)
{
  QPixmap pix(32, 32);
  pix.fill(color);
  return QIcon(pix);
}

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidget::ctkVTKDiscretizableColorTransferWidget(QWidget* parent)
  : QWidget(parent)
  , d_ptr(new ctkVTKDiscretizableColorTransferWidgetPrivate(*this))
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->setupUi(this);
}

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidget::~ctkVTKDiscretizableColorTransferWidget()
{
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setColorTransferFunction(vtkScalarsToColors* ctf)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->colorTransferFunction = ctf;
  d->scalarsToColorsEditor->SetColorTransfertFunction(ctf);

  if (ctf == nullptr)
  {
    d->rangeSlider->setRange(0., 0.);
    d->rangeSlider->setValues(0., 0.);
    return;
  }

  double* newRange = d->scalarsToColorsEditor->GetColorTransfertFunction()->GetRange();
  d->rangeSlider->setRange(newRange[0], newRange[1]);
  d->rangeSlider->setValues(newRange[0], newRange[1]);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setHistogram(vtkImageAccumulate* histogram)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  histogram->Update();

  d->dataRange[0] = histogram->GetMin()[0];
  d->dataRange[1] = histogram->GetMax()[0];
  d->dataMean = histogram->GetMean()[0];

  int* output = static_cast<int*>(histogram->GetOutput()->GetScalarPointer());
  double spacing = histogram->GetComponentSpacing()[0];
  double bin = histogram->GetComponentOrigin()[0];

  vtkSmartPointer<vtkDoubleArray> bins = vtkSmartPointer<vtkDoubleArray>::New();
  bins->SetNumberOfComponents(1);
  bins->SetNumberOfTuples(255);
  bins->SetName("image_extents");
  vtkSmartPointer<vtkIntArray> frequencies = vtkSmartPointer<vtkIntArray>::New();
  frequencies->SetNumberOfComponents(1);
  frequencies->SetNumberOfTuples(255);
  frequencies->SetName("Frequency");

  for (unsigned int j = 0; j < 255; ++j)
  {
    bins->SetTuple1(j, bin);
    bin += spacing;
    frequencies->SetTuple1(j, *output++);
  }

  vtkNew<vtkTable> table;
  table->AddColumn(bins);
  table->AddColumn(frequencies);

  d->histogramTable = table;
  d->scalarsToColorsEditor->SetHistogramTable(table,
    "image_extents", "Frequency");

  d->scalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onPaletteIndexChanged(vtkScalarsToColors* ctf)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  //Setting the transfer function to the scalarsToColorsEditor results
  // in converting ctf to a vtkDiscretizableTransferFunction
  d->scalarsToColorsEditor->SetColorTransfertFunction(ctf);

  //emit signal after the conversion
  emit(currentScalarsToColorsChanged(d->scalarsToColorsEditor->GetColorTransfertFunction()));

  //Update rendering
  d->scalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onGlobalOpacitySliderValueChanged(double value)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->SetGlobalOpacity(value/d->opacitySliderValue);
  d->opacitySliderValue = value;
  d->scalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onOptionButtonClicked()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  QPoint buttonPos = this->mapToGlobal(d->optionButton->pos());
  d->optionPanel->move(buttonPos.x(), buttonPos.y() + d->optionButton->height());
  d->optionPanel->setVisible(!(d->optionPanel->isVisible()));
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onNaNButtonClicked()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction() == nullptr)
  {
    d->nanButton->setIcon(d->getColorIcon(QColor(0, 0, 0, 0)));
    return;
  }

  double r, g, b;
  d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction()->GetNanColor(r, g, b);
  QColor selected = QColorDialog::getColor(QColor(255 * r, 255 * g, 255 * b, 255),
    d->optionPanel, "Select NaN values color", QColorDialog::DontUseNativeDialog);
  d->nanButton->setIcon(d->getColorIcon(selected));
  d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction()->SetNanColor(
    selected.redF(), selected.greenF(), selected.blueF());
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onDiscretizeCheckBoxStateChanged(int state)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction() == nullptr)
  {
    d->classesDiscret->setValue(0);
    d->classesDiscret->setEnabled(false);
    return;
  }

  d->classesDiscret->setEnabled(state == Qt::Checked);
  d->classesDiscret->setValue(d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction()->GetNumberOfValues());
  d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction()->SetDiscretize(state == Qt::Checked);

  d->scalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onDiscreteClassesSpinBoxValueChanged(int value)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsEditor->GetDiscretizableColorTransfertFunction()->SetNumberOfValues(value);
  d->scalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onRangeSliderValuesChanged(double minValue, double maxValue)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsEditor->SetCurrentRange(minValue, maxValue);
  d->scalarsToColorsView->GetInteractor()->Render();
}


// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onCurrentPointChanged()
{
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onCurrentPointEdit()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  double rgb[3];
  if (d->scalarsToColorsEditor->GetCurrentControlPointColor(rgb))
  {
    QColor color = QColorDialog::getColor(QColor::fromRgbF(rgb[0], rgb[1], rgb[2]), this, "Select Color for Control Point",
        QColorDialog::DontUseNativeDialog);
    if (color.isValid())
    {
      rgb[0] = color.redF();
      rgb[1] = color.greenF();
      rgb[2] = color.blueF();
      d->scalarsToColorsEditor->SetCurrentControlPointColor(rgb);
      d->scalarsToColorsView->GetInteractor()->Render();
    }
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onCurrentPointModified()
{
  emit(currentScalarsToColorsModified());
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onResetRangeClicked()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  if (d->dataRange[0] <= d->dataRange[1])
  {
    d->scalarsToColorsEditor->SetCurrentRange(d->dataRange[0], d->dataRange[1]);
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onCenterRangeClicked()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->CenterRange(d->dataMean);
  d->scalarsToColorsView->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onInvertClicked()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->InvertColorTransferFunction();
  d->scalarsToColorsView->GetInteractor()->Render();
}


