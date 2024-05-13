#include "add-element-dialog.h"
#include <QDebug>
#include <QFileDialog>
#include <QBuffer>

AddElementDialog::AddElementDialog(BackendManager *backendManager, QWidget *parent) : QDialog(parent), backendManager(backendManager)
{
  setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);
  setWindowFlags(windowFlags() | Qt::CustomizeWindowHint);

  initPhotoLayout();
  initButtonLayout();
  initSettingsStatusLayout();
  initVerticalDialogLayout();
  initConnections();
}

void AddElementDialog::initPhotoLayout()
{
  addPhotoButton = new QPushButton("Add", this);
  deletePhotoButton = new QPushButton("Cancel", this);
  deletePhotoButton->hide();
  photoLabel = new QLabel("Add photo: ", this);
  photoPathLineEdit = new QLineEdit(this);
  hPhotoLoadLayout = new QHBoxLayout();

  photoPathLineEdit->setReadOnly(true);

  hPhotoLoadLayout->addWidget(photoLabel);
  hPhotoLoadLayout->addWidget(photoPathLineEdit);
  hPhotoLoadLayout->addWidget(addPhotoButton);
  hPhotoLoadLayout->addWidget(deletePhotoButton);
}

void AddElementDialog::initButtonLayout()
{
  okDialogButton = new QPushButton("Ok", this);
  cancelDialogButton = new QPushButton("Cancel", this);
  hButtonLayout = new QHBoxLayout();
  hButtonLayout->addWidget(okDialogButton);
  hButtonLayout->addWidget(cancelDialogButton);
}

void AddElementDialog::initSettingsStatusLayout()
{
  settingsStatus = new QLabel(this);
  settingsStatus->setAlignment(Qt::AlignCenter);
  hSettingsStatusLayout = new QHBoxLayout();
  hSettingsStatusLayout->addWidget(settingsStatus);
}

void AddElementDialog::initVerticalDialogLayout()
{
  verticalDialogLayout = new QVBoxLayout(this);
  verticalDialogLayout->addLayout(hPhotoLoadLayout);
  verticalDialogLayout->addLayout(hSettingsStatusLayout);
  verticalDialogLayout->addLayout(hButtonLayout);
  setLayout(verticalDialogLayout);
}

void AddElementDialog::initConnections()
{
  connect(okDialogButton, &QPushButton::clicked,
          this, &AddElementDialog::slotOkButtonDone);
  connect(cancelDialogButton, &QPushButton::clicked,
          this, &AddElementDialog::slotCancelButtonClicked);
  connect(addPhotoButton, &QPushButton::clicked, this, &AddElementDialog::slotAddButtonClicked);
  connect(deletePhotoButton, &QPushButton::clicked, this, &AddElementDialog::slotDeletePhotoButtonClicked);
}

void AddElementDialog::slotOkButtonDone()
{
  LOG(INFO) << "Slot ok button done";
  bool validation_success = true;
  std::map<QString, std::variant<QString, int>> dataMap;

  auto widgetLists = {alphabeticFields, comparableFields};
  for (const auto &widgetList : widgetLists)
  {
    for (const auto &widgetTuple : widgetList)
    {
      QLineEdit *lineEdit = std::get<2>(widgetTuple);
      QString fieldKey = std::get<1>(widgetTuple)->text().remove(": ");

      if (lineEdit->text().isEmpty())
      {
        LOG(ERROR) << "Validation failed";
        settingsStatus->setText("Incorrect Felds!");
        validation_success = false;
        dataMap.clear();
      }
      else if (lineEdit->text().toInt())
      {
        dataMap[fieldKey] = lineEdit->text().toInt();
      }
      else
      {
        dataMap[fieldKey] = lineEdit->text();
      }
    }
  }

  if (photoPathLineEdit->text().isEmpty())
  {
    validation_success = false;
    dataMap.clear();
  }
  else
  {
    QString imagePath = photoPathLineEdit->text();
    QPixmap pixmap = QPixmap(imagePath).scaled(100, 50, Qt::KeepAspectRatio);
    QImage image = pixmap.toImage();
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");
    QString base64Image = QString::fromLatin1(byteArray.toBase64().data());
    dataMap["photo"] = base64Image;
  }

  if (validation_success)
  {
    hide();
    inputFieldsCleanup();
    photoPathLineEdit->clear();
    settingsStatus->clear();
    LOG(INFO) << "Validation success";
    backendManager->addRecording(dataMap);
    emit okButtonDone();
  }
  else
  {
    LOG(ERROR) << "Incorrect data set";
    settingsStatus->setText("Add correct data");
  }
}

void AddElementDialog::slotCancelButtonClicked()
{
  LOG(INFO) << "Slot cancel button clicked";

  inputFieldsCleanup();
  photoPathLineEdit->clear();
  settingsStatus->clear();
  hide();
}

void AddElementDialog::slotAddButtonClicked()
{
  LOG(INFO) << "Slot add button clicked";
  QString filePath = QFileDialog::getOpenFileName(this, tr("Choose photo"), "", tr("Extensions (*.png *.jpg *.jpeg *.bmp *.gif)"));
  if (!filePath.isEmpty())
  {
    photoPathLineEdit->setText(filePath);
    addPhotoButton->hide();
    deletePhotoButton->show();
  }
}

void AddElementDialog::slotDeletePhotoButtonClicked()
{
  LOG(INFO) << "Slot add button clicked";
  photoPathLineEdit->clear();
  deletePhotoButton->hide();
  addPhotoButton->show();
}

void AddElementDialog::setInputFields(QString fieldsType, QStringList fieldsList)
{
  LOG(INFO) << "Constructing input fields";
  if (!fieldsList.empty())
  {
    alphabeticValidator = new QRegularExpressionValidator(QRegularExpression("[a-zA-Z\\-]+"), this);
    comparableValidator = new QIntValidator(1960, 2024, this);
    for (auto it = fieldsList.begin(); it != fieldsList.end(); ++it)
    {
      QHBoxLayout *layout = new QHBoxLayout();
      QLabel *label = new QLabel(*it + ": ");
      QLineEdit *lineEdit = new QLineEdit(this);
      if (fieldsType == "alphabetic")
      {
        lineEdit->setValidator(alphabeticValidator);
      }
      else if (fieldsType == "comparable")
      {
        lineEdit->setValidator(comparableValidator);
      }

      layout->addWidget(label);
      layout->addWidget(lineEdit);

      WidgetTuple widgetTuple = std::make_tuple(layout, label, lineEdit);
      alphabeticFields.append(widgetTuple);
      verticalDialogLayout->insertLayout(verticalDialogLayout->indexOf(hPhotoLoadLayout), layout);
    }
  }
  else
  {
    LOG(ERROR) << "Backend sent incorrect or empty database fields list";
    settingsStatus->setText("Some error occured");
  }
}

void AddElementDialog::inputFieldsCleanup()
{
  LOG(INFO) << "Input fields cleanup";
  auto widgetLists = {alphabeticFields, comparableFields};
  for (const auto &widgetList : widgetLists)
  {
    for (const auto &widgetTuple : widgetList)
    {
      std::get<2>(widgetTuple)->clear();
    }
  }
}
