#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include "backend-manager.h"

#include <QObject>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <glog/logging.h>

class ConnectWidget : public QWidget {
    Q_OBJECT

public:
    ConnectWidget(BackendManager *backendManager, QWidget *parent = nullptr);
    ~ConnectWidget();
public slots:
    //void slotOkButtonDone();
    void slotDisconnectButtonClicked();
private:
    QVBoxLayout *widgetVLayout;
    QPushButton *connectButton;
    QLabel *connectStatus;
    QHBoxLayout *hConnectLayout;
    QHBoxLayout *hConnectStatusLayout;
    BackendManager *backendManager = nullptr;

    void initConnectLayout();
    void initConnectStatusLayout();
    void initwidgetVLaoyut();
    void initConnections();
signals:
    void connectButtonClicked();
    //void connectionSuccessful();
};


#endif // CONNECTWIDGET_H
