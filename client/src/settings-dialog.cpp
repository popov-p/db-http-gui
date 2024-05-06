#include <QNetworkReply>
#include "settings-dialog.h"


SettingsDialog::SettingsDialog(BackendManager *backendManager, QWidget *parent) : QDialog(parent),
                                                                                  backendManager(backendManager) {
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);
    credentialsValidator = new NoSpcValidator();
    initHostLayout();
    initUsernameLayout();
    initPasswordLayout();
    initLogDirLayout();
    initLogLayout();
    initButtonLayout();
    initSettingsStatusLayout();
    initVerticalDialogLayout();
    initConnections();
}

void SettingsDialog::initHostLayout() {
    ipLabel = new QLabel("ip: ");
    ipLineEdit = new QLineEdit("http://127.0.0.1:8000");
    hIpLayout = new QHBoxLayout();
    hIpLayout->addWidget(ipLabel);
    hIpLayout->addWidget(ipLineEdit);
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

    verticalDialogLayout->addLayout(hIpLayout);
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
    connect(backendManager, &BackendManager::loginSuccessful, this, [this] () {
                ipLineEdit->setText("http://127.0.0.1:8000");
                usernameLineEdit->clear();
                passwordLineEdit->clear();
                logdirLineEdit->setText("../log");
                logComboBox->setCurrentIndex(0);
                settingsStatus->clear();

                /*dump cfg ini*/
                hide();
                qDebug() << "Qt UI: SettingsDialog ok button done";
                LOG(INFO) << "Qt UI: SettingsDialog ok button done";
    });
    connect(backendManager, &BackendManager::loginFailed, this, [this] (int errcode) {
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
    if(!ipLineEdit->text().isEmpty() && !usernameLineEdit->text().isEmpty() &&
       !passwordLineEdit->text().isEmpty() && !logdirLineEdit->text().isEmpty()) {
        backendManager->setBaseURL(ipLineEdit->text().replace(" ", ""));
        backendManager->login(usernameLineEdit->text(),
                              passwordLineEdit->text());
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
