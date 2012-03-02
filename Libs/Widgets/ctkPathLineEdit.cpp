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
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QDirModel>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QRegExp>
#include <QRegExpValidator>
#include <QSettings>

// CTK includes
#include "ctkPathLineEdit.h"
#include "ctkUtils.h"

//-----------------------------------------------------------------------------
class ctkPathLineEditPrivate
{
  Q_DECLARE_PUBLIC(ctkPathLineEdit);

protected:
  ctkPathLineEdit* const q_ptr;

public:
  ctkPathLineEditPrivate(ctkPathLineEdit& object);
  void init();
  void updateFilter();

  QComboBox*            ComboBox;

  QString               Label;              //!< used in file dialogs
  QStringList           NameFilters;        //!< Regular expression (in wildcard mode) used to help the user to complete the line
  QDir::Filters         Filters;            //!< Type of path (file, dir...)
#ifdef USE_QFILEDIALOG_OPTIONS
  QFileDialog::Options DialogOptions;
#else
  ctkPathLineEdit::Options DialogOptions;
#endif

  bool                  HasValidInput;      //!< boolean that stores the old state of valid input

  static QString        sCurrentDirectory;   //!< Content the last value of the current directory
  static int            sMaxHistory;     //!< Size of the history, if the history is full and a new value is added, the oldest value is dropped
};

QString ctkPathLineEditPrivate::sCurrentDirectory = "";
int ctkPathLineEditPrivate::sMaxHistory = 5;

//-----------------------------------------------------------------------------
ctkPathLineEditPrivate::ctkPathLineEditPrivate(ctkPathLineEdit& object)
  :q_ptr(&object)
{
  this->ComboBox = 0;
  this->HasValidInput = false;
  this->Filters = QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Readable;
}

//-----------------------------------------------------------------------------
void ctkPathLineEditPrivate::init()
{
  Q_Q(ctkPathLineEdit);
  this->ComboBox = new QComboBox(q);
  QHBoxLayout* layout = new QHBoxLayout(q);
  layout->addWidget(this->ComboBox);
  layout->setContentsMargins(0,0,0,0);

  this->ComboBox->setEditable(true);
  q->setSizePolicy(QSizePolicy(
                     QSizePolicy::Expanding, QSizePolicy::Fixed,
                     QSizePolicy::LineEdit));

  QObject::connect(this->ComboBox,SIGNAL(editTextChanged(QString)),
                   q, SLOT(setCurrentDirectory(QString)));
  QObject::connect(this->ComboBox,SIGNAL(editTextChanged(QString)),
                   q, SLOT(updateHasValidInput()));
}

