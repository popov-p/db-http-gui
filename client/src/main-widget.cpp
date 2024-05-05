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

    compHeaderData.clear();
    alphCompMap.clear();
    compareComboBox->clear();
    startsWithLetterComboBox->clear();
    filterLabel->hide();
    dropFilterButton->hide();
    filterButton->hide();
    model->clear();

    emit disconnectButtonClicked();
}


void MainWidget::slotAddButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot add button clicked";
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
    desiredHeaderOrder.clear();
    model->clear();

    filterLabel->show();
    filterButton->show();
    backendManager->getHeaders();
}

void MainWidget::slotGetHeadersSuccessful(QMap<QString, QStringList> fieldsMapResponse) {
    //qDebug() << "slotGetFieldsSuccessful";
    desiredHeaderOrder = fieldsMapResponse["total"];
    alphCompMap.insert("comparable", fieldsMapResponse["comparable"]);
    alphCompMap.insert("alphabetic", fieldsMapResponse["alphabetic"]);

    compareComboBox->addItems(QStringList("-") << alphCompMap["comparable"]);
    startsWithLetterComboBox->addItems(QStringList("-") << alphCompMap["alphabetic"]);

    auto it = alphCompMap.constBegin();
    while (it != alphCompMap.constEnd()) {
        const auto& key = it.key();
        const auto& value = it.value();
        addElementDialog->setInputFields(key, value);
        ++it;
    }

    backendManager->getAllRecordings();
}

void MainWidget::slotGetAllRecordingsSuccessful(QStringList currentKeyOrder, QList<QList<QStandardItem*>> rows) {
    idLogicalIndex = currentKeyOrder.indexOf("id");
    for (const QString& compField : alphCompMap["comparable"]) {
        compHeaderData[compField].first = currentKeyOrder.indexOf(compField);
    }

    for (const QList<QStandardItem*> &rowData : rows) {
        model->appendRow(rowData);
    }
    model->setHorizontalHeaderLabels(currentKeyOrder);

    QHeaderView *header = tableView->horizontalHeader();
    for (int i = 0; i < desiredHeaderOrder.size(); ++i) {
        const QString& columnName = desiredHeaderOrder.at(i);
        int columnIndex = currentKeyOrder.indexOf(columnName);
        if (columnIndex != -1) {
            header->moveSection(columnIndex, i);
            currentKeyOrder.move(columnIndex, i);
        }
    }

    tableView->setColumnHidden(idLogicalIndex, true);
    fillColumnData(compHeaderData, model);

    //qDebug() << compHeaderData;
    //qDebug() <<"stop";

}

void MainWidget::slotDeleteAllRecordingsSuccessful(int countDeleted) {
    model->clear();
    if (!countDeleted) {
        responseLabel->setText("Nothing to delete");
    }
    else {
        responseLabel->setText("Deleted " + QString::number(countDeleted) + " recordings");
    }
}

void MainWidget::slotDeleteSelectedRecordingsSuccessful() {
    qDebug() << "slotDeleteSelectedRecordingsSuccessful";
    responseLabel->setText("Delete successful!");
    model->clear();
    backendManager->getAllRecordings();
}

void MainWidget::slotAddRecordingSuccessful() {
    qDebug() << "slotAddRecordingSuccessful";
    responseLabel->setText("Add successful!");
    model->clear();
    backendManager->getAllRecordings();
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
    responseLabel = new QLineEdit("response status will be here");
    responseLabelTimer = new QTimer();
    responseLabel->setReadOnly(true);
    responseLabel->setFocusPolicy(Qt::NoFocus);
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
    tableView->setFocusPolicy(Qt::NoFocus);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->resizeColumnsToContents();
    tableView->horizontalHeader()->setStretchLastSection(true);

    hTableViewLayout->addWidget(tableView);
}

void MainWidget::initFilterOptionsLayout() {
    filterLabel = new QLabel("Filtering options: ");
    startsWithLetterComboBox = new QComboBox();
    startsWithLetterLabel = new QLabel("starts with letter:");
    alphabetComboBox = new QComboBox();
    compareComboBox = new QComboBox();
    compareElementsComboBox = new QComboBox();
    compareElementsComboBox->addItem("-");
    leqCheckBox = new QCheckBox("And less");
    geqCheckBox = new QCheckBox("And greater");
    filterButton = new QPushButton("Filter");
    dropFilterButton = new QPushButton("Drop filter");
    dropFilterButton->hide();
    hFilterLabelLayout = new QHBoxLayout();
    hAlphabeticOptionsLayout = new QHBoxLayout();
    hComparableOptionsLayout = new QHBoxLayout();


    hFilterButtonsLayout = new QHBoxLayout();

    QStringList alphabet;
    alphabet.append("-");
    for (char letter = 'A'; letter <= 'Z'; ++letter) {
        alphabet.append(QString(letter));
    }
    alphabetComboBox->addItems(alphabet);
    hFilterLabelLayout->addStretch();
    hFilterLabelLayout->addWidget(filterLabel);
    hFilterLabelLayout->addStretch();

    hAlphabeticOptionsLayout->addStretch();
    hAlphabeticOptionsLayout->addWidget(startsWithLetterComboBox);

    hAlphabeticOptionsLayout->addWidget(startsWithLetterLabel);
    hAlphabeticOptionsLayout->addWidget(alphabetComboBox);
    hAlphabeticOptionsLayout->addStretch();
    //hFilterOptionsLayout->addStretch();
    hComparableOptionsLayout->addStretch();
    hComparableOptionsLayout->addWidget(compareComboBox);
    hComparableOptionsLayout->addWidget(compareElementsComboBox);
    hComparableOptionsLayout->addWidget(leqCheckBox);
    hComparableOptionsLayout->addWidget(geqCheckBox);
    hComparableOptionsLayout->addStretch();

    hFilterButtonsLayout->addStretch();
    hFilterButtonsLayout->addWidget(filterButton);
    hFilterButtonsLayout->addWidget(dropFilterButton);
    hFilterButtonsLayout->addStretch();
}


