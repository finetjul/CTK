/*=========================================================================

  Library:   CTK

  Copyright (c) 2010  Kitware Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.commontk.org/LICENSE

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=========================================================================*/

// CTK includes
#include "ctkVTKScalarsToColorsView.h"
#include "ctkVTKScalarsToColorsViewPlugin.h"

//-----------------------------------------------------------------------------
ctkVTKScalarsToColorsViewPlugin::ctkVTKScalarsToColorsViewPlugin(QObject *_parent):QObject(_parent)
{
}

//-----------------------------------------------------------------------------
QWidget *ctkVTKScalarsToColorsViewPlugin::createWidget(QWidget *parentWidget)
{
  ctkVTKScalarsToColorsView* newWidget = new ctkVTKScalarsToColorsView(parentWidget);
  return newWidget;
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarsToColorsViewPlugin::domXml() const
{
  return "<widget class=\"ctkVTKScalarsToColorsView\" \
          name=\"VTKScalarBar\">\n"
          "</widget>\n";
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarsToColorsViewPlugin::includeFile() const
{
  return "ctkVTKScalarsToColorsView.h";
}

//-----------------------------------------------------------------------------
bool ctkVTKScalarsToColorsViewPlugin::isContainer() const
{
  return false;
}

//-----------------------------------------------------------------------------
QString ctkVTKScalarsToColorsViewPlugin::name() const
{
  return "ctkVTKScalarsToColorsView";
}
