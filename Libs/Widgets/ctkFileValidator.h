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
/*=========================================================================

Program:   Maverick
Module:    $RCSfile: config.h,v $

Copyright (c) Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved. No part of this software may be reproduced, distributed,
or modified, in any form or by any means, without permission in writing from
Kitware Inc.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO ANY PARTY FOR
DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
OF THE USE OF THIS SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES, INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE IS PROVIDED ON AN
"AS IS" BASIS, AND THE AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================*/

#ifndef __ctkFileValidator_h
#define __ctkFileValidator_h

// Qt includes
#include <QValidator>

// CTK includes
#include "ctkPathLineEdit.h"
#include "ctkWidgetsExport.h"
class ctkFileValidatorPrivate;

/** 
 * \ingroup Widgets
 * \brief Validator for line edits to validates file path
*/
class CTK_WIDGETS_EXPORT ctkFileValidator: public QValidator
{
  Q_OBJECT

  Q_PROPERTY ( QStringList nameFilters READ nameFilters WRITE setNameFilters)
  Q_PROPERTY ( ctkPathLineEdit::Filters filters READ filters WRITE setFilters)
  Q_PROPERTY ( bool mustExists READ mustExists WRITE setMustExists)

public:

  /** Default constructor
  */
  ctkFileValidator(QObject*   parent = 0);

  /** Constructor
   *  /param label        Used in file dialogs
   *  /param nameFilters  Regular expression (in wildcard mode) used to help the user to complete the line,
   *                      example: "Images (*.jpg *.gif *.png)"
   *  /param parent       Parent widget
  */
  ctkFileValidator(const QStringList& nameFilters,
                   ctkPathLineEdit::Filters filters = ctkPathLineEdit::AllEntries,
                   bool mustExists = false,
                   QObject* parent = 0 );
  virtual ~ctkFileValidator();

  void setNameFilters(const QStringList &nameFilters);
  const QStringList& nameFilters()const;

  void setFilters(const ctkPathLineEdit::Filters& filters);
  ctkPathLineEdit::Filters filters()const;

  void setMustExists(bool exists);
  bool mustExists()const;

  virtual State validate(QString& input, int &pos)const;

protected:
  QScopedPointer<ctkFileValidatorPrivate> d_ptr;

private:
  Q_DECLARE_PRIVATE(ctkFileValidator);
  Q_DISABLE_COPY(ctkFileValidator);
};

#endif // __ctkFileValidator_h
