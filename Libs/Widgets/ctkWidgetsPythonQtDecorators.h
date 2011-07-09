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

#ifndef __ctkWidgetsPythonQtDecorators_h
#define __ctkWidgetsPythonQtDecorators_h

// Qt includes
#include <QObject>

// PythonQt includes
#include <PythonQt.h>

// CTK includes
#include <ctkWorkflowWidgetStep.h>

#include "ctkWidgetsExport.h"

// NOTE:
//
// For decorators it is assumed that the methods will never be called
// with the self argument as NULL.  The self argument is the first argument
// for non-static methods.
//

class CTK_WIDGETS_EXPORT ctkWidgetsPythonQtDecorators : public QObject
{
  Q_OBJECT
public:

  ctkWidgetsPythonQtDecorators()
    {
    PythonQt::self()->addParentClass("ctkWorkflowWidgetStep", "ctkWorkflowStep",
                                     PythonQtUpcastingOffset<ctkWorkflowWidgetStep,ctkWorkflowStep>());
    }

public slots:

  bool hasCreateUserInterfaceCommand(ctkWorkflowWidgetStep* step)const
    {
    return step->hasCreateUserInterfaceCommand();
    }

  void setHasCreateUserInterfaceCommand(
    ctkWorkflowWidgetStep* step, bool newHasCreateUserInterfaceCommand)
    {
    step->setHasCreateUserInterfaceCommand(newHasCreateUserInterfaceCommand);
    }

  bool hasShowUserInterfaceCommand(ctkWorkflowWidgetStep* step)const
    {
    return step->hasShowUserInterfaceCommand();
    }

  void setHasShowUserInterfaceCommand(
    ctkWorkflowWidgetStep* step, bool newHasShowUserInterfaceCommand)
    {
    step->setHasShowUserInterfaceCommand(newHasShowUserInterfaceCommand);
    }
};

#endif
