#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QIntValidator>
#include <QComboBox>

MainWindow::MainWindow(QString cfgPath, QWidget *parent)
    : QMainWindow(parent)
{
  setGeometry(100, 100, 800, 600);
  backendManager = new BackendManager(this);
  connectWidget = new ConnectWidget(backendManager, this);
  mainWidget = new MainWidget(backendManager, this);
  settingsDialog = new SettingsDialog(backendManager, cfgPath, this);

  initCentralStackedWidget();
  initConnections();
}

void MainWindow::slotConnectButtonClicked()
{
  LOG(INFO) << "Connect button clicked";
  settingsDialog->exec();
}

void MainWindow::slotDisconnectButtonClicked()
{
  LOG(INFO) << "Disconnect button clicked";
  QTimer *timer = new QTimer();
  connect(timer, &QTimer::timeout, this, [this, timer]()
          {
            centralStackedWidget->setCurrentWidget(connectWidget);
            timer->deleteLater();
          });
  timer->start(500);
}

void MainWindow::slotConnectionSuccessful()
{
  LOG(INFO) << "Connection successful";
  QTimer *timer = new QTimer();
  connect(timer, &QTimer::timeout, this, [this, timer]()
          {
            centralStackedWidget->setCurrentWidget(mainWidget);
            timer->deleteLater();
          });
  timer->start(500);
}

void MainWindow::initCentralStackedWidget()
{
  centralStackedWidget = new QStackedWidget(this);
  setCentralWidget(centralStackedWidget);
  centralStackedWidget->addWidget(connectWidget);
  centralStackedWidget->addWidget(mainWidget);
}

void MainWindow::initConnections()
{
  connect(connectWidget, &ConnectWidget::connectButtonClicked,
          this, &MainWindow::slotConnectButtonClicked);
  connect(backendManager, &BackendManager::loginSuccessful,
          this, &MainWindow::slotConnectionSuccessful);
  connect(settingsDialog, &SettingsDialog::okButtonDone,
          mainWidget, &MainWidget::slotLoginSuccessful);
  connect(mainWidget, &MainWidget::disconnectButtonClicked,
          this, &MainWindow::slotDisconnectButtonClicked);
  connect(mainWidget, &MainWidget::disconnectButtonClicked,
          connectWidget, &ConnectWidget::slotDisconnectButtonClicked);
}
