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
    connectButton = new QPushButton("Connect");
    hConnectLayout = new QHBoxLayout();

    connectButton->setFixedSize(300, 50);
    hConnectLayout->addWidget(connectButton);
}

void ConnectWidget::initConnectStatusLayout() {
    connectStatus = new QLabel();
    hConnectStatusLayout = new QHBoxLayout();

    hConnectStatusLayout->setAlignment(Qt::AlignCenter);
    hConnectStatusLayout->addWidget(connectStatus);
}

void ConnectWidget::initwidgetVLaoyut() {
    widgetVLayout = new QVBoxLayout();
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
        connectStatus->setText("Connected!");
    });
}


ConnectWidget::~ConnectWidget() {
    delete connectButton;
    delete connectStatus;
    delete hConnectLayout;
    delete hConnectStatusLayout;
    delete widgetVLayout;
}

//void ConnectWidget::slotOkButtonDone() {
//    LOG(INFO) << "Qt: ConnectWidget slot ok button clicked (connection success)";
//    connectButton->hide();
//    try {
//        connectStatus->setText("Connected!");
//        //LOG(INFO) << "Qt: connection with db established";
//        emit connectionSuccessful();
//    }
//    catch(...) { /*TODO: Fix this, connection establishes
//                         in a different way, so catch(...) is no more required*/
//        LOG(ERROR) << "Qt: connection to broker failed";
//        connectStatus->setText("Connection Failed!");
//        QTimer *timer = new QTimer();
//        connect(timer, &QTimer::timeout, this, [this, timer]() {
//            connectStatus->setText("");
//            connectButton->show();
//            timer->deleteLater();
//        });
//        timer->start(500);
//    }
//}

void ConnectWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Qt: ConnectWidget slot disconnect button clicked";
    connectStatus->clear();
    connectButton->show();
}
