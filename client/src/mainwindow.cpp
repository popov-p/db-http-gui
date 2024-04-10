#include "mainwindow.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QIntValidator>
#include <QComboBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setGeometry(100,100, 800, 600);
    connectWidget = new ConnectWidget();
    mainWidget = new MainWidget();
    settingsDialog = new SettingsDialog();

    centralStackedWidget = new QStackedWidget();
    setCentralWidget(centralStackedWidget);
    centralStackedWidget->addWidget(connectWidget);
    centralStackedWidget->addWidget(mainWidget);

    connect(connectWidget, &ConnectWidget::connectButtonClicked,
            this, &MainWindow::slotConnectButtonClicked);
    connect(connectWidget, &ConnectWidget::connectionSuccessful,
            this, &MainWindow::slotConnectionSuccessful);
    connect(settingsDialog, &SettingsDialog::okButtonDone,
            mainWidget, &MainWidget::slotOkButtonDone);
    connect(settingsDialog, &SettingsDialog::okButtonDone,
            connectWidget, &ConnectWidget::slotOkButtonDone);
    connect(mainWidget, &MainWidget::disconnectButtonClicked,
            this, &MainWindow::slotDisconnectButtonClicked);
    connect (mainWidget, &MainWidget::disconnectButtonClicked,
            connectWidget, &ConnectWidget::slotDisconnectButtonClicked);
}

MainWindow::~MainWindow()
{
    delete connectWidget;
    delete mainWidget;
    delete centralStackedWidget;
    delete settingsDialog;
}

void MainWindow::slotConnectButtonClicked() {
    LOG(INFO) << "Qt: MainWindow connect button clicked";
    settingsDialog->exec();
}

void MainWindow::slotDisconnectButtonClicked() {
    LOG(INFO) << "Qt: MainWindow slot disconnect button clicked";
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        centralStackedWidget->setCurrentWidget(connectWidget);
        timer->deleteLater();
    });
    timer->start(500);
}

void MainWindow::slotConnectionSuccessful() {
    LOG(INFO) << "Qt: MainWindow slot connection successful";
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this, timer]() {
        centralStackedWidget->setCurrentWidget(mainWidget);
        timer->deleteLater();
    });
    timer->start(500);

}
