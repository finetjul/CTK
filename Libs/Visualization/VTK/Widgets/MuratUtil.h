/*
 * MuratUtil.h
 *
 *  Created on: 3 avr. 2017
 *      Author: a
 */

#ifndef MURATAPP_MURATUTIL_H_
#define MURATAPP_MURATUTIL_H_

#include <string>
#include <vtkSmartPointer.h>

class vtkDiscretizableColorTransferFunction;

class MuratUtil {
public:
	MuratUtil();
	virtual ~MuratUtil();

	static int PALETTE_WIDTH;

	static double BACKGROUND_COLOR[3];

	static std::string getNameFromFile(const std::string &path);

	static void remapColorScale(
            vtkDiscretizableColorTransferFunction* colorTransferFunction,
            vtkDiscretizableColorTransferFunction* rescaledColorTransferFunction,
			double minRescale, double maxRescale);

    static void remapColorScale(vtkDiscretizableColorTransferFunction* colorTransferFunction, double minRescale, double maxRescale);
    static void reverseColorMap(vtkDiscretizableColorTransferFunction* colorTransferFunction);
    static void setTransparency(vtkDiscretizableColorTransferFunction* colorTransferFunction, double transparency);
};

#endif /* MURATAPP_MURATUTIL_H_ */
