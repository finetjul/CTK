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
#include <ctkDoubleSlider.h>
#include <ctkVTKScalarsToColorsComboBox.h>
#include <ctkVTKScalarsToColorsEditor.h>

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
#include <vtkRenderWindow.h>
#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkControlPointsItem.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkRenderer.h>
#include <vtkScalarsToColors.h>
#include <vtkTable.h>

// TODO
#include <MuratUtil.h>

template<typename ... Args> struct SELECT
{
  template<typename C, typename R>
  static constexpr auto OVERLOAD_OF(R (C::*pmf)(Args...)) -> decltype(pmf)
  {
    return pmf;
  }
};

class AutoSelectLineEdit: public QLineEdit
{
protected:
  void focusInEvent(QFocusEvent* e)
  {
    QLineEdit::focusInEvent(e);
//    QTimer::singleShot(0, [this]() {selectAll();});
  }
};


// ----------------------------------------------------------------------------
class ctkVTKDiscretizableColorTransferWidgetPrivate
{
  Q_DECLARE_PUBLIC(ctkVTKDiscretizableColorTransferWidget);
protected:
  ctkVTKDiscretizableColorTransferWidget* const q_ptr;

public:
  ctkVTKDiscretizableColorTransferWidgetPrivate(ctkVTKDiscretizableColorTransferWidget& object);

  void setupUi(QWidget* widget);

  static QIcon getColorIcon(const QColor& color);

  ctkVTKScalarsToColorsComboBox* scalarsToColorsSelector;
  vtkSmartPointer<ctkVTKScalarsToColorsEditor> scalarsToColorsEditor;
  vtkSmartPointer<vtkContextView> histogramView;
  vtkSmartPointer<vtkEventQtSlotConnect> eventLink;

  vtkSmartPointer<vtkScalarsToColors> colorTransferFunction;

  vtkSmartPointer<vtkTable> histogramTable;

  vtkPiecewiseFunction* scalarOpacityFunction;

  QToolButton* gradientOpacityButton;

  QVTKWidget* qvtk;

  //Option part
  QToolButton* nanButton;
  QCheckBox* discretizeCheck;
  QSpinBox* classesDiscret;

  QLineEdit* minRange;
  QLineEdit* maxRange;

  double dataRange[2];
  double dataMean;
};

// ----------------------------------------------------------------------------
ctkVTKDiscretizableColorTransferWidgetPrivate
::ctkVTKDiscretizableColorTransferWidgetPrivate(ctkVTKDiscretizableColorTransferWidget& object)
  : q_ptr(&object)
{
  this->scalarsToColorsSelector = nullptr;
  this->scalarOpacityFunction = nullptr;
  this->gradientOpacityButton = nullptr;
  this->qvtk = nullptr;

  //Option part
  this->nanButton = nullptr;
  this->discretizeCheck = nullptr;
  this->classesDiscret = nullptr;

  this->minRange = nullptr;
  this->maxRange = nullptr;

  this->dataRange[0] = VTK_DOUBLE_MAX;
  this->dataRange[1] = VTK_DOUBLE_MIN;
  this->dataMean = 0.;
}

