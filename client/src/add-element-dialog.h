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
#include <tuple>
#include <QRegularExpressionValidator>
#include <QIntValidator>
#include "backend-manager.h"

typedef std::tuple<QHBoxLayout*, QLabel*, QLineEdit*> WidgetTuple;

class AddElementDialog : public QDialog {
    Q_OBJECT
public:
    AddElementDialog(BackendManager *backendManager, QWidget *parent = nullptr);
    void setInputFields(QString fieldsType, QStringList fieldsList);
    ~AddElementDialog() = default;
public slots:
    void slotOkButtonDone();
    void slotCancelButtonClicked();
    void slotAddButtonClicked();
    void slotDeletePhotoButtonClicked();

private:
    QList<WidgetTuple> alphabeticFields;
    QList<WidgetTuple> comparableFields;
    QRegularExpressionValidator *alphabeticValidator;
    QIntValidator *comparableValidator;
    QPushButton *okDialogButton;
    QPushButton *cancelDialogButton;
    QPushButton *addPhotoButton;
    QPushButton *deletePhotoButton;
    QLabel *settingsStatus;
    QLabel *photoLabel;
    QLineEdit * photoPathLineEdit;

    QHBoxLayout *hButtonLayout;
    QHBoxLayout *hSettingsStatusLayout;
    QHBoxLayout *hPhotoLoadLayout;
    QVBoxLayout *verticalDialogLayout;
    BackendManager *backendManager = nullptr;

    void initButtonLayout();
    void initPhotoLayout();
    void initSettingsStatusLayout();
    void initVerticalDialogLayout();
    void initConnections();

    void inputFieldsCleanup();
signals:
    void okButtonDone();
};

#endif
