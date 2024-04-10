#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "settings-dialog.h"
#include "connect-widget.h"
#include "main-widget.h"
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QIntValidator>
#include <QDialog>
#include <QStackedWidget>
#include <glog/logging.h>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotConnectButtonClicked();
private slots:
    void slotDisconnectButtonClicked();
    void slotConnectionSuccessful();
private:
    QStackedWidget *centralStackedWidget;
    ConnectWidget *connectWidget;
    MainWidget *mainWidget;
    SettingsDialog *settingsDialog;
};


#endif // MAINWINDOW_H