//-----------------------------------------------------------------------------
void ctkPathLineEditPrivate::updateFilter()
{
  Q_Q(ctkPathLineEdit);
  // help completion for the QComboBox::QLineEdit
  QCompleter *newCompleter = new QCompleter(q);
  newCompleter->setModel(new QDirModel(
                           ctk::nameFiltersToExtensions(this->NameFilters),
                           this->Filters | QDir::NoDotAndDotDot | QDir::AllDirs,
                           QDir::Name|QDir::DirsLast, newCompleter));
  this->ComboBox->setCompleter(newCompleter);
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::ctkPathLineEdit(QWidget *parentWidget)
  : QWidget(parentWidget)
  , d_ptr(new ctkPathLineEditPrivate(*this))
{
  Q_D(ctkPathLineEdit);
  d->init();
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::ctkPathLineEdit(const QString& label,
                                 const QStringList& nameFilters,
                                 Filters filters,
                                 QWidget *parentWidget)
  : QWidget(parentWidget)
  , d_ptr(new ctkPathLineEditPrivate(*this))
{
  Q_D(ctkPathLineEdit);
  d->init();

  this->setLabel(label);
  this->setNameFilters(nameFilters);
  this->setFilters(filters);
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::~ctkPathLineEdit()
{
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::setLabel(const QString &label)
{
  Q_D(ctkPathLineEdit);
  d->Label = label;
}

//-----------------------------------------------------------------------------
const QString& ctkPathLineEdit::label()const
{
  Q_D(const ctkPathLineEdit);
  return d->Label;
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::setNameFilters(const QStringList &nameFilters)
{
  Q_D(ctkPathLineEdit);
  d->NameFilters = nameFilters;
  d->updateFilter();
  d->ComboBox->lineEdit()->setValidator(
    new QRegExpValidator(
      ctk::nameFiltersToRegExp(d->NameFilters), this));
}

//-----------------------------------------------------------------------------
const QStringList& ctkPathLineEdit::nameFilters()const
{
  Q_D(const ctkPathLineEdit);
  return d->NameFilters;
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::setFilters(const Filters &filters)
{
  Q_D(ctkPathLineEdit);
  d->Filters = QFlags<QDir::Filter>(static_cast<int>(filters));
  d->updateFilter();
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::Filters ctkPathLineEdit::filters()const
{
  Q_D(const ctkPathLineEdit);
  return QFlags<ctkPathLineEdit::Filter>(static_cast<int>(d->Filters));
}

//-----------------------------------------------------------------------------
#ifdef USE_QFILEDIALOG_OPTIONS
void ctkPathLineEdit::setOptions(const QFileDialog::Options& dialogOptions)
#else
void ctkPathLineEdit::setOptions(const Options& dialogOptions)
#endif
{
  Q_D(ctkPathLineEdit);
  d->DialogOptions = dialogOptions;
}

//-----------------------------------------------------------------------------
#ifdef USE_QFILEDIALOG_OPTIONS
const QFileDialog::Options& ctkPathLineEdit::options()const
#else
const ctkPathLineEdit::Options& ctkPathLineEdit::options()const
#endif
{
  Q_D(const ctkPathLineEdit);
  return d->DialogOptions;
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::browse()
{
  Q_D(ctkPathLineEdit);
  QString path = "";
  if ( d->Filters & QDir::Files ) //file
    {
    if ( d->Filters & QDir::Writable) // load or save
      {
      path = QFileDialog::getSaveFileName(
	this,
        tr("Select a file to save "),
        this->currentPath().isEmpty() ? ctkPathLineEditPrivate::sCurrentDirectory :
	                                this->currentPath(),
	d->NameFilters.join(";;"),
	0,
#ifdef USE_QFILEDIALOG_OPTIONS
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
      }
    else
      {
      path = QFileDialog::getOpenFileName(
        this,
        QString("Open a file"),
        this->currentPath().isEmpty()? ctkPathLineEditPrivate::sCurrentDirectory :
	                               this->currentPath(),
        d->NameFilters.join(";;"),
	0,
#ifdef USE_QFILEDIALOG_OPTIONS
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
      }
    }
  else //directory
    {
    path = QFileDialog::getExistingDirectory(
      this,
      QString("Select a directory..."),
      this->currentPath().isEmpty() ? ctkPathLineEditPrivate::sCurrentDirectory :
                                      this->currentPath(),
#ifdef USE_QFILEDIALOG_OPTIONS
      d->DialogOptions);
#else
      QFlags<QFileDialog::Option>(int(d->DialogOptions)));
#endif
    }
  if (path.isEmpty())
    {
    return;
    }
  this->setCurrentPath(path);
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::retrieveHistory()
{
  Q_D(ctkPathLineEdit);
  d->ComboBox->clear();
  // fill the combobox using the QSettings
  QSettings settings;
  QString key = "ctkPathLineEdit/" + this->objectName();
  QStringList history = settings.value(key).toStringList();
  foreach(QString path, history)
    {
    d->ComboBox->addItem(path);
    if (d->ComboBox->count() >= ctkPathLineEditPrivate::sMaxHistory)
      {
      break;
      }
    }
  //select the most recent file location
  if (this->currentPath().isEmpty())
    {
    d->ComboBox->setCurrentIndex(0);
    }
}

//-----------------------------------------------------------------------------
void ctkPathLineEdit::addCurrentPathToHistory()
{
  Q_D(ctkPathLineEdit);
  if (this->currentPath().isEmpty())
    {
    return;
    }
  QSettings settings;
  //keep the same values, add the current value
  //if more than m_MaxHistory entrees, drop the oldest.
  QString key = "ctkPathLineEdit/" + this->objectName();
  QStringList history = settings.value(key).toStringList();
  if (history.contains(this->currentPath()))
    {
    history.removeAll(this->currentPath());
    }
  history.push_front(this->currentPath());
  settings.setValue(key, history);
  int index =d->ComboBox->findText(this->currentPath());
  if (index >= 0)
    {
    d->ComboBox->removeItem(index);
    }
  while (d->ComboBox->count() >= ctkPathLineEditPrivate::sMaxHistory)
    {
    d->ComboBox->removeItem(d->ComboBox->count() - 1);
    }
  d->ComboBox->insertItem(0, this->currentPath());
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setCurrentFileExtension(const QString& extension)
{
  QString filename = this->currentPath();
  QFileInfo fileInfo(filename);

  if (!fileInfo.suffix().isEmpty())
    {
    filename.replace(fileInfo.suffix(), extension);
    }
  else
    {
    filename.append(QString(".") + extension);
    }
  this->setCurrentPath(filename);
}

QComboBox* ctkPathLineEdit::comboBox() const
{
  Q_D(const ctkPathLineEdit);
  return d->ComboBox;
}

//------------------------------------------------------------------------------
QString ctkPathLineEdit::currentPath()const
{
  Q_D(const ctkPathLineEdit);
  return d->ComboBox->currentText();
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setCurrentPath(const QString& path)
{
  Q_D(ctkPathLineEdit);
  d->ComboBox->setEditText(path);
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::setCurrentDirectory(const QString& directory)
{
  ctkPathLineEditPrivate::sCurrentDirectory = directory;
}

//------------------------------------------------------------------------------
void ctkPathLineEdit::updateHasValidInput()
{
  Q_D(ctkPathLineEdit);

  bool oldHasValidInput = d->HasValidInput;
  d->HasValidInput = d->ComboBox->lineEdit()->hasAcceptableInput();
  if (d->HasValidInput)
    {
    QFileInfo fileInfo(d->ComboBox->currentText());
    ctkPathLineEditPrivate::sCurrentDirectory =
      fileInfo.isFile() ? fileInfo.absolutePath() : fileInfo.absoluteFilePath();
    qDebug() << "emit currentPathChanged()";
    emit currentPathChanged(d->ComboBox->currentText());
    }
  if ( d->HasValidInput != oldHasValidInput)
    {
    qDebug() << "emit validInputChanged()";
    emit validInputChanged(d->HasValidInput);
    }
}
