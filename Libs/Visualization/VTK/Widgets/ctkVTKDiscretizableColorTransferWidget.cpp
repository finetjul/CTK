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

  vtkSmartPointer<vtkTable> histogramTable;

  //Option part
  QToolButton* nanButton;
  QCheckBox* discretizeCheck;
  QSpinBox* classesDiscret;
  QWidget* optionPanel;

  double dataRange[2];
  double dataMean;

  double previousOpacityValue;

  vtkSmartPointer<vtkCallbackCommand> colorTransferFunctionModified;
  static void colorTransferFunctionModifiedCallback(vtkObject *caller, unsigned long eid,
    void *clientdata, void *calldata);
};

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidgetPrivate
::ctkVTKDiscretizableColorTransferWidgetPrivate(ctkVTKDiscretizableColorTransferWidget& object)
  : q_ptr(&object)
{
  this->scalarsToColorsSelector = nullptr;

  //Option part
  this->nanButton = nullptr;
  this->discretizeCheck = nullptr;
  this->classesDiscret = nullptr;

  this->dataRange[0] = VTK_DOUBLE_MAX;
  this->dataRange[1] = VTK_DOUBLE_MIN;
  this->dataMean = 0.;

  this->previousOpacityValue = 0.;

  colorTransferFunctionModified = vtkSmartPointer<vtkCallbackCommand>::New();
  colorTransferFunctionModified->SetClientData(this);
  colorTransferFunctionModified->SetCallback(this->colorTransferFunctionModifiedCallback);
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

  this->previousOpacityValue = opacitySlider->value();

  this->scalarsToColorsSelector->addScalarsToColors(nullptr, q->tr("Reset"));
  this->scalarsToColorsSelector->setCurrentIndex(-1);

  eventLink->Connect(scalarsToColorsEditor.Get(), vtkControlPointsItem::CurrentPointEditEvent,
    q, SLOT(onCurrentPointEdit()));
  eventLink->Connect(scalarsToColorsEditor.Get(), vtkControlPointsItem::CurrentPointChangedEvent,
    q, SLOT(onCurrentPointChanged()));

  QObject::connect(scalarsToColorsSelector, SIGNAL(currentScalarsToColorsChanged(vtkScalarsToColors*)),
    q, SLOT(onPaletteIndexChanged(vtkScalarsToColors*)));

  QObject::connect(opacitySlider, SIGNAL(valueChanged(double)), q, SLOT(setGlobalOpacity(double)));

  QObject::connect(resetRangeButton, SIGNAL(clicked()), q, SLOT(resetColorTransferFunctionRange()));

  QObject::connect(centerRangeButton, SIGNAL(clicked()), q, SLOT(centerColorTransferFunctionRange()));

  QObject::connect(invertColorTransferFunctionButton, SIGNAL(clicked()), q, SLOT(invertColorTransferFunction()));

  QObject::connect(rangeSlider, SIGNAL(valuesChanged(double, double)), q, SLOT(setColorTransferFunctionRange(double, double)));

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

  QObject::connect(optionButton, SIGNAL(clicked()), q, SLOT(toggleOptionPanelVisibility()));

  QObject::connect(nanButton, SIGNAL(clicked()), q, SLOT(setNaNColor()));

  QObject::connect(discretizeCheck, SIGNAL(toggled(bool)), q, SLOT(setDiscretize(bool)));

  QObject::connect(classesDiscret, SIGNAL(valueChanged(int)), q, SLOT(setNumberOfDiscreteValues(int)));

  QObject::connect(discretizeCheck, SIGNAL(toggled(bool)), classesDiscret, SLOT(setEnabled(bool)));
}

