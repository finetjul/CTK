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

// QT includes
#include <QCoreApplication>
#include <QDebug>
#include <QVariant>

// CTKQtTesting includes
#include "ctkXMLEventObserver.h"

//-----------------------------------------------------------------------------
// ctkXMLEventObserver methods

//-----------------------------------------------------------------------------
ctkXMLEventObserver::ctkXMLEventObserver(QObject* p)
  : pqEventObserver(p)
{
  this->XMLStream = NULL;
  this->MainWindow = qobject_cast<QMainWindow*>(p);
}

//-----------------------------------------------------------------------------
ctkXMLEventObserver::~ctkXMLEventObserver()
{
  delete this->XMLStream;
  this->MainWindow = 0;
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::recordApplicationSettings()
{
  // We save the MainWindow state in the xml for the next playback
  if(this->XMLStream)
    {
    this->XMLStream->writeStartElement("settings");

    this->recordApplicationSetting("name","qApp", "applicationName",
                                   QCoreApplication::applicationName());

    this->recordApplicationSetting("version" , "qApp", "applicationVersion",
                                   QCoreApplication::applicationVersion());

    if(this->MainWindow)
      {
      this->recordApplicationSetting("font" , "MainWindow", "font",
                                     QVariant(this->MainWindow->font()).toString());

      this->recordApplicationSetting("geometry" , "MainWindow", "mainWindowGeometry",
                                     QString(this->MainWindow->saveGeometry().toHex()));

      this->recordApplicationSetting("state" , "MainWindow", "mainWindowState",
                                     QString(this->MainWindow->saveState().toHex()));
      }

    this->XMLStream->writeEndElement();
    }
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::recordApplicationSetting(const QString &startElement,
                                                   const QString &attribute1,
                                                   const QString &attribute2,
                                                   const QString &attribute3)
{
  this->XMLStream->writeStartElement(startElement);
  this->XMLStream->writeAttribute("widget", attribute1);
  this->XMLStream->writeAttribute("command", attribute2);
  this->XMLStream->writeAttribute("arguments", attribute3);
  this->XMLStream->writeEndElement();
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::setStream(QTextStream* stream)
{
  if (this->XMLStream)
    {
    this->XMLStream->writeEndElement();
    this->XMLStream->writeEndElement();
    this->XMLStream->writeEndDocument();
    delete this->XMLStream;
    this->XMLStream = NULL;
    }
  if (this->Stream)
    {
    *this->Stream << this->XMLString;
    }
  this->XMLString = QString();
  pqEventObserver::setStream(stream);
  if (this->Stream)
    {
    this->XMLStream = new QXmlStreamWriter(&this->XMLString);
    this->XMLStream->setAutoFormatting(true);
    this->XMLStream->writeStartDocument();
    this->XMLStream->writeStartElement("xml");
    this->recordApplicationSettings();
    this->XMLStream->writeStartElement("events");
    }
}

//-----------------------------------------------------------------------------
void ctkXMLEventObserver::onRecordEvent(const QString& widget,
                                     const QString& command,
                                     const QString& arguments)
{
  if(this->XMLStream)
    {
    this->XMLStream->writeStartElement("event");
    this->XMLStream->writeAttribute("widget", widget);
    this->XMLStream->writeAttribute("command", command);
    this->XMLStream->writeAttribute("arguments", arguments);
    this->XMLStream->writeEndElement();
    }
}
