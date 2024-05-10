#include "main-widget.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

MainWidget::MainWidget(BackendManager* backendManager, QWidget *parent) : QWidget(parent) , backendManager(backendManager) {
    initHeaderLayout();
    initResponseLabelLayout();
    initTableViewLayout();
    initFilterOptionsLayout();
    initWidgetVLayout();
    initConnections();
}

void MainWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Disconnect button clicked";
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
    geqCheckBox->hide();
    leqCheckBox->hide();
    model->clear();
    backendManager->logout();
    emit disconnectButtonClicked();
}

void MainWidget::slotAddButtonClicked() {
    LOG(INFO) << "Add button clicked";
    addElementDialog = new AddElementDialog(backendManager, this);

    QObject::connect(addElementDialog, &QDialog::finished, addElementDialog, [this]() {
        LOG(INFO) << "Add element dialog finished, deleting temporary object";
        addElementDialog->deleteLater();
    });

    auto it = alphCompMap.constBegin();
    while (it != alphCompMap.constEnd()) {
        const auto& key = it.key();
        const auto& value = it.value();
        addElementDialog->setInputFields(key, value);
        ++it;
    }

    addElementDialog->show();
    addElementDialog->exec();
}

void MainWidget::slotUpdateCompareElementsComboBox(const QString& changedField) {
    LOG(INFO) << "Updating compare elements combo-box";
    compareElementsComboBox->clear();
    compareElementsComboBox->addItem("-");
    if(changedField != "-") {
        leqCheckBox->show();
        geqCheckBox->show();
        const QList<int>& intList = compHeaderData[changedField].second;
        for (const int & value : intList) {
            compareElementsComboBox->addItem(QString::number(value));
        }
    }
    else {
        leqCheckBox->hide();
        geqCheckBox->hide();
        leqCheckBox->setChecked(false);
        geqCheckBox->setChecked(false);
        compareElementsComboBox->clear();
    }
}

void MainWidget::slotLoginSuccessful() {
    LOG(INFO) << "Login successful";
    usernameLabel->setText(backendManager->getActiveUser());
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
    LOG(INFO) << "Get headers successful";
    desiredHeaderOrder = fieldsMapResponse["total"];
    alphCompMap.insert("comparable", fieldsMapResponse["comparable"]);
    alphCompMap.insert("alphabetic", fieldsMapResponse["alphabetic"]);

    compareComboBox->addItems(QStringList("-") << alphCompMap["comparable"]);
    startsWithLetterComboBox->addItems(QStringList("-") << alphCompMap["alphabetic"]);

    backendManager->getAllRecordings();
}

