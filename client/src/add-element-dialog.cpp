#include "add-element-dialog.h"
#include <QDebug>

AddElementDialog::AddElementDialog(QNetworkAccessManager* networkManager, QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);

    okDialogButton = new QPushButton("Ok");
    cancelDialogButton = new QPushButton("Cancel");

    settingsStatus = new QLabel();
    settingsStatus->setAlignment(Qt::AlignCenter);

    hButtonLayout = new QHBoxLayout();
    hButtonLayout->addWidget(okDialogButton);
    hButtonLayout->addWidget(cancelDialogButton);


    hSettingsStatusLayout = new QHBoxLayout();
    hSettingsStatusLayout->addWidget(settingsStatus);

    verticalDialogLayout = new QVBoxLayout();
    verticalDialogLayout->addLayout(hSettingsStatusLayout);
    verticalDialogLayout->addLayout(hButtonLayout);

    setLayout(verticalDialogLayout);

    connect(okDialogButton, &QPushButton::clicked,
            this, &AddElementDialog::slotOkButtonDone);
    connect(cancelDialogButton, &QPushButton::clicked,
            this, &AddElementDialog::slotCancelButtonClicked);

}

AddElementDialog::~AddElementDialog() {
    delete okDialogButton;
    delete cancelDialogButton;
    delete settingsStatus;
    delete hButtonLayout;
    delete hSettingsStatusLayout;
    delete verticalDialogLayout;

}

void AddElementDialog::slotOkButtonDone() {
    qDebug() << "Qt : AddElementDialog slot ok button done";
    hide(); //for now

//    if(!usernameLineEdit->text().isEmpty() &&
//       !passwordLineEdit->text().isEmpty() && !logdirLineEdit->text().isEmpty()) {
//        //dumpCfgIni(client.get_cfg_path());
//        //client.load_cfg();
//        //client.start_logging();
//        usernameLineEdit->clear();
//        passwordLineEdit->clear();
//        logdirLineEdit->setText("../log");
//        settingsStatus->clear();
//        hide();
//        LOG(INFO) << "Qt: SettingsDialog ok button done";
//        emit okButtonDone();
//    }
//    else {
//        settingsStatus->setText("set valid parameters");
//    }
}

void AddElementDialog::slotCancelButtonClicked() {
    LOG(INFO) << "Qt: AddElementDialog slotCancelButtonClicked";
//    usernameLineEdit->clear();
//    passwordLineEdit->clear();
//    settingsStatus->clear();
    emit cancelButtonClicked();
    hide();
}

void AddElementDialog::dumpCfgIni(std::string cfg_path) {
//    QSettings *settingsIni = new QSettings(QString::fromStdString(cfg_path), QSettings::IniFormat);
//    settingsIni->beginGroup("auth");
//    settingsIni->setValue("username", usernameLineEdit->text());
//    settingsIni->setValue("password", passwordLineEdit->text());
//    settingsIni->beginGroup("logging");
//    settingsIni->setValue("dir", logdirLineEdit->text());
//    settingsIni->setValue("severity", logComboBox->currentText());
//    settingsIni->endGroup();
//    delete settingsIni;
//    LOG(INFO) << "Qt: SettingsDialog dumping cfg";
}
