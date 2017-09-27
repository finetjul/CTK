/******************************************************************************

 This source file is part of the tomviz project.

 Copyright Kitware, Inc.

 This source code is released under the New BSD License, (the "License").

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.

 ******************************************************************************/
//#include <init/ColorTableRegistry.h>
#include "ctkVTKDiscretizableColorTransferWidget.h"

#include <vtkContextScene.h>
#include <vtkContextView.h>
#include <vtkControlPointsItem.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkPiecewiseFunction.h>
#include <vtkRenderWindow.h>
#include <vtkVector.h>

#include <QTimer>
#include <QColorDialog>
#include <QHBoxLayout>
#include <QToolButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSlider>
#include <QLabel>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QComboBox>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QPainter>
#include <QIcon>

#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <QVTKInteractor.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <vtkDiscretizableColorTransferFunction.h>
#include <QDebug>

#include <QVTKWidget.h>
#include <ctkVTKScalarsToColorsComboBox.h>
#include <ctkVTKScalarsToColorsEditor.h>
#include <vtkSmartPointer.h>
#include <vtkDoubleArray.h>
#include <vtkChartXY.h>
#include <vtkTable.h>
#include <vtkPlot.h>
#include <vtkContextView.h>
#include <vtkContextScene.h>

#include "MuratUtil.h"
#include <vtkIntArray.h>
#include <vtkImageAccumulate.h>
#include <vtkImageData.h>
#include <vtkRenderer.h>
#include <vtkContext2D.h>

#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkJPEGReader.h>
#include <vtkDataSetMapper.h>

template<typename ... Args> struct SELECT {
	template<typename C, typename R>
	static constexpr auto OVERLOAD_OF(R (C::*pmf)(Args...)) -> decltype(pmf) {
		return pmf;
	}
};

class AutoSelectLineEdit: public QLineEdit {
protected:
	void focusInEvent(QFocusEvent* e) {
		QLineEdit::focusInEvent(e);
//		QTimer::singleShot(0, [this]() {selectAll();});
	}
};

