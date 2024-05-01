#ifndef ADD_ELEMENT_DIALOG_H
#define ADD_ELEMENT_DIALOG_H

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
#include "backend-manager.h"

class AddElementDialog : public QDialog {
    Q_OBJECT
public:
    AddElementDialog(BackendManager *backendManager, QWidget *parent = nullptr);
    void setAlphabeticFields(QStringList alphabeticFields);
    void setComparableFields(QStringList comparableFields);
    ~AddElementDialog();
public slots:
    void slotOkButtonDone();
    void slotCancelButtonClicked();
private:
    QPushButton *okDialogButton;
    QPushButton *cancelDialogButton;
    QLabel *settingsStatus;
    QHBoxLayout *hButtonLayout;
    QHBoxLayout *hSettingsStatusLayout;
    QVBoxLayout *verticalDialogLayout;
    BackendManager *backendManager = nullptr;

    void initButtonLayout();
    void initSettingsStatusLayout();
    void initVerticalDialogLayout();
    void initConnections();
signals:
    void okButtonDone();
};

#endif
