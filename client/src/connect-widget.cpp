#include "connect-widget.h"
#include <QDebug>
#include <QTimer>
#include <glog/logging.h>


ConnectWidget::ConnectWidget(BackendManager *backendManager, QWidget *parent) :
                                                                    QWidget(parent), backendManager(backendManager) {
    initConnectLayout();
    initConnectStatusLayout();
    initwidgetVLaoyut();
    initConnections();
}

void ConnectWidget::initConnectLayout() {
    connectButton = new QPushButton("Connect", this);
    hConnectLayout = new QHBoxLayout();

    connectButton->setFixedSize(300, 50);
    hConnectLayout->addWidget(connectButton);
}

void ConnectWidget::initConnectStatusLayout() {
    connectStatus = new QLineEdit(this);
    connectStatus->setReadOnly(true);
    connectStatus->setFocusPolicy(Qt::NoFocus);
    connectStatus->setAlignment(Qt::AlignCenter);
    connectStatus->setFixedSize(300, 20);
    connectStatus->hide();
    hConnectStatusLayout = new QHBoxLayout();

    hConnectStatusLayout->setAlignment(Qt::AlignCenter);
    hConnectStatusLayout->addWidget(connectStatus);
}

void ConnectWidget::initwidgetVLaoyut() {
    widgetVLayout = new QVBoxLayout(this);
    widgetVLayout->addStretch();
    widgetVLayout->addLayout(hConnectLayout);
    widgetVLayout->addLayout(hConnectStatusLayout);
    widgetVLayout->addStretch();

    setLayout(widgetVLayout);
}

void ConnectWidget::initConnections() {
    connect(connectButton, &QPushButton::clicked, this, [this](){
                    connectStatus->setText("");
                    connectButton->show();
                    emit connectButtonClicked();
    });
    connect(backendManager, &BackendManager::loginSuccessful, this, [this] () {
        connectButton->hide();
        connectStatus->show();
        connectStatus->setText("Connected!");
    });
}

void ConnectWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Slot disconnect button clicked";
    connectStatus->clear();
    connectStatus->hide();
    connectButton->show();
}