ctkVTKDiscretizableColorTransferWidget::ctkVTKDiscretizableColorTransferWidget(QWidget* parent) :
		QWidget(parent), qvtk(new QVTKWidget(this))
{
  histogramView->GetScene()->AddItem(scalarsToColorsEditor.Get());

  histogramView->SetInteractor(qvtk->GetInteractor());
  qvtk->SetRenderWindow(histogramView->GetRenderWindow());

  histogramView->GetRenderWindow()->Render();

  histogramView->GetRenderer()->SetBackground(MuratUtil::BACKGROUND_COLOR);

  eventLink->Connect(scalarsToColorsEditor.Get(), vtkControlPointsItem::CurrentPointEditEvent, this, SLOT(onCurrentPointEdit()));
  eventLink->Connect(scalarsToColorsEditor.Get(), vtkControlPointsItem::CurrentPointChangedEvent, this, SLOT(onCurrentPointChanged()));

	auto hLayout = new QHBoxLayout;
	auto vLayoutLeft = new QVBoxLayout;

  scalarsToColorsSelector = new ctkVTKScalarsToColorsComboBox();
  scalarsToColorsSelector->setFixedHeight(25);
  scalarsToColorsSelector->setDefaultText("Select color transfer function ...");
  scalarsToColorsSelector->setCurrentIndex(-1);//Start with default

	//add transparency slider
	QSlider* slider = new QSlider(Qt::Orientation::Vertical);
	slider->setToolTip("Set global opacity");
	slider->setRange(0, 100);

	QLabel* sliderTop = new QLabel("1.0");
	QLabel* sliderBottom = new QLabel("0.0");

	vLayoutLeft->addWidget(sliderTop, 0, Qt::AlignHCenter);
	vLayoutLeft->addWidget(slider, 0, Qt::AlignHCenter);
	vLayoutLeft->addWidget(sliderBottom, 0, Qt::AlignHCenter);

	connect(slider, SIGNAL(valueChanged(int)), this, SLOT(transparencyChanged(int)));

	auto vLayoutRight = new QVBoxLayout;

	hLayout->addLayout(vLayoutLeft);
	hLayout->addWidget(qvtk);
	hLayout->addLayout(vLayoutRight);
	hLayout->setContentsMargins(0, 0, 0, 0);

	vLayoutRight->setContentsMargins(0, 0, 0, 0);
	vLayoutRight->addStretch(1);

	auto button = new QToolButton;
	button->setIcon(QIcon("D:/Total/Data/pqResetRange.png"));
	button->setToolTip("Reset to data range");
	connect(button, SIGNAL(clicked()), this, SLOT(onResetRangeClicked()));
	vLayoutRight->addWidget(button, Qt::AlignCenter);

	button = new QToolButton;
	button->setIcon(QIcon("D:/Total/Data/pqResetRangeCustom.png"));
	button->setToolTip("Center current range on median");
	connect(button, SIGNAL(clicked()), this, SLOT(onCenterRangeClicked()));
	vLayoutRight->addWidget(button, Qt::AlignCenter);

	button = new QToolButton;
	button->setIcon(QIcon("D:\Total\Data\pqInvert.png"));
	button->setToolTip("Invert color map");
	connect(button, SIGNAL(clicked()), this, SLOT(onInvertClicked()));
	vLayoutRight->addWidget(button, Qt::AlignCenter);

	//option panel part
	QWidget* optionPanel = new QWidget(this, Qt::Popup);
	auto panelLayout = new QGridLayout(optionPanel);

	auto optionButton = new QToolButton;
	optionButton->setIcon(QIcon("D:/Total/Data/pqOptions.png"));
	optionButton->setToolTip("Other options");

	vLayoutRight->addWidget(optionButton, Qt::AlignCenter);

	QLabel* nanLabel = new QLabel("NaN values :");
	nanButton = new QToolButton;
	panelLayout->addWidget(nanLabel, 0, 0);
	panelLayout->addWidget(nanButton, 0, 1);

	discretizeCheck = new QCheckBox;
	discretizeCheck->setText("Discretize");
	classesDiscret = new QSpinBox;
	classesDiscret->setMinimum(1);
	classesDiscret->setMaximum(255);
	panelLayout->addWidget(discretizeCheck, 1, 0);
	panelLayout->addWidget(classesDiscret, 1, 1);

	vLayoutRight->addStretch(1);

	QLabel* minLabel = new QLabel("Min");
	minRange = new AutoSelectLineEdit;
	minRange->setEnabled(false);
	minRange->setFixedSize(100, 25);
	minRange->setValidator(new QDoubleValidator());

	QLabel* maxLabel = new QLabel("Max");
	maxRange = new AutoSelectLineEdit;
	maxRange->setEnabled(false);
	maxRange->setFixedSize(100, 25);
	maxRange->setValidator(new QDoubleValidator());

//	connect(optionButton, &QPushButton::clicked, [this,optionButton,optionPanel]() {
//		QPoint buttonPos = mapToGlobal(optionButton->pos());
//		optionPanel->move(buttonPos.x(),buttonPos.y()+optionButton->height());
//		optionPanel->setVisible(!optionPanel->isVisible());
//	});

//	connect(nanButton, &QToolButton::clicked, [this,optionPanel]() {
//		double r, g, b;
//		LUT->GetNanColor(r,g,b);
//		QColor selected = QColorDialog::getColor(QColor(255*r,255*g,255*b,255),optionPanel, "Select NaN values color",QColorDialog::DontUseNativeDialog);
//		nanButton->setIcon(getColorIcon(selected));
//		LUT->SetNanColor(selected.redF(),selected.greenF(),selected.blueF());
//	});

//	connect(discretizeCheck, &QCheckBox::stateChanged, [this](int state) {
//		LUT->SetDiscretize(state==Qt::Checked);
//		histogramView->GetRenderWindow()->Render();
//	});
//	connect(classesDiscret, SELECT<int>::OVERLOAD_OF(&QSpinBox::valueChanged), [this](int value) {
//		LUT->SetNumberOfValues(value);
//		histogramView->GetRenderWindow()->Render();
//	});
	connect(minRange, SIGNAL(returnPressed()), this, SLOT(onRangeEditorReturn()));
	connect(maxRange, SIGNAL(returnPressed()), this, SLOT(onRangeEditorReturn()));
  connect(scalarsToColorsSelector, SIGNAL(currentScalarsToColorsChanged(vtkScalarsToColors*)),
    this, SLOT(onPaletteIndexChanged(vtkScalarsToColors*)));

	auto hRangeLayout = new QHBoxLayout;
	hRangeLayout->addWidget(minLabel);
	hRangeLayout->addWidget(minRange);
	hRangeLayout->addStretch();
	hRangeLayout->addWidget(maxLabel);
	hRangeLayout->addWidget(maxRange);

	auto vGlobalLayout = new QVBoxLayout(this);

	vGlobalLayout->addWidget(scalarsToColorsSelector);
	vGlobalLayout->addLayout(hLayout);
	vGlobalLayout->addLayout(hRangeLayout);

  setLayout(vGlobalLayout);
}

void ctkVTKDiscretizableColorTransferWidget::SetColorTransferFunction(vtkScalarsToColors* ctf)
{
  this->colorTransferFunction = ctf;
  this->scalarsToColorsEditor->SetColorTransfertFunction(ctf);
}

