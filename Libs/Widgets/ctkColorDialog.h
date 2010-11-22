/*=========================================================================

  Library:   CTK

  Copyright (c) Kitware Inc.

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

#ifndef __ctkColorDialog_h
#define __ctkColorDialog_h

// Qt includes
#include <QColorDialog>

// CTK includes
#include "ctkWidgetsExport.h"

class ctkColorDialogPrivate;

/// Customizable QColorDialog.
/// Extra widgets can be added to the left of the dialog into a QStackedWidget
class CTK_WIDGETS_EXPORT ctkColorDialog : public QColorDialog
{
  Q_OBJECT

public:
  /// Constructor
  /// By default, behaves like a QColorDialog
  /// \sa QColorDialog()
  explicit ctkColorDialog(QWidget* parent = 0);
  explicit ctkColorDialog(const QColor& initial, QWidget* parent = 0);
  virtual ~ctkColorDialog();
  
  /// Add an extra widget under the file format combobox. If a label is
  /// given, it will appear in the first column.
  /// The widget is reparented to ctkColorDialog
  void addTab(QWidget* widget, const QString& label);

  /// Return the extra widget if any
  QWidget* widget(int index)const;

  /// Pops up a modal color dialog with the given window \a title (or "Select Color" if none is
  /// specified), lets the user choose a color, and returns that color. The color is initially set
  /// to \a initial. The dialog is a child of \a parent. It returns an invalid (see
  /// QColor::isValid()) color if the user cancels the dialog.
  ///
  /// The \a options argument allows you to customize the dialog;
  /// QColorDialog::DontUseNativeDialog is forced
  static QColor getColor(const QColor &initial, QWidget *parent,
                         const QString &title, ColorDialogOptions options);
  static void addDefaultTab(QWidget* widget, const QString& label);

protected:
  QScopedPointer<ctkColorDialogPrivate> d_ptr;

  static QList<QWidget*> DefaultTabs;
private:
  Q_DECLARE_PRIVATE(ctkColorDialog);
  Q_DISABLE_COPY(ctkColorDialog);
};

#endif