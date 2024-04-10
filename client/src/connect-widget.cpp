#include "connect-widget.h"
#include <QDebug>
#include <QTimer>
#include <glog/logging.h>


ConnectWidget::ConnectWidget(QWidget *parent) {
    widgetVLayout = new QVBoxLayout();
    connectButton = new QPushButton("Connect");
    connectButton->setFixedSize(300, 50);

    hConnectLayout = new QHBoxLayout();
    hConnectLayout->addWidget(connectButton);

    hConnectStatusLayout = new QHBoxLayout();
    hConnectStatusLayout->setAlignment(Qt::AlignCenter);
    connectStatus = new QLabel();
    hConnectStatusLayout->addWidget(connectStatus);

    widgetVLayout->addStretch();
    widgetVLayout->addLayout(hConnectLayout);
    widgetVLayout->addLayout(hConnectStatusLayout);
    widgetVLayout->addStretch();

    setLayout(widgetVLayout);

    connect(connectButton, &QPushButton::clicked, this, &ConnectWidget::connectButtonClicked);
}

ConnectWidget::~ConnectWidget() {
    delete connectButton;
    delete connectStatus;
    delete hConnectLayout;
    delete hConnectStatusLayout;
    delete widgetVLayout;
}

void ConnectWidget::slotOkButtonDone() {
    LOG(INFO) << "Qt: ConnectWidget slot ok button clicked (connection success)";
    connectButton->hide();
    try {
        connectStatus->setText("Connected!");
        LOG(INFO) << "Qt: connection with broker established";
        emit connectionSuccessful();
    }
    catch(...) {
        LOG(ERROR) << "Qt: connection to broker failed";
        connectStatus->setText("Connection Failed!");
        QTimer *timer = new QTimer();
        connect(timer, &QTimer::timeout, [this, timer]() {
            connectStatus->setText("");
            connectButton->show();
            timer->deleteLater();
        });
        timer->start(500);
    }
}

void ConnectWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Qt: ConnectWidget slot disconnect button clicked";
    connectStatus->clear();
    connectButton->show();
}