void ctkVTKDiscretizableColorTransferWidget::SetHistogramTable(vtkTable* hTable)
{
  this->histogramTable = hTable;
  this->scalarsToColorsEditor->SetHistogramTable(hTable,
    "image_extents", "Frequency");

  qvtk->GetRenderWindow()->Render();
}

void ctkVTKDiscretizableColorTransferWidget::onScalarOpacityFunctionChanged()
{
  std::cout << "onScalarOpacityFunctionChanged" << std::endl;
  // Update the histogram
  qvtk->GetRenderWindow()->Render();
}

void ctkVTKDiscretizableColorTransferWidget::onPaletteIndexChanged(vtkScalarsToColors* ctf)
{
  //Setting the transfer function to the scalarsToColorsEditor results
  // in converting ctf to a vtkDiscretizableTransferFunction
  this->scalarsToColorsEditor->SetColorTransfertFunction(ctf);

  //emit signal after the conversion
  emit(currentScalarsToColorsChanged(this->scalarsToColorsEditor->GetColorTransfertFunction()));

  //Update rendering
  qvtk->GetRenderWindow()->Render();
}

void ctkVTKDiscretizableColorTransferWidget::onHistogramDataModified(vtkTable* hTable)
{
  this->scalarsToColorsEditor->SetHistogramTable(hTable,
    "image_extents", "Frequency");

  qvtk->GetRenderWindow()->Render();
}

void ctkVTKDiscretizableColorTransferWidget::transparencyChanged(int value100) {
	//MuratUtil::setTransparency(LUT, ((double) value100) / 100.0);
	onScalarOpacityFunctionChanged();
}

QIcon ctkVTKDiscretizableColorTransferWidget::getColorIcon(QColor color) {
	QPixmap pix(32, 32);
	pix.fill(color);
	return QIcon(pix);
}

void ctkVTKDiscretizableColorTransferWidget::onCurrentPointChanged()
{
}

void ctkVTKDiscretizableColorTransferWidget::onCurrentPointEdit() {
	double rgb[3];
	if (scalarsToColorsEditor->GetCurrentControlPointColor(rgb))
  {
		QColor color = QColorDialog::getColor(QColor::fromRgbF(rgb[0], rgb[1], rgb[2]), this, "Select Color for Control Point",
				QColorDialog::DontUseNativeDialog);
		if (color.isValid()) {
			rgb[0] = color.redF();
			rgb[1] = color.greenF();
			rgb[2] = color.blueF();
			scalarsToColorsEditor->SetCurrentControlPointColor(rgb);
			onScalarOpacityFunctionChanged();
		}
	}
}

void ctkVTKDiscretizableColorTransferWidget::onCurrentPointModified()
{
  emit(currentScalarsToColorsModified());
}

void ctkVTKDiscretizableColorTransferWidget::onResetRangeClicked() {
  std::cout << "onResetRangeClicked" << std::endl;
	scalarsToColorsEditor->SetCurrentRange(dataRange[0], dataRange[1]);
}

void ctkVTKDiscretizableColorTransferWidget::onCenterRangeClicked() {
  std::cout << "onCenterRangeClicked" << std::endl;
	scalarsToColorsEditor->CenterRange(dataMean);
}

void ctkVTKDiscretizableColorTransferWidget::onInvertClicked() {
  std::cout << "onInvertClicked" << std::endl;
	//MuratUtil::reverseColorMap(LUT);
  qvtk->GetRenderWindow()->Render();
}

void ctkVTKDiscretizableColorTransferWidget::onColorFunctionChanged() {
  std::cout << "onColorFunctionChanged" << std::endl;
	//double range[2];
	//colorTransferFunction->GetRange(range);
	//minRange->setText(QString::number(range[0]));
	//maxRange->setText(QString::number(range[1]));
}

void ctkVTKDiscretizableColorTransferWidget::onRangeEditorReturn() {
  std::cout << "onRangeEditorReturn" << std::endl;
	double min = minRange->text().toDouble();
	double max = maxRange->text().toDouble();
	scalarsToColorsEditor->SetCurrentRange(min, max);
}

struct Delegate: public QStyledItemDelegate {
	static const int ICON_WIDTH = 120;
	static const int ICON_HEIGHT = 20;
	Delegate(QObject* parent = nullptr) :
			QStyledItemDelegate(parent) {
	}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		return QSize(ICON_WIDTH, ICON_HEIGHT);
	}

};

ctkVTKDiscretizableColorTransferWidget::~ctkVTKDiscretizableColorTransferWidget() = default;

