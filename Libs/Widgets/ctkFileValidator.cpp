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
#include <QDebug>
#include <QFileInfo>
#include <QRegExpValidator>

// CTK includes
#include "ctkFileValidator.h"
#include "ctkUtils.h"

//-----------------------------------------------------------------------------
class ctkFileValidatorPrivate
{
  Q_DECLARE_PUBLIC(ctkFileValidator);

protected:
  ctkFileValidator* const q_ptr;

public:
  ctkFileValidatorPrivate(ctkFileValidator& object);
  void init();

  QRegExpValidator*     NameValidator;

  QStringList           NameFilters;        //!< Regular expression (in wildcard mode) used to help the user to complete the line
  QDir::Filters         Filters;            //!< Type of path (file, dir...)
  bool                  MustExists;
};

//-----------------------------------------------------------------------------
ctkFileValidatorPrivate::ctkFileValidatorPrivate(ctkFileValidator& object)
  :q_ptr(&object)
{
  this->NameValidator = 0;
  this->Filters = QDir::AllEntries|QDir::NoDotAndDotDot|QDir::Readable;
  this->MustExists = false;
}

//-----------------------------------------------------------------------------
void ctkFileValidatorPrivate::init()
{
  Q_Q(ctkFileValidator);
  this->NameValidator = new QRegExpValidator(q);
}

//-----------------------------------------------------------------------------
ctkFileValidator::ctkFileValidator(QObject* parentObject)
  : QValidator(parentObject)
  , d_ptr(new ctkFileValidatorPrivate(*this))
{
  Q_D(ctkFileValidator);
  d->init();
}

//-----------------------------------------------------------------------------
ctkFileValidator::ctkFileValidator(const QStringList& nameFilters,
                                   ctkPathLineEdit::Filters filters,
                                   bool mustExists,
                                   QObject *parentObject)
  : QValidator(parentObject)
  , d_ptr(new ctkFileValidatorPrivate(*this))
{
  Q_D(ctkFileValidator);
  d->init();

  this->setNameFilters(nameFilters);
  this->setFilters(filters);
  this->setMustExists(mustExists);
}

//-----------------------------------------------------------------------------
ctkFileValidator::~ctkFileValidator()
{
}

//-----------------------------------------------------------------------------
void ctkFileValidator::setNameFilters(const QStringList &nameFilters)
{
  Q_D(ctkFileValidator);
  d->NameFilters = nameFilters;
  d->NameValidator->setRegExp(ctk::nameFiltersToRegExp(d->NameFilters));
}

//-----------------------------------------------------------------------------
const QStringList& ctkFileValidator::nameFilters()const
{
  Q_D(const ctkFileValidator);
  return d->NameFilters;
}

//-----------------------------------------------------------------------------
void ctkFileValidator::setFilters(const ctkPathLineEdit::Filters &filters)
{
  Q_D(ctkFileValidator);
  d->Filters = QFlags<QDir::Filter>(static_cast<int>(filters));
}

//-----------------------------------------------------------------------------
ctkPathLineEdit::Filters ctkFileValidator::filters()const
{
  Q_D(const ctkFileValidator);
  return QFlags<ctkPathLineEdit::Filter>(static_cast<int>(d->Filters));
}

//-----------------------------------------------------------------------------
void ctkFileValidator::setMustExists(bool mustExists)
{
  Q_D(ctkFileValidator);
  d->MustExists = mustExists;
}

//-----------------------------------------------------------------------------
bool ctkFileValidator::mustExists()const
{
  Q_D(const ctkFileValidator);
  return d->MustExists;
}

//-----------------------------------------------------------------------------
QValidator::State ctkFileValidator::validate(QString& input, int& pos)const
{
  Q_D(const ctkFileValidator);
  QValidator::State res = d->NameValidator->validate(input, pos);
  if (res == QValidator::Acceptable)
    {
    QFileInfo fileInfo(input);
    if ( d->MustExists && !fileInfo.exists() )
      {
      return QValidator::Intermediate;
      }
    bool canBeFile = (d->Filters & QDir::Files);
    bool canBeDir = (d->Filters & QDir::Dirs);
    if ((!canBeFile && fileInfo.isFile()) ||
        (!canBeDir && fileInfo.isDir()) )
      {
      return QValidator::Invalid;
      }
    // add here more filtering
    }
  return res;
}
