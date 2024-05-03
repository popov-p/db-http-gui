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

    addCompMap.clear();
    compareComboBox->clear();
    startsWithLetterComboBox->clear();
    model->clear();

    emit disconnectButtonClicked();
}

// void MainWidget::slotDeleteSelectedRows() {
//     QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();
//     for (int i = selectedIndexes.count() - 1; i >= 0; --i) {
//         model->removeRow(selectedIndexes.at(i).row());
//     }
// }

void MainWidget::slotAddButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot add button clicked";
    auto it = addCompMap.constBegin();
    while (it != addCompMap.constEnd()) {
        const auto& key = it.key();
        const auto& value = it.value();
        addElementDialog->setInputFields(key, value);
        ++it;
    }
    addElementDialog->exec();
}

void MainWidget::slotLoginSuccessful() {
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
    totalFields.clear();
    model->clear();

    backendManager->getHeaders();
}

void MainWidget::slotGetHeadersSuccessful(QMap<QString, QStringList> fieldsMapResponse) {
    //qDebug() << "slotGetFieldsSuccessful";
    totalFields = fieldsMapResponse["total"];
    addCompMap.insert("comparable", fieldsMapResponse["comparable"]);
    addCompMap.insert("alphabetic", fieldsMapResponse["alphabetic"]);

    compareComboBox->addItems(addCompMap["comparable"]);
    startsWithLetterComboBox->addItems(addCompMap["alphabetic"]);
    //model->setHorizontalHeaderLabels(totalFields);
    backendManager->getAllRecordings();
    //qDebug() << "total fields ON DOWNLOAD are: ---" << totalFields.size();
}

void MainWidget::slotGetAllRecordingsSuccessful(QStringList currentKeyOrder, QList<QList<QStandardItem*>> rows) {
    //qDebug() << "slotGetAllDbRecordingsSuccessful ---- ";
    idLogicalIndex = currentKeyOrder.indexOf("id");

    for (const QList<QStandardItem*> &rowData : rows) {
        model->appendRow(rowData);
    }
    model->setHorizontalHeaderLabels(currentKeyOrder);
    QHeaderView *header = tableView->horizontalHeader();

    for (int i = 0; i < totalFields.size(); ++i) {
        const QString& columnName = totalFields.at(i);
        int columnIndex = currentKeyOrder.indexOf(columnName);
        if (columnIndex != -1) {
            header->moveSection(columnIndex, i);
            currentKeyOrder.move(columnIndex, i);
        }
    }
    tableView->setColumnHidden(idLogicalIndex, true);
}

void MainWidget::slotDeleteAllRecordingsSuccessful(int countDeleted) {
    model->clear();
    responseLabel->setText("Deleted " + QString::number(countDeleted) + "recordings");
}

void MainWidget::slotDeleteSelectedRecordingsSuccessful() {

}








void MainWidget::initHeaderLayout() {
    addButton = new QPushButton("Add");
    deleteSelectedButton = new QPushButton("Delete \n selected");
    deleteAllButton = new QPushButton("Delete all");
    disconnectButton = new QPushButton("Disconnect");
    usernameLabel = new QLabel("upd later");
    hHeaderLayout = new QHBoxLayout();

    QList<QPushButton*> buttons = {addButton, deleteSelectedButton,
                                   deleteAllButton, disconnectButton};
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

    QStringList alphabet;
    alphabet.append("-");
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        alphabet.append(QString(letter));
    }
    alphabetComboBox->addItems(alphabet);

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
    connect(deleteSelectedButton, &QPushButton::clicked, this, [this]() {
        //backendManager->deleteSelectedRecordings();
    });
    connect(deleteAllButton, &QPushButton::clicked,this, [this] () {
        backendManager->deleteAllRecordings();
    });
    connect(backendManager, &BackendManager::loginSuccessful, this, &MainWidget::slotLoginSuccessful);
    connect(backendManager, &BackendManager::getHeadersSuccessful, this, &MainWidget::slotGetHeadersSuccessful);
    connect(backendManager, &BackendManager::getAllRecordingsSuccessful, this, &MainWidget::slotGetAllRecordingsSuccessful);
    connect(backendManager, &BackendManager::deleteAllRecordingsSuccessful, this, &MainWidget::slotDeleteAllRecordingsSuccessful);
    connect(backendManager, &BackendManager::deleteSelectedRecordingsSuccessful, this, &MainWidget::slotDeleteSelectedRecordingsSuccessful);
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