void MainWidget::slotGetAllRecordingsSuccessful(QStringList currentKeyOrder, QList<QList<QStandardItem*>> rows) {
    LOG(INFO) << "Get all recordings successful";
    idLogicalIndex = currentKeyOrder.indexOf("id");
    photoLogicalIndex = currentKeyOrder.indexOf("photo");
    compHeaderData.clear();
    for (const QString& compField : alphCompMap["comparable"]) {
        compHeaderData[compField].first = currentKeyOrder.indexOf(compField);
    }

    for (const QList<QStandardItem*> &rowData : rows) {
        if (photoLogicalIndex >= 0 && photoLogicalIndex < rowData.size()) {
            QStandardItem *photoItem = rowData.at(photoLogicalIndex);
            QString base64String = photoItem->text();
            photoItem->setText("");
            QByteArray imageData = QByteArray::fromBase64(base64String.toUtf8());
            QImage image = QImage::fromData(imageData);
            QPixmap pixmap = QPixmap::fromImage(image);
            photoItem->setData(pixmap, Qt::DecorationRole);
        }
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
    slotClearComparableFields();
}

void MainWidget::slotDeleteAllRecordingsSuccessful(int countDeleted) {
    model->clear();
    if (!countDeleted) {
        LOG(ERROR) << "Delete all recodrings, nothing to delete";
        responseLabel->setText("Nothing to delete");
    }
    else {
        LOG(INFO) << "Deleted " + QString::number(countDeleted).toStdString() + " recordings";
        responseLabel->setText("Deleted " + QString::number(countDeleted) + " recordings");
    }
}

void MainWidget::slotDeleteSelectedRecordingsSuccessful() {
    LOG(INFO) << "Delete selected recordings success";
    responseLabel->setText("Delete successful!");
    model->clear();
    backendManager->getAllRecordings();
}

void MainWidget::slotAddRecordingSuccessful() {
    LOG(INFO) << "Add selected recordings success";
    responseLabel->setText("Add successful!");
    model->clear();
    backendManager->getAllRecordings();
}

void MainWidget::slotClearComparableFields() {
    LOG(INFO) << "Comparable fields cleanup";
    startsWithLetterComboBox->setCurrentIndex(0);
    alphabetComboBox->setCurrentIndex(0);
    compareComboBox->setCurrentIndex(0);
    compareElementsComboBox->setCurrentIndex(0);
    geqCheckBox->setChecked(false);
    leqCheckBox->setChecked(false);
    dropFilterButton->hide();
    filterButton->show();
}

void MainWidget::slotFilterSelectSuccessful(QList<int> studentIds) {
    LOG(INFO) << "Filtered select success";
    int rowCount = model->rowCount();

    for (int row = 0; row < rowCount; ++row) {
        QModelIndex index = model->index(row, idLogicalIndex);
        int id = model->data(index).toInt();

        if (!studentIds.contains(id)) {
            tableView->hideRow(row);
        }
    }
    for (const auto& id : studentIds) {
        for (int row = 0; row < rowCount; ++row) {
            QModelIndex index = model->index(row, idLogicalIndex);
            int rowId = model->data(index).toInt();
            if (rowId == id) {
                tableView->showRow(row);
                break;
            }
        }
    }
}

void MainWidget::slotFilterButtonClicked() {
    LOG(INFO) << "Filter button clicked";
    std::map<QString, std::variant<QString, int>> requestArgs;
    bool alphabereticFilterActive = startsWithLetterComboBox->currentText() != "-";
    bool alphabeticFilterCorrect = alphabetComboBox->currentText() != "-";
    bool comparableFilterActive = compareComboBox->currentText() != "-";
    bool comparableFilterCorrect = compareElementsComboBox->currentText() != "-";
    if (!alphabereticFilterActive && !comparableFilterActive) {
        LOG(ERROR) << "Filtering options are empty";
        responseLabel->setText("Nothing to filter!");
        dropFilterButton->hide();
        model->clear();
        backendManager->getAllRecordings();
    }
    else if (alphabereticFilterActive && !comparableFilterActive) {
        if (alphabeticFilterCorrect) {
            LOG(INFO) << "Alphabetic fileds filtering";
            requestArgs[startsWithLetterComboBox->currentText()] = alphabetComboBox->currentText();
            backendManager->filteredSelect(requestArgs);
            dropFilterButton->show();
        }
        else {
            LOG(ERROR) << "Alphabetic fields, incorrect filtering options";
            requestArgs.clear();
            responseLabel->setText("Choose correct letter");
        }
    }
    else if (!alphabereticFilterActive && comparableFilterActive) {
        if (comparableFilterCorrect) {
            LOG(INFO) << "Comparable Fields filtering";
            requestArgs[compareComboBox->currentText()] = compareElementsComboBox->currentText().toInt();
            requestArgs["and_less"] = leqCheckBox->isChecked();
            requestArgs["and_greater"] = geqCheckBox->isChecked();
            backendManager->filteredSelect(requestArgs);
            dropFilterButton->show();
        }
        else {
            LOG(ERROR) << "Comparable fields, incorrect filtering options";
            requestArgs.clear();
            responseLabel->setText("Choose correct comparable field");
        }
    }
    else {
        if (alphabeticFilterCorrect && comparableFilterCorrect) {
            LOG(INFO) << "Both filtering types are active";
            requestArgs[startsWithLetterComboBox->currentText()] = alphabetComboBox->currentText();
            requestArgs[compareComboBox->currentText()] = compareElementsComboBox->currentText().toInt();
            requestArgs["and_less"] = leqCheckBox->isChecked();
            requestArgs["and_greater"] = geqCheckBox->isChecked();
            backendManager->filteredSelect(requestArgs);
            dropFilterButton->show();
        }
        else {
            LOG(ERROR) << "Error setting filtering options";
            requestArgs.clear();
            responseLabel->setText("Bad args");
        }
    }
}

void MainWidget::initHeaderLayout() {
    addButton = new QPushButton("Add", this);
    deleteSelectedButton = new QPushButton("Delete \n selected", this);
    deleteAllButton = new QPushButton("Delete all", this);
    disconnectButton = new QPushButton("Disconnect", this);
    usernameLabel = new QLabel("upd later", this);
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
    responseLabel = new QLineEdit("response status will be here", this);
    responseLabelTimer = new QTimer();
    responseLabel->setReadOnly(true);
    responseLabel->setFocusPolicy(Qt::NoFocus);
    hResponseLabelLayout = new QHBoxLayout();

    responseLabel->setFixedSize(300, 20);
    responseLabel->setAlignment(Qt::AlignCenter);

    hResponseLabelLayout->addWidget(responseLabel);
}

void MainWidget::initTableViewLayout() {
    model = new QStandardItemModel(this);
    tableView = new QTableView(this);
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
    filterLabel = new QLabel("Filtering options: ", this);
    startsWithLetterComboBox = new QComboBox(this);
    startsWithLetterLabel = new QLabel("`s first letter: ", this);
    alphabetComboBox = new QComboBox(this);
    compareComboBox = new QComboBox(this);
    compareElementsComboBox = new QComboBox(this);
    compareElementsComboBox->addItem("-");
    leqCheckBox = new QCheckBox("And less", this);
    geqCheckBox = new QCheckBox("And greater", this);
    leqCheckBox->hide();
    geqCheckBox->hide();
    filterButton = new QPushButton("Filter", this);
    dropFilterButton = new QPushButton("Drop filter", this);
    dropFilterButton->hide();
    hFilterLabelLayout = new QHBoxLayout();
    hAlphabeticOptionsLayout = new QHBoxLayout();
    hComparableOptionsLayout = new QHBoxLayout();


    hFilterButtonsLayout = new QHBoxLayout();

    hFilterLabelLayout->addStretch();
    hFilterLabelLayout->addWidget(filterLabel);
    hFilterLabelLayout->addStretch();

    hAlphabeticOptionsLayout->addStretch();
    hAlphabeticOptionsLayout->addWidget(startsWithLetterComboBox);

    hAlphabeticOptionsLayout->addWidget(startsWithLetterLabel);
    hAlphabeticOptionsLayout->addWidget(alphabetComboBox);
    hAlphabeticOptionsLayout->addStretch();

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
    widgetVLayout = new QVBoxLayout(this);
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
    connect(startsWithLetterComboBox, &QComboBox::currentTextChanged, this, [this]() {
        if(startsWithLetterComboBox->currentText() == "-") {
            alphabetComboBox->clear();
        }
        else {
            alphabetComboBox->clear();
            QStringList alphabet;
            alphabet.append("-");
            for (char letter = 'A'; letter <= 'Z'; ++letter) {
                alphabet.append(QString(letter));
            }
            alphabetComboBox->addItems(alphabet);
        }
    });
    connect(compareComboBox, &QComboBox::currentTextChanged, this, &MainWidget::slotUpdateCompareElementsComboBox);
    connect(dropFilterButton, &QPushButton::clicked, this, [this](){
        slotClearComparableFields();
        model->clear();
        backendManager->getAllRecordings();
    });
    connect(filterButton, &QPushButton::clicked, this, &MainWidget::slotFilterButtonClicked);
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
        responseLabelTimer->start(1000);
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
    connect(backendManager, &BackendManager::filteredSelectSuccessful, this, &MainWidget::slotFilterSelectSuccessful);
    connect(backendManager, &BackendManager::requestFailed, this, [this](QString requestName, QNetworkReply::NetworkError errcode){
        responseLabel->setText(requestName + QString::number(errcode));
    });
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
                    if (!columnData.contains(columnNumber)) {
                        columnData.append(columnNumber);
                    }
                }
            }
        }
        it.value().second = columnData;
    }
}
