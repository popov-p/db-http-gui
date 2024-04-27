#include "main-widget.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

MainWidget::MainWidget(BackendManager* backendManager, QWidget *parent) : backendManager(backendManager) {
    addElementDialog = new AddElementDialog(backendManager);

    initHeaderLayout();
    initResponseLabelLayout();
    initTableViewLayout();
    initFilterOptionsLayout();
    initWidgetVLayout();
    initConnections();
}
void MainWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot disconnect button clicked";
    addButton->hide();
    deleteSelectedButton->hide();
    deleteAllButton->hide();
    usernameLabel->hide();
    disconnectButton->hide();
    responseLabel->setText("Disconnected!");
    tableView->hide();

    startsWithLetterComboBox->hide();
    startsWithLetterLabel->hide();
    alphabetComboBox->hide();

    compareComboBox->hide();
    compareElementsComboBox->hide();
    geqCheckBox->hide();
    leqCheckBox->hide();
    //networkManager->clearAccessCache();

    emit disconnectButtonClicked();
}

void MainWidget::slotDeleteSelecterRows() {
    QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();
    for (int i = selectedIndexes.count() - 1; i >= 0; --i) {
        model->removeRow(selectedIndexes.at(i).row());
    }
}

void MainWidget::slotAuthSuccessful() {
    LOG(INFO) << "Qt: MainWidget handled slot connect button clicked";
    disconnectButton->show();
    addButton->show();
    deleteSelectedButton->show();
    deleteAllButton->show();
    tableView->show();

    startsWithLetterComboBox->show();
    startsWithLetterLabel->show();
    alphabetComboBox->show();

    compareComboBox->show();
    compareElementsComboBox->show();
    geqCheckBox->show();
    leqCheckBox->show();
    responseLabel->clear();
}

void MainWidget::slotAddButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot add button clicked";
    addElementDialog->exec();
}

// void MainWidget::slotFetchAllStudents() {
//     if (allStudentsReply->error() == QNetworkReply::NoError) {
//         QByteArray responseData = allStudentsReply->readAll();
//         QJsonDocument jsonDocument = QJsonDocument::fromJson(responseData);
//         if (!jsonDocument.isNull() && jsonDocument.isArray()) {
//             QList <QString> headers;
//             QJsonArray jsonArray = jsonDocument.array();
//             QJsonObject jsonObject = jsonArray.first().toObject();
//             for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it) {
//                 headers.append(it.key());
//             }
//             QJsonObject sortedJsonObject;
//             for (const QString& header : headers) {
//                 sortedJsonObject[header] = jsonObject.value(header);
//             }
//             model->setHorizontalHeaderLabels(headers);
//             //jsonDocument = QJsonDocument(sortedJsonObject);
//             foreach (const QJsonValue &value, jsonArray) {
//                 if (value.isObject()) {
//                     QJsonObject jsonObject = value.toObject();
//                     int id = jsonObject["id"].toInt();
//                     QString last_name = jsonObject["last_name"].toString();
//                     QString first_name = jsonObject["first_name"].toString();
//                     QString patronymic = jsonObject["patronymic"].toString();
//                     int year = jsonObject["year"].toInt();
//                     QString photo = jsonObject["photo"].toString();
//                     int course = jsonObject["course"].toInt();
//                     QString group = jsonObject["group"].toString();
//                     /*!!! REWRITE THIS !!!*/
//                     QList<QStandardItem*> rowItems;
//                     rowItems << new QStandardItem(QString::number(id));
//                     rowItems << new QStandardItem(last_name);
//                     rowItems << new QStandardItem(first_name);
//                     rowItems << new QStandardItem(patronymic);
//                     rowItems << new QStandardItem(QString::number(year));
//                     rowItems << new QStandardItem(photo);
//                     rowItems << new QStandardItem(QString::number(course));
//                     rowItems << new QStandardItem(group);

//                     model->appendRow(rowItems);
//                 }
//             }
//         } else {
//             responseLabel->setText("DB is empty");
//         }
//     }
//     else {
//         qDebug() << "Error:" << allStudentsReply->errorString();
//     }

//     allStudentsReply->deleteLater();
// }
//void MainWidget::handleAllHeaderOrder() {

//}

