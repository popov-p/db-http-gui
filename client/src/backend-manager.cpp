#include "backend-manager.h"

BackendManager::BackendManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager();
    initConnections();
}

void BackendManager::login(QString username, QString password) {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/auth"));
    QByteArray authHeader = "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toUtf8().toBase64());
    request.setRawHeader("Authorization", authHeader);
    request.setTransferTimeout(500);
    qDebug() << "login";
    networkManager->post(request, QByteArray());;
}

void BackendManager::slotRequestFinished(QNetworkReply *reply) {
    qDebug() << "slotRequestFinished (all requests handler)";
    QString url = reply->url().toString();
        if (replyHandlers.contains(url)) {
            replyHandlers[url](reply);
        }

        reply->deleteLater();
}

void BackendManager::initConnections() { 
    replyHandlers["http://127.0.0.1:8000/auth"] = [this](QNetworkReply *reply) {
            qDebug() << "This output is from auth lambda inside";
            if (reply->error() == QNetworkReply::NoError) {
                qDebug() << "Authentication successful";
                emit authSuccessful();
            } else {
                qDebug() << "Authentication failed";
                emit authFailed(reply->error());
            }
            reply->deleteLater();
    };

    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &BackendManager::slotRequestFinished);
}

BackendManager::~BackendManager() {
    delete networkManager;
}
