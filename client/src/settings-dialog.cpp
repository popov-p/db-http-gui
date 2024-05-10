#include <QNetworkReply>
#include "settings-dialog.h"
#include <QDir>

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
    ipLabel = new QLabel("ip: ", this);
    ipLineEdit = new QLineEdit("http://127.0.0.1:8000", this);
    hIpLayout = new QHBoxLayout();
    hIpLayout->addWidget(ipLabel);
    hIpLayout->addWidget(ipLineEdit);
}

void SettingsDialog::initUsernameLayout() {
    usernameLabel = new QLabel("username: ", this);
    usernameLineEdit = new QLineEdit(this);
    hUsernameLayout = new QHBoxLayout();

    usernameLineEdit->setValidator(credentialsValidator);
    hUsernameLayout->addWidget(usernameLabel);
    hUsernameLayout->addWidget(usernameLineEdit);
}

void SettingsDialog::initPasswordLayout() {
    passwordLabel = new QLabel("password: ", this);
    passwordLineEdit = new QLineEdit(this);
    hPasswordLayout = new QHBoxLayout();

    passwordLineEdit->setEchoMode(QLineEdit::Password);
    passwordLineEdit->setValidator(credentialsValidator);
    hPasswordLayout->addWidget(passwordLabel);
    hPasswordLayout->addWidget(passwordLineEdit);
}

void SettingsDialog::initLogDirLayout() {
    logdirLabel = new QLabel("logdir: ", this);
    logdirLineEdit = new QLineEdit(this);
    hLogDirLayout = new QHBoxLayout();

    logdirLineEdit->setText("../log");
    hLogDirLayout->addWidget(logdirLabel);
    hLogDirLayout->addWidget(logdirLineEdit);
}

void SettingsDialog::initLogLayout() {
    logLabel = new QLabel("log severity: ", this);
    logComboBox = new QComboBox(this);
    logComboBox->addItem("INFO");
    logComboBox->addItem("ERROR");
    hLogLayout = new QHBoxLayout();
    hLogLayout->addWidget(logLabel);
    hLogLayout->addWidget(logComboBox);
}

void SettingsDialog::initButtonLayout() {
    okDialogButton = new QPushButton("Ok", this);
    cancelDialogButton = new QPushButton("Cancel", this);
    hButtonLayout = new QHBoxLayout();

    hButtonLayout->addWidget(okDialogButton);
    hButtonLayout->addWidget(cancelDialogButton);
}

void SettingsDialog::initSettingsStatusLayout() {
    settingsStatus = new QLabel(this);
    hSettingsStatusLayout = new QHBoxLayout();

    settingsStatus->setAlignment(Qt::AlignCenter);
    hSettingsStatusLayout->addWidget(settingsStatus);
}

void SettingsDialog::initVerticalDialogLayout() {
    verticalDialogLayout = new QVBoxLayout(this);

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
        dumpCfgIni();
        startLogging();
        ipLineEdit->setText("http://127.0.0.1:8000");
        usernameLineEdit->clear();
        passwordLineEdit->clear();
        logdirLineEdit->setText("../log");
        logComboBox->setCurrentIndex(0);
        settingsStatus->clear();
        hide();
        LOG(INFO) << "Login successful";
    });
    connect(backendManager, &BackendManager::loginFailed, this, [this] (int errcode) {
        LOG(ERROR) << "Login failed, errcode = " + QString::number(errcode).toStdString();
        settingsStatus->setText("Auth failed! Errcode: " + QString::number(errcode));
    });
}

void SettingsDialog::slotOkButtonDone() {
    if(!ipLineEdit->text().isEmpty() && !usernameLineEdit->text().isEmpty() &&
       !passwordLineEdit->text().isEmpty() && !logdirLineEdit->text().isEmpty()) {
        backendManager->setBaseURL(ipLineEdit->text().replace(" ", ""));
        backendManager->login(usernameLineEdit->text(),
                              passwordLineEdit->text());
    }
    else {
        LOG(ERROR) << "Error in startup parameters setting";
        settingsStatus->setText("set valid parameters");
    }
}

void SettingsDialog::slotCancelButtonClicked() {
    LOG(INFO) << "Cancel button clicked";
    usernameLineEdit->clear();
    passwordLineEdit->clear();
    settingsStatus->clear();
    hide();
}

void SettingsDialog::dumpCfgIni() {
    QSettings settingsIni = QSettings(QDir(logdirLineEdit->text() + QString("/../cfg.ini")).absolutePath(), QSettings::IniFormat);

    settingsIni.beginGroup("base-url");
    settingsIni.setValue("ip", ipLineEdit->text());
    settingsIni.endGroup();

    settingsIni.beginGroup("auth");
    settingsIni.setValue("last-active-user", usernameLineEdit->text());
    settingsIni.endGroup();

    settingsIni.beginGroup("logging");
    settingsIni.setValue("dir", QDir(logdirLineEdit->text()).absolutePath());
    settingsIni.setValue("severity", logComboBox->currentText());
    settingsIni.endGroup();
    LOG(INFO) << "Qt: SettingsDialog dumping cfg";
}

void SettingsDialog::startLogging() {
    auto logDir = QDir(logdirLineEdit->text());
    if (!QDir(logdirLineEdit->text()).exists()) {
        QDir().mkdir(logdirLineEdit->text());
    }

    if (logComboBox->currentText() == "INFO") {
        google::SetLogDestination(google::GLOG_INFO, (logDir.filePath("INFO_").toStdString()).c_str());
    }
    else if (logComboBox->currentText() == "ERROR") {
        google::SetLogDestination(google::GLOG_ERROR, (logDir.filePath("ERROR_").toStdString()).c_str());
    }
    else {
        google::SetLogDestination(google::GLOG_INFO, (logDir.filePath("INFO_").toStdString()).c_str());
        LOG(INFO) << "Unknown log lvl. Default: INFO";
    }
}
