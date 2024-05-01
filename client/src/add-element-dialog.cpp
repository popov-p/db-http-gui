#include "add-element-dialog.h"
#include <QDebug>
#include <QFileDialog>

AddElementDialog::AddElementDialog(BackendManager *backendManager, QWidget *parent) : QDialog(parent), backendManager(backendManager) {
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);

    initPhotoLayout();
    initButtonLayout();
    initSettingsStatusLayout();
    initVerticalDialogLayout();
    initConnections();
}

void AddElementDialog::initPhotoLayout() {
    addPhotoButton = new QPushButton("Add");
    deletePhotoButton = new QPushButton("Cancel");
    deletePhotoButton->hide();
    photoLabel = new QLabel("Add photo: ");
    photoPathLineEdit = new QLineEdit();
    hPhotoLoadLayout = new QHBoxLayout();

    photoPathLineEdit->setReadOnly(true);

    hPhotoLoadLayout->addWidget(photoLabel);
    hPhotoLoadLayout->addWidget(photoPathLineEdit);
    hPhotoLoadLayout->addWidget(addPhotoButton);
    hPhotoLoadLayout->addWidget(deletePhotoButton);

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
    verticalDialogLayout->addLayout(hPhotoLoadLayout);
    verticalDialogLayout->addLayout(hSettingsStatusLayout);
    verticalDialogLayout->addLayout(hButtonLayout);
    setLayout(verticalDialogLayout);
}

void AddElementDialog::initConnections() {
    connect(okDialogButton, &QPushButton::clicked,
            this, &AddElementDialog::slotOkButtonDone);
    connect(cancelDialogButton, &QPushButton::clicked,
            this, &AddElementDialog::slotCancelButtonClicked);
    connect(addPhotoButton, &QPushButton::clicked, this, &AddElementDialog::slotAddButtonClicked);
    connect(deletePhotoButton, &QPushButton::clicked, this, &AddElementDialog::slotDeletePhotoButtonClicked);
}

AddElementDialog::~AddElementDialog() {
    delete okDialogButton;
    delete cancelDialogButton;
    delete settingsStatus;
    delete hButtonLayout;
    delete hSettingsStatusLayout;
    delete verticalDialogLayout;
    delete addPhotoButton;
    delete photoLabel;
    delete photoPathLineEdit;


}

void AddElementDialog::slotOkButtonDone() {
    qDebug() << "Qt : AddElementDialog slot ok button done";

    bool validation_success = true;

    auto widgetLists = {alphabeticFields, comparableFields};
    for (const auto& widgetList: widgetLists) {
        for (const auto& widgetTuple: widgetList) {
            if (std::get<2>(widgetTuple)->text().isEmpty()) {
                settingsStatus->setText("Only photo field is optional!");
                validation_success = false;
            }
        }
    }
    if (validation_success) {
        hide();
        inputFieldsCleanup();
        photoPathLineEdit->clear();
        settingsStatus->clear();
        emit okButtonDone();
    }

}

void AddElementDialog::slotCancelButtonClicked() {
    LOG(INFO) << "Qt: AddElementDialog slotCancelButtonClicked";
    inputFieldsCleanup();
    photoPathLineEdit->clear();
    settingsStatus->clear();
    hide();
}

void AddElementDialog::slotAddButtonClicked() {
    LOG(INFO) << "Qt: AddElementDialog slotAddButtonClicked";
    QString filePath = QFileDialog::getOpenFileName(this, tr("Choose photo"), "", tr("Extensions (*.png *.jpg *.jpeg *.bmp *.gif)"));
    if (!filePath.isEmpty()) {
        photoPathLineEdit->setText(filePath);
        addPhotoButton->hide();
        deletePhotoButton->show();
    }

}

void AddElementDialog::slotDeletePhotoButtonClicked() {
    LOG(INFO) << "Qt: AddElementDialog slotDeleteButtonClicked";
    photoPathLineEdit->clear();
    deletePhotoButton->hide();
    addPhotoButton->show();
}

void AddElementDialog::setInputFields(QString fieldsType, QStringList fieldsList) {
    if(!fieldsList.empty()) {
        alphabeticValidator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z\\-]+"), this);
        comparableValidator = new QIntValidator();
        for (auto it = fieldsList.begin(); it != fieldsList.end(); ++it) {
            QHBoxLayout *layout = new QHBoxLayout();
            QLabel *label = new QLabel(*it + ": ");
            QLineEdit *lineEdit = new QLineEdit();
            if (fieldsType == "alphabetic") {
                lineEdit->setValidator(alphabeticValidator);
            }
            else if (fieldsType == "comparable") {
                lineEdit->setValidator(comparableValidator);
            }

            layout->addWidget(label);
            layout->addWidget(lineEdit);

            WidgetTuple widgetTuple = std::make_tuple(layout, label, lineEdit);
            alphabeticFields.append(widgetTuple);
            verticalDialogLayout->insertLayout(verticalDialogLayout->indexOf(hPhotoLoadLayout), layout);
        }
    }
    else {
        settingsStatus->setText("Some error occured");
    }
}

void AddElementDialog::inputFieldsCleanup() {
    for (const auto& widgetTuple:alphabeticFields) {
        QHBoxLayout *layout = std::get<0>(widgetTuple);
        QLabel *label = std::get<1>(widgetTuple);
        QLineEdit *lineEdit = std::get<2>(widgetTuple);
        delete layout;
        delete label;
        delete lineEdit;
        alphabeticFields.clear();
    }
}