// ----------------------------------------------------------------------------
QIcon ctkVTKDiscretizableColorTransferWidgetPrivate::getColorIcon(const QColor& color)
{
  QPixmap pix(32, 32);
  pix.fill(color);
  return QIcon(pix);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidgetPrivate::colorTransferFunctionModifiedCallback(vtkObject *caller, unsigned long eid,
  void *clientdata, void *calldata)
{
  if (eid != vtkCommand::ModifiedEvent)
  {
    return;
  }
  ctkVTKDiscretizableColorTransferWidgetPrivate* self =
    reinterpret_cast<ctkVTKDiscretizableColorTransferWidgetPrivate*>(clientdata);

  vtkSmartPointer<vtkDiscretizableColorTransferFunction> dctf =
    self->scalarsToColorsEditor->GetDiscretizableColorTransferFunction();

  if (dctf->GetDiscretize())
  {
    dctf->Build();
  }

  self->discretizeCheck->setChecked(dctf->GetDiscretize());

  if (dctf->GetDiscretize())
  {
    self->classesDiscret->setValue(dctf->GetNumberOfValues());
  }

  double* newRange = self->scalarsToColorsEditor->GetCurrentRange();
  self->rangeSlider->setValues(newRange[0], newRange[1]);

  double r, g, b;
  self->scalarsToColorsEditor->GetDiscretizableColorTransferFunction()->GetNanColor(r, g, b);
  QColor selected = QColor::fromRgbF(r, g, b);
  self->nanButton->setIcon(self->getColorIcon(selected));

  //TODO: fix remapColorScale
  //self->scalarsToColorsView->GetInteractor()->Render();
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
void ctkVTKDiscretizableColorTransferWidget::setColorTransferFunction(
  vtkScalarsToColors* ctf)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  vtkScalarsToColors* oldCtf = d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction();
  if (oldCtf != nullptr)
  {
    oldCtf->RemoveObserver(d->colorTransferFunctionModified);
  }

  //Setting the transfer function to the scalarsToColorsEditor results
  // in converting ctf to a vtkDiscretizableTransferFunction
  d->scalarsToColorsEditor->SetColorTransferFunction(ctf);

  ctf = d->scalarsToColorsEditor->GetColorTransferFunction();
  emit(currentScalarsToColorsChanged(ctf));

  if (ctf == nullptr)
  {
    d->rangeSlider->setRange(0., 255.);
    d->rangeSlider->setValues(0., 1.);
    d->rangeSlider->setEnabled(false);
    d->previousOpacityValue = 0.0;
    d->opacitySlider->setValue(d->previousOpacityValue);
    d->opacitySlider->setEnabled(false);
    d->optionButton->setEnabled(false);
    d->resetRangeButton->setEnabled(false);
    d->centerRangeButton->setEnabled(false);
    d->invertColorTransferFunctionButton->setEnabled(false);
    return;
  }

  // Set sliders values depending on the new color transfer function
  d->rangeSlider->setEnabled(true);
  d->opacitySlider->setEnabled(true);
  d->optionButton->setEnabled(true);
  d->resetRangeButton->setEnabled(true);
  d->centerRangeButton->setEnabled(true);
  d->invertColorTransferFunctionButton->setEnabled(true);

  double* newRange =
    d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction()->GetRange();
  d->rangeSlider->setRange(newRange[0], newRange[1]);

  d->previousOpacityValue = 1.0;
  d->opacitySlider->setValue(d->previousOpacityValue);

  ctf->AddObserver(
    vtkCommand::ModifiedEvent, d->colorTransferFunctionModified);
  d->colorTransferFunctionModified->Execute(ctf, vtkCommand::ModifiedEvent, this);
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

  this->setColorTransferFunction(ctf);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setGlobalOpacity(double value)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->SetGlobalOpacity(value/d->previousOpacityValue);
  d->previousOpacityValue = value;
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::toggleOptionPanelVisibility()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  QPoint buttonPos = this->mapToGlobal(d->optionButton->pos());
  d->optionPanel->move(buttonPos.x(), buttonPos.y() + d->optionButton->height());
  d->optionPanel->setVisible(!(d->optionPanel->isVisible()));
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setNaNColor()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  if (d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction() == nullptr)
  {
    d->nanButton->setIcon(d->getColorIcon(QColor(0, 0, 0, 0)));
    return;
  }

  double r, g, b;
  d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction()->GetNanColor(r, g, b);
  QColor selected = QColorDialog::getColor(QColor(255 * r, 255 * g, 255 * b, 255),
    d->optionPanel, "Select NaN values color", QColorDialog::DontUseNativeDialog);
  d->nanButton->setIcon(d->getColorIcon(selected));
  d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction()->SetNanColor(
    selected.redF(), selected.greenF(), selected.blueF());
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setDiscretize(bool checked)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction()->SetDiscretize(checked);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setNumberOfDiscreteValues(int value)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->GetDiscretizableColorTransferFunction()->SetNumberOfValues(value);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setColorTransferFunctionRange(double minValue, double maxValue)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);

  d->scalarsToColorsEditor->SetCurrentRange(minValue, maxValue);
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
    }
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onCurrentPointModified()
{
  emit(currentScalarsToColorsModified());
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::resetColorTransferFunctionRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  if (d->dataRange[0] <= d->dataRange[1])
  {
    d->scalarsToColorsEditor->SetCurrentRange(d->dataRange[0], d->dataRange[1]);
  }
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::centerColorTransferFunctionRange()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->CenterRange(d->dataMean);
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::invertColorTransferFunction()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->InvertColorTransferFunction();
}