void MainWidget::initHeaderLayout() {
    addButton = new QPushButton("Add");
    deleteSelectedButton = new QPushButton("Delete \n selected");
    deleteAllButton = new QPushButton("Delete all");
    disconnectButton = new QPushButton("Disconnect");
    usernameLabel = new QLabel("upd later");
    hHeaderLayout = new QHBoxLayout();

    QList<QPushButton*> buttons = {addButton, deleteSelectedButton, deleteAllButton, disconnectButton};
    for (QPushButton *button : buttons) {
        button->setFixedSize(100, 50);
    }

    hHeaderLayout->addWidget(addButton);
    hHeaderLayout->addWidget(deleteSelectedButton);
    hHeaderLayout->addWidget(deleteAllButton);
    hHeaderLayout->addStretch();
    hHeaderLayout->addWidget(usernameLabel);
    hHeaderLayout->addWidget(disconnectButton);
}

void MainWidget::initResponseLabelLayout() {
    responseLabel = new QLabel("response status will be here");
    hResponseLabelLayout = new QHBoxLayout();

    responseLabel->setFixedSize(300, 20);
    responseLabel->setAlignment(Qt::AlignCenter);

    hResponseLabelLayout->addWidget(responseLabel);
}

void MainWidget::initTableViewLayout() {
    model = new QStandardItemModel();
    tableView = new QTableView();
    hTableViewLayout = new QHBoxLayout();

    tableView->setModel(model);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setStretchLastSection(true);

    hTableViewLayout->addWidget(tableView);
}

void MainWidget::initFilterOptionsLayout() {
    startsWithLetterComboBox = new QComboBox();
    startsWithLetterLabel = new QLabel("Starts with letter:");
    alphabetComboBox = new QComboBox();
    compareComboBox = new QComboBox();
    compareElementsComboBox = new QComboBox();
    leqCheckBox = new QCheckBox("And less");
    geqCheckBox = new QCheckBox("And greater");
    hFilterOptionsLayout = new QHBoxLayout();

    //startsWithLetterComboBox->addItem("Last name"); /*TODO: Fix this with a get*/
    //startsWithLetterComboBox->addItem("First name");
    //startsWithLetterComboBox->addItem("Patronymic");

    QStringList alphabet;
    alphabet.append("-");
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        alphabet.append(QString(letter));
    }
    alphabetComboBox->addItems(alphabet);


    //compareComboBox->addItem("Course"); /*TODO: Fix this !!*/
    //compareComboBox->addItem("Group");

    hFilterOptionsLayout->addWidget(startsWithLetterComboBox);
    hFilterOptionsLayout->addWidget(startsWithLetterLabel);
    hFilterOptionsLayout->addWidget(alphabetComboBox);
    hFilterOptionsLayout->addStretch();
    hFilterOptionsLayout->addWidget(compareComboBox);
    hFilterOptionsLayout->addWidget(compareElementsComboBox);
    hFilterOptionsLayout->addWidget(leqCheckBox);
    hFilterOptionsLayout->addWidget(geqCheckBox);
}


void MainWidget::initWidgetVLayout() {
    widgetVLayout = new QVBoxLayout();
    widgetVLayout->addLayout(hHeaderLayout);
    widgetVLayout->addStretch();
    widgetVLayout->addLayout(hResponseLabelLayout);
    widgetVLayout->addLayout(hTableViewLayout);
    widgetVLayout->addLayout(hFilterOptionsLayout);
    widgetVLayout->addStretch();
    setLayout(widgetVLayout);
}

void MainWidget::initConnections() {
    connect(addButton, &QPushButton::clicked, this, &MainWidget::slotAddButtonClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWidget::slotDisconnectButtonClicked);
    connect(deleteSelectedButton, &QPushButton::clicked, this, &MainWidget::slotDeleteSelecterRows);
    connect(deleteAllButton, &QPushButton::clicked,this, [this]() {
        model->clear();
    });
    connect(backendManager, &BackendManager::authSuccessful, this, &MainWidget::slotAuthSuccessful);
}

MainWidget::~MainWidget() {
    delete addButton;
    delete deleteSelectedButton;
    delete deleteAllButton;
    delete disconnectButton;
    delete usernameLabel;
    delete responseLabel;
    delete hHeaderLayout;
    delete hResponseLabelLayout;

    delete hTableViewLayout;
    delete model;
    delete tableView;

    delete startsWithLetterComboBox;
    delete startsWithLetterLabel;
    delete alphabetComboBox;

    delete compareComboBox;
    delete compareElementsComboBox;
    delete geqCheckBox;
    delete leqCheckBox;

    delete addElementDialog;
    delete hFilterOptionsLayout;
    delete widgetVLayout;
}





