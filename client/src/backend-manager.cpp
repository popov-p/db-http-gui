#include "backend-manager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

void multiarg(QByteArray& ba) {};

BackendManager::BackendManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager();
    initConnections();
}

void BackendManager::login(QString username, QString password) {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/auth"));
    QByteArray credentials;
    multiarg(credentials, username, ":", password);
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    qDebug() << "login";
    networkManager->post(request, QByteArray());;
}

void BackendManager::retrieveDbFields() {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/fields"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    qDebug() << "login";
    networkManager->get(request, QByteArray());;
}

void BackendManager::slotRequestFinished(QNetworkReply *reply) {
    qDebug() << "slotRequestFinished (all requests handler)";
    QString url = reply->url().toString();
        if (replyHandlers.contains(url)) {
            replyHandlers[url](reply);
        } else {
            //emit unknownUrl;
        }

        reply->deleteLater();
}

void BackendManager::handleLogin(QNetworkReply *reply) {
    qDebug() << "This output is from auth lambda inside";
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Authentication successful";
        emit authSuccessful();
    } else {
        qDebug() << "Authentication failed";
        emit authFailed(reply->error());
    }
    reply->deleteLater();
}

void BackendManager::initConnections() {
    replyHandlers["http://127.0.0.1:8000/auth"] = std::bind(&BackendManager::handleLogin, this, std::placeholders::_1);


    replyHandlers["http://127.0.0.1:8000/fields"] = [this] (QNetworkReply *reply) {
        qDebug() << "This output is from Fields Lambda inside";
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            if (!jsonResponse.isNull()) {
                QJsonObject jsonObject = jsonResponse.object();
                if (jsonObject.contains("total") && jsonObject.contains("alphabetic") && jsonObject.contains("comparable")) {
                    QJsonArray totalArray = jsonObject["total"].toArray();
                    QStringList totalList;
                    for (const QJsonValue &value : totalArray) {
                        totalList.append(value.toString());
                    }
                    qDebug() << "TotalList: " << totalList;

                    QJsonArray alphabeticArray = jsonObject["alphabetic"].toArray();
                    QStringList alphabeticList;
                    for (const QJsonValue &value : alphabeticArray) {
                        alphabeticList.append(value.toString());
                    }
                    qDebug() << "AlphabeticList: " << alphabeticList;
                    QJsonArray comparableArray = jsonObject["comparable"].toArray();
                    QStringList comparableList;
                    for (const QJsonValue &value : comparableArray) {
                        comparableList.append(value.toString());
                    }
                    qDebug() << "ComparableList: " << comparableList;
                    //emit wtf(/**/);

                } else {
                    qDebug() << "Invalid JSON format";
                }
            } else {
                qDebug() << "Failed to parse JSON response";
            }
        } else {
            qDebug() << "Network error:" << reply->errorString();
        }
    };

    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &BackendManager::slotRequestFinished);
}

BackendManager::~BackendManager() {
    delete networkManager;
}
