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
    //qDebug() << "login";
    networkManager->post(request, QByteArray());;
}

void BackendManager::getHeaders() {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/fields"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    //qDebug() << "login";
    networkManager->get(request);
}

void BackendManager::getAllRecordings() {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/students"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    networkManager->get(request);
}

void BackendManager::deleteAllRecordings() {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/delete-all"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    request.setTransferTimeout(500);
    networkManager->deleteResource(request);
}

void BackendManager::deleteSelectedRecordings(const QVector<int>& studentIds) {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/delete-selected"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");
    request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
    QJsonArray jsonArray;
    for (int studentId : studentIds) {
        jsonArray.append(studentId);
    }
    QByteArray data = QJsonDocument(jsonArray).toJson();

    networkManager->sendCustomRequest(request, "DELETE", data);
}

void BackendManager::addRecording(const std::map<QString, std::variant<QString, int>>& dataMap) {
    QNetworkRequest request(QUrl("http://127.0.0.1:8000/add"));
    QByteArray credentials;
    multiarg(credentials, "pavel", ":", "popov");

    QJsonObject jsonObject;

    for (const auto& pair : dataMap) {
        QString key = pair.first;
        std::variant<QString, int> value = pair.second;

        if (std::holds_alternative<QString>(value)) {
            QString stringValue = std::get<QString>(value);
            jsonObject[key] = QJsonValue(stringValue);
        }
        else if (std::holds_alternative<int>(value)) {
            int intValue = std::get<int>(value);
            jsonObject[key] = QJsonValue(intValue);
        }
    }
    QJsonDocument jsonDocument(jsonObject);
    QByteArray jsonData = QJsonDocument(jsonObject).toJson(QJsonDocument::Indented);

    networkManager->sendCustomRequest(request, "POST", jsonData);
}

void BackendManager::slotRequestFinished(QNetworkReply *reply) {
    //qDebug() << "slotRequestFinished (all requests handler)";
    QString url = reply->url().toString();
        if (replyHandlers.contains(url)) {
            replyHandlers[url](reply);
        } else {
            LOG(ERROR) << "BM unknown URL";
        }

        reply->deleteLater();
}

void BackendManager::handleLogin(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        emit loginSuccessful();
    } else {
        emit loginFailed(reply->error());
    }
    reply->deleteLater();
}

void BackendManager::handleGetHeaders(QNetworkReply *reply) {
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
                    fieldsMapResponse[key] = list;
                }
                emit getHeadersSuccessful(fieldsMapResponse);
            } else {
                emit getHeadersFailed(QNetworkReply::ContentConflictError);
            }
        } else {
            emit getHeadersFailed(QNetworkReply::UnknownContentError);
        }
    } else {
        emit getHeadersFailed(reply->error());
    }
}

void BackendManager::handleGetAllRecordings(QNetworkReply *reply) {
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
                else {
                    emit getAllRecordingsFailed(QNetworkReply::UnknownContentError);
                }
            }
            if (!jsonArray.isEmpty() && jsonArray.at(0).isObject()) {
                QJsonObject firstObject = jsonArray.at(0).toObject();
                QStringList keys = firstObject.keys();
                qDebug() << "List of keys:" << keys;
                emit getAllRecordingsSuccessful(keys, rows);
            }
        }
        else {
            emit getAllRecordingsFailed(QNetworkReply::ContentConflictError);
        }
    }
    else {
        emit getAllRecordingsFailed(reply->error());
    }
}

void BackendManager::handleDeleteAllRecordings(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QString responseString = QString::fromUtf8(responseData);
        bool ok;
        int deletedCount = responseString.toInt(&ok);
        if (ok) {
            emit deleteAllRecordingsSuccessful(deletedCount);
        } else {
            emit deleteAllRecordingsFailed(QNetworkReply::ContentConflictError);
        }
    } else {
        emit deleteAllRecordingsFailed(reply->error());
    }
}

void BackendManager::handleDeleteSelectedRecordings(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        emit deleteSelectedRecordingsSuccessful();
    }
    else {
        emit deleteSelectedRecordingsFailed(reply->error());
    }
}

void BackendManager::handleAddRecording(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        emit addRecordingSuccessful();
    }
    else {
        emit addRecordingFailed(reply->error());
    }
}

void BackendManager::initConnections() {
    replyHandlers["http://127.0.0.1:8000/auth"] = std::bind(&BackendManager::handleLogin, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/fields"] = std::bind(&BackendManager::handleGetHeaders, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/students"] = std::bind(&BackendManager::handleGetAllRecordings, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/delete-all"] = std::bind(&BackendManager::handleDeleteAllRecordings, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/delete-selected"] = std::bind(&BackendManager::handleDeleteSelectedRecordings, this, std::placeholders::_1);
    replyHandlers["http://127.0.0.1:8000/add"] = std::bind(&BackendManager::handleAddRecording, this, std::placeholders::_1);
    QObject::connect(networkManager, &QNetworkAccessManager::finished, this, &BackendManager::slotRequestFinished);
}

BackendManager::~BackendManager() {
    delete networkManager;
}
