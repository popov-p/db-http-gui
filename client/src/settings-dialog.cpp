#include <QNetworkReply>
#include "settings-dialog.h"


SettingsDialog::SettingsDialog(BackendManager *backendManager, QWidget *parent) : QDialog(parent),
                                                                                  backendManager(backendManager) {
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
    credentialsValidator = new NoSpcValidator();
    initUsernameLayout();
    initPasswordLayout();
    initLogDirLayout();
    initLogLayout();
    initButtonLayout();
    initSettingsStatusLayout();
    initVerticalDialogLayout();
    initConnections();
}



void SettingsDialog::initUsernameLayout() {
    usernameLabel = new QLabel("username: ");
    usernameLineEdit = new QLineEdit();
    hUsernameLayout = new QHBoxLayout();

    usernameLineEdit->setValidator(credentialsValidator);
    hUsernameLayout->addWidget(usernameLabel);
    hUsernameLayout->addWidget(usernameLineEdit);
}
void SettingsDialog::initPasswordLayout() {
    passwordLabel = new QLabel("password: ");
    passwordLineEdit = new QLineEdit();
    hPasswordLayout = new QHBoxLayout();

    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setValidator(credentialsValidator);
    hPasswordLayout->addWidget(passwordLabel);
    hPasswordLayout->addWidget(passwordLineEdit);

}
void SettingsDialog::initLogDirLayout() {
    logdirLabel = new QLabel("logdir: ");
    logdirLineEdit = new QLineEdit();
    hLogDirLayout = new QHBoxLayout();

    logdirLineEdit->setText("../log");
    hLogDirLayout->addWidget(logdirLabel);
    hLogDirLayout->addWidget(logdirLineEdit);
}
void SettingsDialog::initLogLayout() {
    logLabel = new QLabel("log severity: ");
    logComboBox = new QComboBox();
    logComboBox->addItem("INFO");
    logComboBox->addItem("ERROR");
    hLogLayout = new QHBoxLayout();
    hLogLayout->addWidget(logLabel);
    hLogLayout->addWidget(logComboBox);
}
void SettingsDialog::initButtonLayout() {
    okDialogButton = new QPushButton("Ok");
    cancelDialogButton = new QPushButton("Cancel");
    hButtonLayout = new QHBoxLayout();

    hButtonLayout->addWidget(okDialogButton);
    hButtonLayout->addWidget(cancelDialogButton);
}
void SettingsDialog::initSettingsStatusLayout() {
    settingsStatus = new QLabel();
    hSettingsStatusLayout = new QHBoxLayout();

    settingsStatus->setAlignment(Qt::AlignCenter);
    hSettingsStatusLayout->addWidget(settingsStatus);
}
void SettingsDialog::initVerticalDialogLayout() {
    verticalDialogLayout = new QVBoxLayout();

    verticalDialogLayout->addLayout(hUsernameLayout);
    verticalDialogLayout->addLayout(hPasswordLayout);
    verticalDialogLayout->addLayout(hLogDirLayout);
    verticalDialogLayout->addLayout(hLogLayout);
    verticalDialogLayout->addLayout(hSettingsStatusLayout);
    verticalDialogLayout->addLayout(hButtonLayout);

    setLayout(verticalDialogLayout);
}
void SettingsDialog::initConnections() {
    connect(okDialogButton, &QPushButton::clicked,
            this, &SettingsDialog::slotOkButtonDone);
    connect(cancelDialogButton, &QPushButton::clicked,
            this, &SettingsDialog::slotCancelButtonClicked);
    connect(backendManager, &BackendManager::authSuccessful, this, [this] () {
                 usernameLineEdit->clear();
                 passwordLineEdit->clear();
                 logdirLineEdit->setText("../log");
                 logComboBox->setCurrentIndex(0);
                 settingsStatus->clear();
                 hide();
                 qDebug() << "Qt UI: SettingsDialog ok button done";
                 LOG(INFO) << "Qt UI: SettingsDialog ok button done";
    });
    connect(backendManager, &BackendManager::authFailed, this, [this] (int errcode) {
        settingsStatus->setText("Auth failed! Errcode: " + QString::number(errcode));
    });
}


SettingsDialog::~SettingsDialog() {
    delete okDialogButton;
    delete cancelDialogButton;
    delete usernameLabel;
    delete passwordLabel;
    delete logdirLabel;
    delete logLabel;
    delete usernameLineEdit;
    delete passwordLineEdit;
    delete logdirLineEdit;
    delete logComboBox;
    delete settingsStatus;
    delete hUsernameLayout;
    delete hPasswordLayout;
    delete hLogDirLayout;
    delete hLogLayout;
    delete hSettingsStatusLayout;
    delete hButtonLayout;
    delete verticalDialogLayout;
    delete credentialsValidator;
}

void SettingsDialog::slotOkButtonDone() {
    if(!usernameLineEdit->text().isEmpty() &&
       !passwordLineEdit->text().isEmpty() && !logdirLineEdit->text().isEmpty()) {
        backendManager->login(usernameLineEdit->text(),
                              passwordLineEdit->text());

        // QObject::connect(reply, &QNetworkReply::finished, this, [&, reply](){
        //     if(reply->error() == QNetworkReply::NoError) {
        //         LOG(INFO) << reply->readAll().toStdString();
        //         emit okButtonDone();
        //     } else if (reply->error() == QNetworkReply::ContentAccessDenied) {
        //         LOG(ERROR) << reply->errorString().toStdString();
        //         //networkManager->clearAccessCache();
        //         settingsStatus->setText("Auth failed");
        //     }
        //     else {
        //         LOG(ERROR) << reply->errorString().toStdString();
        //         //networkManager->clearAccessCache();
        //         settingsStatus->setText("Connection error");

        //     }
        //     reply->deleteLater();
        // });
        /*TODO: fix this later*/
        //dumpCfgIni(client.get_cfg_path());
        //client.load_cfg();
        //client.start_logging();
    }
    else {
        settingsStatus->setText("set valid parameters");
    }
}

void SettingsDialog::slotCancelButtonClicked() {
    usernameLineEdit->clear();
    passwordLineEdit->clear();
    settingsStatus->clear();
    hide();
}

void SettingsDialog::dumpCfgIni(std::string cfg_path) {
    QSettings *settingsIni = new QSettings(QString::fromStdString(cfg_path), QSettings::IniFormat);
    settingsIni->beginGroup("auth");
    settingsIni->setValue("username", usernameLineEdit->text());
    settingsIni->setValue("password", passwordLineEdit->text());
    settingsIni->beginGroup("logging");
    settingsIni->setValue("dir", logdirLineEdit->text());
    settingsIni->setValue("severity", logComboBox->currentText());
    settingsIni->endGroup();
    delete settingsIni;
    LOG(INFO) << "Qt: SettingsDialog dumping cfg";
}
