#include "add-element-dialog.h"
#include <QDebug>

AddElementDialog::AddElementDialog(BackendManager *backendManager, QWidget *parent) : QDialog(parent), backendManager(backendManager) {
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);

    initButtonLayout();
    initSettingsStatusLayout();
    initVerticalDialogLayout();
    initConnections();
}

void AddElementDialog::initButtonLayout() {
    okDialogButton = new QPushButton("Ok");
    cancelDialogButton = new QPushButton("Cancel");
    hButtonLayout = new QHBoxLayout();
    hButtonLayout->addWidget(okDialogButton);
    hButtonLayout->addWidget(cancelDialogButton);
}

void AddElementDialog::initSettingsStatusLayout() {
    settingsStatus = new QLabel();
    settingsStatus->setAlignment(Qt::AlignCenter);
    hSettingsStatusLayout = new QHBoxLayout();
    hSettingsStatusLayout->addWidget(settingsStatus);
}

void AddElementDialog::initVerticalDialogLayout() {
    verticalDialogLayout = new QVBoxLayout();
    verticalDialogLayout->addLayout(hSettingsStatusLayout);
    verticalDialogLayout->addLayout(hButtonLayout);
    setLayout(verticalDialogLayout);
}

void AddElementDialog::initConnections() {
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
    hide();
}

