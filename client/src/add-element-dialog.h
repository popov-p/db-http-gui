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
#include <QNetworkAccessManager>

class AddElementDialog : public QDialog {
    Q_OBJECT
public:
    AddElementDialog(QNetworkAccessManager* networkManager, QWidget *parent = nullptr);
    void dumpCfgIni(std::string cfg_path);
    ~AddElementDialog();
signals:
    void okButtonDone();
    void cancelButtonClicked();
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
};

#endif
