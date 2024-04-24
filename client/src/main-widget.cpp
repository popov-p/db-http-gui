#include "main-widget.h"
#include <QDebug>


MainWidget::MainWidget(QWidget *parent) {
    addElementDialog = new AddElementDialog();
    widgetVLayout = new QVBoxLayout();

    disconnectButton = new QPushButton("Disconnect");
    disconnectButton->setFixedSize(100, 50);

    addButton = new QPushButton("Add");
    addButton->setFixedSize(100, 50);

    deleteButton = new QPushButton("Delete \n selected");
    deleteButton->setFixedSize(100, 50);

    deleteAllButton = new QPushButton("Delete all");
    deleteAllButton->setFixedSize(100, 50);



    hHeaderLayout = new QHBoxLayout();
    hHeaderLayout->addWidget(addButton);
    hHeaderLayout->addWidget(deleteButton);
    hHeaderLayout->addWidget(deleteAllButton);
    hHeaderLayout->addStretch();
    hHeaderLayout->addWidget(disconnectButton);

    responseLabel = new QLabel("response status will be here");
    responseLabel->setFixedSize(300, 20);
    responseLabel->setAlignment(Qt::AlignCenter);


    hResponseLabelLayout = new QHBoxLayout();
    hResponseLabelLayout->addWidget(responseLabel);

    /*database layout preparations*/
    model = new QStandardItemModel();
    tableView = new QTableView();
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setModel(model);
    model->setHorizontalHeaderLabels(QStringList() << "Name" << "Values");

    //test filling


    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 2; ++col) {
            QString data = QString("Row %1, Col %2").arg(row).arg(col);
            QStandardItem *item = new QStandardItem(data);
            item->setEditable(false);
            model->setItem(row, col, item);
        }
    } //this will be commented later

    QStringList names = {"Петров", "Иванов", "Попов", "Сидоров", "Петровский"};
    for (int row = 0; row < names.size(); ++row) {
            QModelIndex index = model->index(row, 0, QModelIndex());
            model->setData(index, names[row]);
        }
    tableView->resizeColumnsToContents();

    hTableViewLayout = new QHBoxLayout();
    hTableViewLayout->addWidget(tableView);


    startsWithLetterComboBox = new QComboBox();
    startsWithLetterComboBox->addItem("Last name");
    startsWithLetterComboBox->addItem("First name");
    startsWithLetterComboBox->addItem("Patronymic");

    startsWithLetterLabel = new QLabel("Starts with letter:");
    startsWithLetterLineEdit = new QLineEdit();

    compareComboBox = new QComboBox();
    compareComboBox->addItem("Course");
    compareComboBox->addItem("Group");

    leqCheckBox = new QCheckBox("And less");
    geqCheckBox = new QCheckBox("And greater");

    hFilterOptionsLayout = new QHBoxLayout();
    hFilterOptionsLayout->addWidget(startsWithLetterComboBox);
    hFilterOptionsLayout->addWidget(startsWithLetterLabel);
    hFilterOptionsLayout->addWidget(startsWithLetterLineEdit);
    hFilterOptionsLayout->addStretch();
    hFilterOptionsLayout->addWidget(compareComboBox);
    hFilterOptionsLayout->addWidget(leqCheckBox);
    hFilterOptionsLayout->addWidget(geqCheckBox);





    widgetVLayout->addLayout(hHeaderLayout);
    widgetVLayout->addStretch();
    widgetVLayout->addLayout(hResponseLabelLayout);
    widgetVLayout->addLayout(hTableViewLayout);
    widgetVLayout->addLayout(hFilterOptionsLayout);
    widgetVLayout->addStretch();

    setLayout(widgetVLayout);
    connect(addButton, &QPushButton::clicked, this, &MainWidget::slotAddButtonClicked);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWidget::slotDisconnectButtonClicked);
    connect(deleteButton, &QPushButton::clicked, [this]() {
            QModelIndexList selectedIndexes = tableView->selectionModel()->selectedRows();

            for (int i = selectedIndexes.count() - 1; i >= 0; --i) {
                model->removeRow(selectedIndexes.at(i).row());
            }
        });
    connect(deleteAllButton, &QPushButton::clicked, [this]() {
            model->clear();
        });
}

MainWidget::~MainWidget() {
    delete disconnectButton;
    delete responseLabel;
    delete hHeaderLayout;
    delete hResponseLabelLayout;
    delete hTableViewLayout;
    delete widgetVLayout;
    delete model;
    delete tableView;
}

void MainWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot disconnect button clicked";
    disconnectButton->hide();
    addButton->hide();
    deleteButton->hide();
    deleteAllButton->hide();
    tableView->hide();
    emit disconnectButtonClicked();
}

void MainWidget::slotOkButtonDone() {
    LOG(INFO) << "Qt: MainWidget slot connect button clicked";
    disconnectButton->show();
    addButton->show();
    deleteButton->show();
    deleteAllButton->show();
    tableView->show();
}

void MainWidget::slotAddButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot add button clicked";
    addElementDialog->exec();
}