//-----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidgetPrivate::setupUi(QWidget* widget)
{
  Q_Q(ctkVTKDiscretizableColorTransferWidget);
  this->qvtk = new QVTKWidget(q);

  this->scalarsToColorsEditor = vtkSmartPointer<ctkVTKScalarsToColorsEditor>::New();
  this->histogramView = vtkSmartPointer<vtkContextView> ::New();
  this->eventLink = vtkSmartPointer<vtkEventQtSlotConnect>::New();

  this->histogramView->GetScene()->AddItem(this->scalarsToColorsEditor.Get());
  this->histogramView->SetInteractor(this->qvtk->GetInteractor());
  this->qvtk->SetRenderWindow(this->histogramView->GetRenderWindow());

  this->histogramView->GetRenderWindow()->Render();
  this->histogramView->GetRenderer()->SetBackground(MuratUtil::BACKGROUND_COLOR);

  eventLink->Connect(scalarsToColorsEditor.Get(),
    vtkControlPointsItem::CurrentPointEditEvent,
    q, SLOT(onCurrentPointEdit()));
  eventLink->Connect(scalarsToColorsEditor.Get(),
    vtkControlPointsItem::CurrentPointChangedEvent,
    q, SLOT(onCurrentPointChanged()));

  QGridLayout* layout = new QGridLayout;
  layout->setContentsMargins(0, 0, 0, 0);

  this->scalarsToColorsSelector = new ctkVTKScalarsToColorsComboBox(q);
  this->scalarsToColorsSelector->addScalarsToColors(nullptr, q->tr("Reset"));
  QObject::connect(scalarsToColorsSelector, SIGNAL(currentScalarsToColorsChanged(vtkScalarsToColors*)),
    q, SLOT(onPaletteIndexChanged(vtkScalarsToColors*)));
  layout->addWidget(this->scalarsToColorsSelector, 0, 0, 1, 6);

  //add transparency slider
  QLabel* sliderTop = new QLabel("1.0", q);
  layout->addWidget(sliderTop, 1, 0, 1, 1);

  ctkDoubleSlider* slider = new ctkDoubleSlider(Qt::Orientation::Vertical);
  slider->setToolTip("Set global opacity");
  slider->setRange(0.00001, 1.);
  slider->setValue(1.);
  QObject::connect(slider, SIGNAL(valueChanged(double)), q, SLOT(transparencyChanged(double)));
  layout->addWidget(slider, 2, 0, 5, 1);

  QLabel* sliderBottom = new QLabel("0.0", q);
  layout->addWidget(sliderBottom, 7, 0, 1, 1);

  layout->addWidget(this->qvtk, 1, 1, 7, 4);

  QToolButton* resetRangeButton = new QToolButton(q);
  resetRangeButton->setIcon(QIcon(":Icons/resetRange.png"));
  resetRangeButton->setToolTip("Reset to data range");
  QObject::connect(resetRangeButton, SIGNAL(clicked()), q, SLOT(onResetRangeClicked()));
  layout->addWidget(resetRangeButton,1,5,1,1);

  layout->addItem(new QSpacerItem(0, 0), 2, 5);

  QToolButton* centerRangeButton = new QToolButton(q);
  centerRangeButton->setIcon(QIcon(":Icons/resetRangeCustom.png"));
  centerRangeButton->setToolTip("Center current range on median");
  QObject::connect(centerRangeButton, SIGNAL(clicked()), q, SLOT(onCenterRangeClicked()));
  layout->addWidget(centerRangeButton, 3, 5, 1, 1);

  layout->addItem(new QSpacerItem(0, 0), 4, 5);

  QToolButton* invertButton= new QToolButton(q);
  invertButton->setIcon(QIcon(":Icons/invert.png"));
  invertButton->setToolTip("Invert color map");
  QObject::connect(invertButton, SIGNAL(clicked()), q, SLOT(onInvertClicked()));
  layout->addWidget(invertButton, 5, 5, 1, 1);

  layout->addItem(new QSpacerItem(0, 0), 6, 5);

  //option panel part
  //QWidget* optionPanel = new QWidget(q, Qt::Popup);
  //auto panelLayout = new QGridLayout(optionPanel);

  QToolButton* optionButton = new QToolButton(q);
  optionButton->setIcon(q->style()->standardIcon(QStyle::SP_FileDialogDetailedView, nullptr, optionButton));
  optionButton->setToolTip("Other options");
  layout->addWidget(optionButton, 7, 5, 1, 1);

  //QLabel* nanLabel = new QLabel("NaN values :");
  //nanButton = new QToolButton;
  //panelLayout->addWidget(nanLabel, 0, 0);
  //panelLayout->addWidget(nanButton, 0, 1);

  //discretizeCheck = new QCheckBox;
  //discretizeCheck->setText("Discretize");
  //classesDiscret = new QSpinBox;
  //classesDiscret->setMinimum(1);
  //classesDiscret->setMaximum(255);
  //panelLayout->addWidget(discretizeCheck, 1, 0);
  //panelLayout->addWidget(classesDiscret, 1, 1);


  QLabel* minLabel = new QLabel(q->tr("Min"));
  layout->addWidget(minLabel, 8, 0, 1, 2);
  minRange = new AutoSelectLineEdit;
  minRange->setEnabled(false);
  minRange->setFixedSize(100, 25);
  minRange->setValidator(new QDoubleValidator());
  QObject::connect(minRange, SIGNAL(returnPressed()), q, SLOT(onRangeEditorReturn()));
  layout->addWidget(minRange, 8, 2, 1, 1);

  QLabel* maxLabel = new QLabel(q->tr("Max"));
  layout->addWidget(maxLabel, 8, 3, 1, 1);
  maxRange = new AutoSelectLineEdit;
  maxRange->setEnabled(false);
  maxRange->setFixedSize(100, 25);
  maxRange->setValidator(new QDoubleValidator());
  QObject::connect(maxRange, SIGNAL(returnPressed()), q, SLOT(onRangeEditorReturn()));
  layout->addWidget(maxRange, 8, 4, 1, 2);

  layout->setColumnStretch(2, 1);
  layout->setColumnStretch(4, 1);

  //  connect(optionButton, &QPushButton::clicked, [this,optionButton,optionPanel]() {
  //    QPoint buttonPos = mapToGlobal(optionButton->pos());
  //    optionPanel->move(buttonPos.x(),buttonPos.y()+optionButton->height());
  //    optionPanel->setVisible(!optionPanel->isVisible());
  //  });

  //  connect(nanButton, &QToolButton::clicked, [this,optionPanel]() {
  //    double r, g, b;
  //    LUT->GetNanColor(r,g,b);
  //    QColor selected = QColorDialog::getColor(QColor(255*r,255*g,255*b,255),optionPanel, "Select NaN values color",QColorDialog::DontUseNativeDialog);
  //    nanButton->setIcon(getColorIcon(selected));
  //    LUT->SetNanColor(selected.redF(),selected.greenF(),selected.blueF());
  //  });

  //  connect(discretizeCheck, &QCheckBox::stateChanged, [this](int state) {
  //    LUT->SetDiscretize(state==Qt::Checked);
  //    histogramView->GetRenderWindow()->Render();
  //  });
  //  connect(classesDiscret, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged), [this](int value) {
  //    LUT->SetNumberOfValues(value);
  //    histogramView->GetRenderWindow()->Render();
  //  });

  q->setLayout(layout);
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
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::setHistogramTable(vtkTable* hTable)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->histogramTable = hTable;
  d->scalarsToColorsEditor->SetHistogramTable(hTable,
    "image_extents", "Frequency");

  d->qvtk->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onScalarOpacityFunctionChanged()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  // Update the histogram
  d->qvtk->GetInteractor()->Render();
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
  d->qvtk->GetRenderWindow()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onHistogramDataModified(vtkTable* hTable)
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  d->scalarsToColorsEditor->SetHistogramTable(hTable,
    "image_extents", "Frequency");

  d->qvtk->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::transparencyChanged(double value)
{
  //MuratUtil::setTransparency(LUT, value);
  onScalarOpacityFunctionChanged();
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
    if (color.isValid()) {
      rgb[0] = color.redF();
      rgb[1] = color.greenF();
      rgb[2] = color.blueF();
      d->scalarsToColorsEditor->SetCurrentControlPointColor(rgb);
      onScalarOpacityFunctionChanged();
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
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onInvertClicked()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  //MuratUtil::reverseColorMap(LUT);
  d->qvtk->GetInteractor()->Render();
}

// ----------------------------------------------------------------------------
void ctkVTKDiscretizableColorTransferWidget::onRangeEditorReturn()
{
  Q_D(ctkVTKDiscretizableColorTransferWidget);
  double min = d->minRange->text().toDouble();
  double max = d->maxRange->text().toDouble();
  d->scalarsToColorsEditor->SetCurrentRange(min, max);
}


