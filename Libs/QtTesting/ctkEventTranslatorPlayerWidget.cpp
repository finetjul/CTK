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
#include <QDebug>
#include <QFileDialog>
#include <QFile>
#include <QMetaEnum>
#include <QTextStream>
#include <QVBoxLayout>

// CTKTesting includes
#include "ctkCallback.h"
#include "ctkEventTranslatorPlayerWidget.h"
#include "ctkQtTestingUtility.h"
#include "ctkXMLEventObserver.h"
#include "ctkXMLEventSource.h"

// Standard include
#include <cmath>

//-----------------------------------------------------------------------------
ctkEventTranslatorPlayerWidget::ctkEventTranslatorPlayerWidget()
{
  this->Ui.setupUi(this);

  QObject::connect(Ui.TranslatorButton, SIGNAL(clicked(bool)),
                   this, SLOT(onClickedRecord(bool)));
  QObject::connect(Ui.PlayerButton, SIGNAL(clicked(bool)),
                   this, SLOT(onClickedPlayback(bool)));
  QObject::connect(Ui.TestCaseComboBox, SIGNAL(currentIndexChanged(int)),
                   this, SLOT(switchTestCase(int)));

  this->TestUtility = new pqTestUtility(this);
  this->TestUtility->addEventObserver("xml", new ctkXMLEventObserver(this));
  this->TestUtility->addEventSource("xml", new ctkXMLEventSource(this));

  this->currentWidget = 0;
}

