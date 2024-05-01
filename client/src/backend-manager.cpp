#include "backend-manager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardItemModel>

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

void BackendManager::getDbFields() {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/fields"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    qDebug() << "login";
    networkManager->get(request);
}

void BackendManager::getAllDbRecordings() {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/students"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    qDebug() << "get All Db recordings";
    networkManager->get(request);
}

void BackendManager::slotRequestFinished(QNetworkReply *reply) {
    qDebug() << "slotRequestFinished (all requests handler)";
    QString url = reply->url().toString();
        if (replyHandlers.contains(url)) {
            replyHandlers[url](reply);
        } else {
            LOG(ERROR) << "BM unknown URL";
        }

        reply->deleteLater();
}

void BackendManager::handleLogin(QNetworkReply *reply) {
    qDebug() << "This output is from auth lambda inside";
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "Authentication successful";
        emit loginSuccessful();
    } else {
        qDebug() << "Authentication failed";
        emit loginFailed(reply->error());
    }
    reply->deleteLater();
}

void BackendManager::handleDBFields(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QStringList keys = {"total", "alphabetic", "comparable"};
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        if (!jsonResponse.isNull()) {
            QJsonObject jsonObject = jsonResponse.object();
            if (jsonObject.contains(keys[0]) && jsonObject.contains(keys[1]) && jsonObject.contains(keys[2])) {
                QMap<QString, QStringList> fieldsMapResponse;
                for (const auto & key : keys) {
                    QJsonArray jsonArray = jsonObject[key].toArray();
                    QStringList list;
                    for (const QJsonValue& value : jsonArray) {
                        list.append(value.toString());
                    }
                    qDebug() << "Fields List: " << list;
                    fieldsMapResponse[key] = list;
                }
                emit getFieldsSuccessful(fieldsMapResponse);
            } else {
                qDebug() << "Invalid JSON format";
                emit getFieldsFailed(QNetworkReply::ContentConflictError);
            }
        } else {
            qDebug() << "Failed to parse JSON response";
            emit getFieldsFailed(QNetworkReply::UnknownContentError);
        }
    } else {
        qDebug() << reply->errorString();
        emit getFieldsFailed(reply->error());
    }
}

void BackendManager::handleAllDbRecordings(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
        if (jsonDocument.isArray()) {
            QList<QList<QStandardItem*>> rows;
            QJsonArray jsonArray = jsonDocument.array();
            for (const QJsonValue &value : jsonArray) {
                if (value.isObject()) {
                    QJsonObject jsonObject = value.toObject();
                    QList<QStandardItem*> row;
                    for (const QString &key : jsonObject.keys()) {
                        QJsonValue jsonValue = jsonObject.value(key);
                        QStandardItem *item = nullptr;
                        if (jsonValue.isString()) {
                            item = new QStandardItem(jsonObject.value(key).toString());
                        }
                        else if (jsonValue.isDouble()) {
                            item = new QStandardItem(QString::number(jsonValue.toInt()));
                        }
                        if (item != nullptr) {
                            row << item;
                        }
                    }
                    rows.append(row);
                }
            }

        //qDebug() << "Received data:" << data;
        emit getAllDbRecordingsSuccessful(rows);
        }
        else {
            qDebug() << reply->errorString();
            emit getAllDbRecordingsFailed(reply->error());
        }
    }
}

void BackendManager::initConnections() {
    replyHandlers["http://127.0.0.1:8000/auth"] = std::bind(&BackendManager::handleLogin, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/fields"] = std::bind(&BackendManager::handleDBFields, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/students"] = std::bind(&BackendManager::handleAllDbRecordings, this, std::placeholders::_1);
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &BackendManager::slotRequestFinished);
}

BackendManager::~BackendManager() {
    delete networkManager;
}