void MainWidget::initWidgetVLayout() {
    widgetVLayout = new QVBoxLayout();
    widgetVLayout->addLayout(hHeaderLayout);
    widgetVLayout->addStretch();
    widgetVLayout->addLayout(hResponseLabelLayout);
    widgetVLayout->addLayout(hTableViewLayout);
    widgetVLayout->addLayout(hFilterLabelLayout);
    widgetVLayout->addLayout(hAlphabeticOptionsLayout);
    widgetVLayout->addLayout(hComparableOptionsLayout);
    widgetVLayout->addLayout(hFilterButtonsLayout);
    widgetVLayout->addStretch();
    setLayout(widgetVLayout);
}

void MainWidget::initConnections() {
    connect(dropFilterButton, &QPushButton::clicked, this, [&]() {
        startsWithLetterComboBox->setCurrentIndex(0);
        alphabetComboBox->setCurrentIndex(0);
        compareComboBox->setCurrentIndex(0);
        compareElementsComboBox->setCurrentIndex(0);
        geqCheckBox->setChecked(false);
        leqCheckBox->setChecked(false);
        dropFilterButton->hide();
        filterButton->show();
    });
    connect(filterButton, &QPushButton::clicked, this, [&]() {
        //filterButton->show();
        dropFilterButton->show();
    });
    connect(leqCheckBox, &QCheckBox::stateChanged, this, [&](int state) {
        if (state == Qt::Checked) {
            geqCheckBox->setChecked(false);
        }
    });
    connect(geqCheckBox, &QCheckBox::stateChanged, this, [&](int state) {
        if (state == Qt::Checked) {
            leqCheckBox->setChecked(false);
        }
    });
    connect(responseLabel, &QLineEdit::textChanged, this, [&]() {
        responseLabelTimer->start(500);
    });
    connect(responseLabelTimer, &QTimer::timeout, this, [&]() {
        responseLabel->clear();
    });

    connect(addButton, &QPushButton::clicked, this, &MainWidget::slotAddButtonClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWidget::slotDisconnectButtonClicked);
    connect(deleteSelectedButton, &QPushButton::clicked, this, [this] () {
        QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();
        if (!selectedIndexes.empty()) {
            QVector<int> selectedIds;
            for (const QModelIndex &index : selectedIndexes) {
                QModelIndex idIndex = index.sibling(index.row(), idLogicalIndex);
                int id = idIndex.data(Qt::DisplayRole).toInt();
                selectedIds.append(id);
            }
            backendManager->deleteSelectedRecordings(selectedIds);
        }
        else {
            responseLabel->setText("Nothing to delete");
        }
    });
    connect(deleteAllButton, &QPushButton::clicked,this, [this] () {
        backendManager->deleteAllRecordings();
    });
    connect(backendManager, &BackendManager::loginSuccessful, this, &MainWidget::slotLoginSuccessful);
    connect(backendManager, &BackendManager::getHeadersSuccessful, this, &MainWidget::slotGetHeadersSuccessful);
    connect(backendManager, &BackendManager::getAllRecordingsSuccessful, this, &MainWidget::slotGetAllRecordingsSuccessful);
    connect(backendManager, &BackendManager::deleteAllRecordingsSuccessful, this, &MainWidget::slotDeleteAllRecordingsSuccessful);
    connect(backendManager, &BackendManager::deleteSelectedRecordingsSuccessful, this, &MainWidget::slotDeleteSelectedRecordingsSuccessful);
    connect(backendManager, &BackendManager::addRecordingSuccessful, this, &MainWidget::slotAddRecordingSuccessful);
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
    delete widgetVLayout;
}


void fillColumnData(QMap<QString, QPair<int, QList<int>>>& compHeaderIds, QStandardItemModel* model) {
    for (auto it = compHeaderIds.begin(); it != compHeaderIds.end(); ++it) {
        const QString& fieldName = it.key();
        int columnKey = it.value().first;

        QList<int> columnData;
        for (int row = 0; row < model->rowCount(); ++row) {
            QModelIndex index = model->index(row, columnKey);
            QStandardItem* item = model->itemFromIndex(index);
            if (item) {
                bool ok;
                int columnNumber = item->text().toInt(&ok);
                if (ok) {
                    columnData.append(columnNumber);
                }
            }
        }

        it.value().second = columnData;
    }
}

void fillComboBoxData(QMap<QString, QPair<int, QList<int>>>& compHeaderIds, QComboBox *compareComboBox, QComboBox *compareElementsComboBox) {
    if(!compareComboBox->count()) {
        for(int i = 0; i < compareComboBox->count(); i++) {

        }
    }
}




