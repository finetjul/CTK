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

// Qt includes
#include <QApplication>
#include <QString>
#include <QStyle>
#include <QStyleOptionSlider>
#include <QTimer>

// CTK includes
#include "ctkVTKPropertyWidget.h"
#include "ctkTest.h"

// VTK includes
#include <vtkProperty.h>

// ----------------------------------------------------------------------------
class ctkVTKPropertyWidgetTester: public QObject
{
  Q_OBJECT
private slots:
  void testGUIEvents();

};

// ----------------------------------------------------------------------------
void ctkVTKPropertyWidgetTester::testGUIEvents()
{
  ctkVTKPropertyWidget propertyWidget(0);
  propertyWidget.show();
  qApp->processEvents();
  propertyWidget.resize(100, 100);
  qApp->processEvents();
  propertyWidget.resize(1, 100);
  qApp->processEvents();
  propertyWidget.resize(100, 1);
  qApp->processEvents();
  propertyWidget.hide();
  qApp->processEvents();
  propertyWidget.show();
}


// ----------------------------------------------------------------------------
CTK_TEST_MAIN(ctkVTKPropertyWidgetTest)
#include "moc_ctkVTKPropertyWidgetTest.cpp"
