#ifndef CONNECTWIDGET_H
#define CONNECTWIDGET_H

#include <glog/logging.h>
#include <QObject>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include "backend-manager.h"

class ConnectWidget : public QWidget {
    Q_OBJECT

public:
    ConnectWidget(BackendManager *backendManager, QWidget *parent = nullptr);
    ~ConnectWidget() = default;
public slots:
    void slotDisconnectButtonClicked();
private:
    QVBoxLayout *widgetVLayout;
    QPushButton *connectButton;
    QLineEdit *connectStatus;
    QHBoxLayout *hConnectLayout;
    QHBoxLayout *hConnectStatusLayout;
    BackendManager *backendManager = nullptr;

    void initConnectLayout();
    void initConnectStatusLayout();
    void initwidgetVLaoyut();
    void initConnections();
signals:
    void connectButtonClicked();
};

#endif // CONNECTWIDGET_H
