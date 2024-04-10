#include "settings-dialog.h"


SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);

    credentialsValidator = new NoSpcValidator();

    okDialogButton = new QPushButton("Ok");
    cancelDialogButton = new QPushButton("Cancel");

    usernameLabel = new QLabel("username: ");
    passwordLabel = new QLabel("password: ");
    logdirLabel = new QLabel("logdir: ");
    logLabel = new QLabel("log level: ");
    settingsStatus = new QLabel();
    settingsStatus->setAlignment(Qt::AlignCenter);

    usernameLineEdit = new QLineEdit();
    passwordLineEdit = new QLineEdit();
    logdirLineEdit = new QLineEdit();
    logdirLineEdit->setText("../log");

    usernameLineEdit->setValidator(credentialsValidator);
    passwordLineEdit->setValidator(credentialsValidator);

    logComboBox = new QComboBox();
    logComboBox->addItem("INFO");
    logComboBox->addItem("ERROR");

    hUsernameLayout = new QHBoxLayout();
    hUsernameLayout->addWidget(usernameLabel);
    hUsernameLayout->addWidget(usernameLineEdit);

    hPasswordLayout = new QHBoxLayout();
    hPasswordLayout->addWidget(passwordLabel);
    hPasswordLayout->addWidget(passwordLineEdit);

    hLogDirLayout = new QHBoxLayout();
    hLogDirLayout->addWidget(logdirLabel);
    hLogDirLayout->addWidget(logdirLineEdit);

    hLogLayout = new QHBoxLayout();
    hLogLayout->addWidget(logLabel);
    hLogLayout->addWidget(logComboBox);

    hButtonLayout = new QHBoxLayout();
    hButtonLayout->addWidget(okDialogButton);
    hButtonLayout->addWidget(cancelDialogButton);


    hSettingsStatusLayout = new QHBoxLayout();
    hSettingsStatusLayout->addWidget(settingsStatus);

    verticalDialogLayout = new QVBoxLayout();
    verticalDialogLayout->addLayout(hUsernameLayout);
    verticalDialogLayout->addLayout(hPasswordLayout);
    verticalDialogLayout->addLayout(hLogDirLayout);
    verticalDialogLayout->addLayout(hLogLayout);
    verticalDialogLayout->addLayout(hSettingsStatusLayout);
    verticalDialogLayout->addLayout(hButtonLayout);

    setLayout(verticalDialogLayout);

    connect(okDialogButton, &QPushButton::clicked,
            this, &SettingsDialog::slotOkButtonDone);
    connect(cancelDialogButton, &QPushButton::clicked,
            this, &SettingsDialog::slotCancelButtonClicked);

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
        //dumpCfgIni(client.get_cfg_path());
        //client.load_cfg();
        //client.start_logging();
        usernameLineEdit->clear();
        passwordLineEdit->clear();
        logdirLineEdit->setText("../log");
        settingsStatus->clear();
        hide();
        LOG(INFO) << "Qt: SettingsDialog ok button done";
        emit okButtonDone();
    }
    else {
        settingsStatus->setText("set valid parameters");
    }
}

void SettingsDialog::slotCancelButtonClicked() {
    usernameLineEdit->clear();
    passwordLineEdit->clear();
    settingsStatus->clear();
    emit cancelButtonClicked();
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
