#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <glog/logging.h>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSettings>
#include <QValidator>
#include "backend-manager.h"

class NoSpcValidator : public QValidator
{
public:
  State validate(QString &input, int &pos) const override
  {
    Q_UNUSED(pos);
    for (const QChar &ch : input)
    {
      if (ch == ' ')
      {
        return Invalid;
      }
    }
    return Acceptable;
  }
};

class SettingsDialog : public QDialog
{
  Q_OBJECT
public:
  SettingsDialog(BackendManager *backendManager, QWidget *parent = nullptr);
  void dumpCfgIni();
  void startLogging();
  ~SettingsDialog() = default;
public slots:
  void slotOkButtonDone();
  void slotCancelButtonClicked();

private:
  QPushButton *okDialogButton;
  QPushButton *cancelDialogButton;
  QLabel *ipLabel;
  QLabel *usernameLabel;
  QLabel *passwordLabel;
  QLabel *logdirLabel;
  QLabel *logLabel;
  QLabel *settingsStatus;
  QLineEdit *ipLineEdit;
  QLineEdit *usernameLineEdit;
  QLineEdit *passwordLineEdit;
  QLineEdit *logdirLineEdit;
  QComboBox *logComboBox;
  QHBoxLayout *hIpLayout;
  QHBoxLayout *hUsernameLayout;
  QHBoxLayout *hPasswordLayout;
  QHBoxLayout *hLogDirLayout;
  QHBoxLayout *hLogLayout;
  QHBoxLayout *hButtonLayout;
  QHBoxLayout *hSettingsStatusLayout;
  QVBoxLayout *verticalDialogLayout;
  NoSpcValidator *credentialsValidator;
  BackendManager *backendManager = nullptr;
  void initHostLayout();
  void initUsernameLayout();
  void initPasswordLayout();
  void initLogDirLayout();
  void initLogLayout();
  void initButtonLayout();
  void initSettingsStatusLayout();
  void initVerticalDialogLayout();
  void initConnections();
signals:
  void okButtonDone();
};

#endif // SETTINGSDIALOG_H
