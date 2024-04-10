#include "main-widget.h"
#include <QDebug>


MainWidget::MainWidget(QWidget *parent) {
    widgetVLayout = new QVBoxLayout();

    disconnectButton = new QPushButton("Disconnect");
    disconnectButton->setFixedSize(300, 50);

    hDisconnectLayout = new QHBoxLayout();
    hDisconnectLayout->addStretch();
    hDisconnectLayout->addWidget(disconnectButton);

    responseLabel = new QLabel("response status will be here");
    responseLabel->setFixedSize(300, 20);
    responseLabel->setAlignment(Qt::AlignCenter);


    hResponseLabelLayout = new QHBoxLayout();
    hResponseLabelLayout->addWidget(responseLabel);

    widgetVLayout->addLayout(hDisconnectLayout);
    widgetVLayout->addStretch();
    widgetVLayout->addLayout(hResponseLabelLayout);
    widgetVLayout->addStretch();

    setLayout(widgetVLayout);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWidget::slotDisconnectButtonClicked);
}

MainWidget::~MainWidget() {
    delete disconnectButton;
    delete responseLabel;
    delete hDisconnectLayout;
    delete hResponseLabelLayout;
    delete widgetVLayout;
}

void MainWidget::slotDisconnectButtonClicked() {
    LOG(INFO) << "Qt: MainWidget slot disconnect button clicked";
    disconnectButton->hide();
    emit disconnectButtonClicked();
}

void MainWidget::slotOkButtonDone() {
    LOG(INFO) << "Qt: MainWidget slot connect button clicked";
    disconnectButton->show();
}