//-----------------------------------------------------------------------------
ctkEventTranslatorPlayerWidget::~ctkEventTranslatorPlayerWidget()
{
  this->TestUtility = 0;
  delete this->TestUtility;
  delete this->currentWidget;
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::addTestCase(QWidget *widget,
                                                 QString fileName,
                                                 void (*newCallback)(void * data))
{
  this->TestCase.push_back(new InfoTestCase);

  this->TestCase[this->TestCase.count() - 1]->Widget = widget;
  this->TestCase[this->TestCase.count() - 1]->FileName = fileName;
  this->TestCase[this->TestCase.count() - 1]->Callback =
      new ctkCallback(newCallback);
  this->TestCase[this->TestCase.count() - 1]->Callback->setCallbackData(widget);

  this->TestCase[this->TestCase.count() - 1]->Dialog = false;

  this->Ui.stackedWidget->addWidget(widget);
  this->Ui.TestCaseComboBox->addItem(QString::number(this->TestCase.count()),
                                   QVariant(this->TestCase.count()));
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::addTestCase(QDialog *dialog,
                                                 QString fileName,
                                                 void (*newCallback)(void * data))
{
  this->TestCase.push_back(new InfoTestCase);

  this->TestCase[this->TestCase.count() - 1]->Widget = dialog;
  this->TestCase[this->TestCase.count() - 1]->FileName = fileName;
  this->TestCase[this->TestCase.count() - 1]->Callback =
      new ctkCallback(newCallback);
  this->TestCase[this->TestCase.count() - 1]->Callback->setCallbackData(dialog);

  this->TestCase[this->TestCase.count() - 1]->Dialog = false;

//  QVBoxLayout* layout = new QVBoxLayout();
  QPushButton* button = new QPushButton("Open the Dialog");
  connect(button, SIGNAL(clicked(bool)), this, SLOT(popupDialog()));

  this->Ui.stackedWidget->addWidget(button);
  this->Ui.TestCaseComboBox->addItem(QString::number(this->TestCase.count()),
                                   QVariant(this->TestCase.count()));
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::setTestUtility(pqTestUtility* newTestUtility)
{
  this->TestUtility = newTestUtility;
  this->TestUtility->addEventObserver("xml", new ctkXMLEventObserver(this));
  this->TestUtility->addEventSource("xml", new ctkXMLEventSource(this));
}

//-----------------------------------------------------------------------------
pqTestUtility* ctkEventTranslatorPlayerWidget::testUtility() const
{
  return this->TestUtility;
}

//-----------------------------------------------------------------------------
void  ctkEventTranslatorPlayerWidget::addWidgetEventPlayer(
  pqWidgetEventPlayer* player)
{
  this->TestUtility->eventPlayer()->addWidgetEventPlayer(player);
}

//-----------------------------------------------------------------------------
void  ctkEventTranslatorPlayerWidget::addWidgetEventTranslator(
  pqWidgetEventTranslator* translator)
{
  this->TestUtility->eventTranslator()->addWidgetEventTranslator(translator);
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::record(int currentTestCase)
{
  if(this->TestCase.count() == 0 ||
     currentTestCase > this->TestCase.count() - 1)
    {
    qWarning() << "Problem with the test case. Please verify that you added a test case.";
    return;
    }

  // We load the xml and check if it is different form the other test case
  QFile* filexml = new QFile(this->TestCase[currentTestCase]->FileName);
  if (!filexml->open(QIODevice::ReadWrite))
    {
    qWarning() << "The file .xml was not created";
    return;
    }
  for(int i = 0 ; i < currentTestCase && i != currentTestCase; i++)
    {
    if (this->TestCase[i]->FileName ==
          this->TestCase[currentTestCase]->FileName)
      {
      qWarning() << "This xml file should already recorded\n";
      return;
      }
    }

  this->TestUtility->recordTests(filexml->fileName());
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::play(int currentTestCase)
{
  if(this->TestCase.count() == 0)
    {
    qWarning() << "No test case had been added. Please add a test case.";
    return false;
    }

  // Connect the slot player done to the the good callback
  QObject::connect(this, SIGNAL(playerDone(QWidget*)),
                   this->TestCase[currentTestCase]->Callback, SLOT(invoke()));

  if (!QFile::exists(this->TestCase[currentTestCase]->FileName))
    {
    qWarning() << "No .xml file for this test case";
    return false;
    }

  if (!this->TestUtility->playTests(QStringList(this->TestCase[currentTestCase]->FileName)))
    {
    qWarning() << "The Test case " << currentTestCase
               << " playback has failed !";
    return false;
    }
  emit this->playerDone(this->TestCase[currentTestCase]->Widget);

  QObject::disconnect(this->TestCase[currentTestCase]->Callback);
  return true;
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::play()
{
  bool success = true;
  for(int i = 0 ; i < this->TestCase.count() ; i++ )
    {
    this->Ui.TestCaseComboBox->setCurrentIndex(i);
    success &= this->play(i);
    }
  QApplication::exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::popupDialog()
{
  QDialog* widget = qobject_cast<QDialog*>(this->TestCase[this->Ui.TestCaseComboBox->currentIndex()]->Widget);
  widget->exec();
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::onClickedPlayback(bool)
{
  this->play(this->Ui.TestCaseComboBox->currentIndex());
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::onClickedRecord(bool)
{
  this->record(this->Ui.TestCaseComboBox->currentIndex());
}

//-----------------------------------------------------------------------------
void ctkEventTranslatorPlayerWidget::switchTestCase(int index)
{
  this->Ui.stackedWidget->setCurrentIndex(index);
}

//-----------------------------------------------------------------------------
const char* enumValueToKey(QObject* object, const char* enumName, int value)
  {
  const QMetaObject * metaObject = object->metaObject();
  QMetaEnum theEnum = metaObject->enumerator(metaObject->indexOfEnumerator(enumName));
  return theEnum.valueToKey(value);
  }

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const double &actual,
                                             const double& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const int &actual,
                                             const int& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QString& actual,
                                             const QString& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QStringList& actual,
                                             const QStringList& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual.join(" ") << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected.join(" ") << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QDateTime& actual,
                                             const QDateTime& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = " << actual.date().toString() << " \n"
        << "\tExpected value : '" << expectedName << "' = " << expected.date().toString() << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QColor& actual,
                                             const QColor& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  if (actual != expected)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - Problem with function " << function << "\n"
        << "\tActual value : '" << actualName << "' = R:" << actual.red() << " G:"<< actual.green() << " B:" << actual.blue() << "\n"
        << "\tExpected value : '" << expectedName << "' = R:" << expected.red() << " G:"<< expected.green() << " B:" << expected.blue()<< endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
//  enumValueToKey(widget, "Axis", currentCurrentAxis)
  return true;
}

//-----------------------------------------------------------------------------
bool ctkEventTranslatorPlayerWidget::compare(const QImage& actual,
                                             const QImage& expected,
                                             const char* actualName,
                                             const char* expectedName,
                                             const char * function, int line)
{
  double totaldiff = 0.0 ; //holds the number of different pixels

  // images are considered the same if both contain a null image
  if (actual.isNull() && expected.isNull())
    {
    return true;
    }
  // images are not the same if one images contains a null image
  if (actual.isNull() || expected.isNull())
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - 1 image is Null " << function << "\n" << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
  // images do not have the same size
  if (actual.size() != expected.size())
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - The 2 Images don't have the same size " << function << "\n"
        << "\tActual value : '" << actualName << "' = W:" << actual.width() << " H:"<< actual.height() << "\n"
        << "\tExpected value : '" << expectedName << "' = W:" << expected.width() << " H:"<< expected.height() << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }

  QImage a = actual.convertToFormat(QImage::Format_ARGB32);
  QImage e = expected.convertToFormat(QImage::Format_ARGB32);

  for ( int y=0; y<a.height(); y++ )
    {
    for ( int x=0; x<a.width(); x++ )
      {
      QRgb actPix = a.pixel(x, y);
      QRgb expPix = e.pixel(x, y);
      if (qAlpha(actPix) == 0 && qAlpha(expPix) == 0)
        {
        continue;
        }
      if (actPix != expPix)
        {
        totaldiff ++;
        }
      }
    }
  totaldiff = (totaldiff * 100)  / (a.width() * a.height());
  if (totaldiff >= 0.01)
    {
    QTextStream(stderr, QIODevice::WriteOnly)
        << "Line " << line << " - The 2 Images have "
        << totaldiff << "% differencies \n" << endl;
    QApplication::exit(EXIT_FAILURE);
    return false;
    }
  return true;
}
