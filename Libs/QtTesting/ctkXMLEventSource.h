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

#ifndef __ctkXMLEventSource_h
#define __ctkXMLEventSource_h

// QT includes
#include <QXmlStreamReader>
#include <QMainWindow>

// QtTesting includes
#include <pqEventSource.h>

// CTKQtTesting includes
#include "ctkQtTestingExport.h"

//-----------------------------------------------------------------------------
class CTK_QTTESTING_EXPORT ctkXMLEventSource : public pqEventSource
{
  Q_OBJECT

public:
  typedef pqEventSource Superclass;

  ctkXMLEventSource(QObject* p);
  ~ctkXMLEventSource();

  virtual void setContent(const QString& xmlfilename);
  int getNextEvent(QString& widget, QString& command, QString&arguments);

  bool settingsRecorded();
  bool restoreApplicationSettings();
  QMap<QString, QString> recoverSettingsFromXML();

protected:
  QXmlStreamReader *XMLStream;
  QMainWindow* MainWindow;
};

#endif // __ctkXMLEventSource_h
