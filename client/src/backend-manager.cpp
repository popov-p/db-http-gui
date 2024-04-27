#include "backend-manager.h"

BackendManager::BackendManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager();
    authenticator = new QAuthenticator();
}

void BackendManager::login(QString username, QString password) {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/auth"));

    QByteArray authHeader = "Basic " + QByteArray(QString("%1:%2").arg(username).arg(password).toUtf8().toBase64());
    request.setRawHeader("Authorization", authHeader);

    QNetworkReply *reply = networkManager->post(request, QByteArray());;
}


void BackendManager::initConnections() {

}
BackendManager::~BackendManager() {
    delete networkManager;
    delete authenticator;
}
