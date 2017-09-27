/*
 * MuratUtil.cxx
 *
 *  Created on: 3 avr. 2017
 *      Author: a
 */

#include "MuratUtil.h"

#include <QFileInfo>
#include <vtkDiscretizableColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>

int MuratUtil::PALETTE_WIDTH = 255;

double MuratUtil::BACKGROUND_COLOR[3] = { 49.0 / 256, 54.0 / 256, 59.0 / 256 };

MuratUtil::MuratUtil() {
	// TODO Auto-generated constructor stub

}

MuratUtil::~MuratUtil() {
	// TODO Auto-generated destructor stub
}

std::string MuratUtil::getNameFromFile(const std::string & path) {
	QFileInfo f(path.c_str());
	return f.fileName().toStdString();
}

void MuratUtil::remapColorScale(vtkDiscretizableColorTransferFunction* colorTransferFunction,
        vtkDiscretizableColorTransferFunction* rescaledColorTransferFunction, double minRescale, double maxRescale) {
	//The code for internal and external colortables could be in the same loops but to make
	// this clear about having or not set the pointer, we do it like this
	//Color
	double rescaleWidth = maxRescale - minRescale;
	double * r = colorTransferFunction->GetRange();
	//Map to the histogram color map and to the external
//	rescaledColorTransferFunction->SetEnableModifiedEvents(false);
    rescaledColorTransferFunction->RemoveAllPoints();

  for (int i = 0; i < colorTransferFunction->GetSize(); i++) {
    double val[6];
    colorTransferFunction->GetNodeValue(i, val);
    double normalized = (val[0] - r[0]) / (r[1] - r[0]);

    double newPostHisto = minRescale + normalized * rescaleWidth;
    rescaledColorTransferFunction->AddRGBPoint(newPostHisto, val[1], val[2], val[3], val[4], val[5]);
  }

  //Opacity
    if(rescaledColorTransferFunction->GetScalarOpacityFunction())
    {
      rescaledColorTransferFunction->GetScalarOpacityFunction()->RemoveAllPoints();
    }
    else
    {
      vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();
      rescaledColorTransferFunction->SetScalarOpacityFunction(opacityFunction);
    }

    if(!colorTransferFunction->GetScalarOpacityFunction())
    {
      vtkSmartPointer<vtkPiecewiseFunction> opacityFunction =
        vtkSmartPointer<vtkPiecewiseFunction>::New();
      colorTransferFunction->SetScalarOpacityFunction(opacityFunction);
    }

  for (int i = 0; i < colorTransferFunction->GetScalarOpacityFunction()->GetSize(); i++) {
    double val[4];
    colorTransferFunction->GetScalarOpacityFunction()->GetNodeValue(i, val);
    double normalized = (val[0] - r[0]) / (r[1] - r[0]);

    double newPostHisto = minRescale + normalized * rescaleWidth;
    rescaledColorTransferFunction->GetScalarOpacityFunction()->AddPoint(newPostHisto, val[1], val[2], val[3]);
  }
//	rescaledColorTransferFunction->SetDiscretize(colorTransferFunction->GetDiscretize());
//	rescaledColorTransferFunction->SetNumberOfValues(colorTransferFunction->GetNumberOfValues());

//	rescaledColorTransferFunction->SetEnableModifiedEvents(true);
	rescaledColorTransferFunction->Build();
}
/**
 * Simple copy from one to another
 */
void MuratUtil::remapColorScale(vtkDiscretizableColorTransferFunction* colorTransferFunction, double minRescale, double maxRescale) {
    vtkSmartPointer<vtkDiscretizableColorTransferFunction> rescaled = vtkSmartPointer<vtkDiscretizableColorTransferFunction>::New();
	remapColorScale(colorTransferFunction, rescaled, minRescale, maxRescale);
  remapColorScale(rescaled, colorTransferFunction, minRescale, maxRescale);
}

/**
 * Simply swap values
 */
void MuratUtil::reverseColorMap(vtkDiscretizableColorTransferFunction* ctf){
	int size = ctf->GetSize();
//	ctf->SetEnableModifiedEvents(false);
	for (int i = 0; i < size/2; i++) {
		double val[6];
		ctf->GetNodeValue(i, val);

		double valRev[6];
		ctf->GetNodeValue(size-1-i,valRev);

		std::swap(val[0],valRev[0]);

		ctf->SetNodeValue(i,valRev);
		ctf->SetNodeValue(size-1-i,val);
	}
//	ctf->SetEnableModifiedEvents(true);
	ctf->Modified();
}

void MuratUtil::setTransparency(vtkDiscretizableColorTransferFunction* ctf, double transparency){
	//Opacity
//	ctf->SetEnableModifiedEvents(false);
	for (int i = 0; i < ctf->GetScalarOpacityFunction()->GetSize(); i++) {
		double val[4];
		ctf->GetScalarOpacityFunction()->GetNodeValue(i, val);
		val[1]=transparency;
		ctf->GetScalarOpacityFunction()->SetNodeValue(i,val);
	}
//	ctf->SetEnableModifiedEvents(true);
	ctf->Modified();
}
