#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <glog/logging.h>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QDialog>
#include <QStackedWidget>
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include "settings-dialog.h"
#include "connect-widget.h"
#include "backend-manager.h"
#include "main-widget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

public slots:
    void slotConnectButtonClicked();
private:
    QStackedWidget *centralStackedWidget;
    ConnectWidget *connectWidget;
    MainWidget *mainWidget;
    SettingsDialog *settingsDialog;

    void initConnections();
    void initCentralStackedWidget();

    BackendManager *backendManager;
private slots:
    void slotDisconnectButtonClicked();
    void slotConnectionSuccessful();
};


#endif // MAINWINDOW_H
