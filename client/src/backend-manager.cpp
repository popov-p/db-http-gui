#include "backend-manager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QUrlQuery>

void multiarg(QByteArray &ba) {};

BackendManager::BackendManager(QObject *parent) : QObject(parent)
{
  networkManager = new QNetworkAccessManager(this);
}

void BackendManager::setBaseURL(QString url)
{
  LOG(INFO) << "Setting base URL";
  baseUrl = url;
}

QString BackendManager::getActiveUser()
{
  return activeUser;
}

void BackendManager::logout()
{
  LOG(INFO) << "Logout";
  activeCredentials.clear();
  activeUser.clear();
  baseUrl.clear();
  networkManager->clearAccessCache();
}

void BackendManager::login(QString username, QString password)
{
  LOG(INFO) << "POST /auth";
  QNetworkRequest request(QUrl(baseUrl + "/auth"));
  QByteArray credentials;
  multiarg(credentials, username, ":", password);
  request.setRawHeader("Authorization", "Basic " + credentials.toBase64());
  request.setTransferTimeout(500);

  QNetworkReply *reply = networkManager->post(request, QByteArray());
  connect(reply, &QNetworkReply::finished, this, [this, reply, username, credentials]()
          {
            if (reply->error() == QNetworkReply::NoError)
            {
              activeUser = username;
              activeCredentials = credentials;
              LOG(INFO) << "POST /auth success";
              emit loginSuccessful();
            }
            else
            {
              LOG(ERROR) << "POST /auth failed, " + reply->errorString().toStdString();
              emit loginFailed(reply->error());
            }
            reply->deleteLater();
          });
}

void BackendManager::getHeaders()
{
  LOG(INFO) << "GET /fields";
  QNetworkRequest request(QUrl(baseUrl + "/fields"));
  request.setRawHeader("Authorization", "Basic " + activeCredentials.toBase64());
  request.setTransferTimeout(500);
  QNetworkReply *reply = networkManager->get(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply]()
          {
            if (reply->error() == QNetworkReply::NoError)
            {
              QStringList keys = {"total", "alphabetic", "comparable"};
              QByteArray responseData = reply->readAll();
              QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
              if (!jsonResponse.isNull())
              {
                QJsonObject jsonObject = jsonResponse.object();
                if (jsonObject.contains(keys[0]) && jsonObject.contains(keys[1]) && jsonObject.contains(keys[2]))
                {
                  QMap<QString, QStringList> fieldsMapResponse;
                  for (const auto & key : keys)
                  {
                    QJsonArray jsonArray = jsonObject[key].toArray();
                    QStringList list;
                    for (const QJsonValue& value : jsonArray)
                    {
                      list.append(value.toString());
                    }
                    fieldsMapResponse[key] = list;
                  }
                  LOG(INFO) << "GET /fields success";
                  emit getHeadersSuccessful(fieldsMapResponse);
                }
                else
                {
                  LOG(ERROR) << "GET /fields failed, ContentConflictError";
                  emit requestFailed("getHeaders, ", QNetworkReply::ContentConflictError);
                }
              }
              else
              {
                LOG(ERROR) << "GET /fields failed, UnknownContentError";
                emit requestFailed("getHeaders, ",QNetworkReply::UnknownContentError);
              }
          }
          else
          {
            LOG(ERROR) <<"GET /fields failed, " + reply->errorString().toStdString();
            emit requestFailed("getHeaders, ",reply->error());
          }
          reply->deleteLater(); });
}

void BackendManager::getAllRecordings()
{
  LOG(INFO) << "GET /students";
  QNetworkRequest request(QUrl(baseUrl + "/students"));
  request.setRawHeader("Authorization", "Basic " + activeCredentials.toBase64());
  request.setTransferTimeout(500);
  QNetworkReply *reply = networkManager->get(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply]()
          {
            if (reply->error() == QNetworkReply::NoError)
            {
              QByteArray data = reply->readAll();
              QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
              if (jsonDocument.isArray())
              {
                QList<QList<QStandardItem*>> rows;
                QJsonArray jsonArray = jsonDocument.array();
                for (const QJsonValue &value : jsonArray)
                {
                  if (value.isObject())
                  {
                    QJsonObject jsonObject = value.toObject();
                    QList<QStandardItem*> row;
                    for (const QString &key : jsonObject.keys())
                    {
                      QJsonValue jsonValue = jsonObject.value(key);
                      QStandardItem *item = nullptr;
                      if (jsonValue.isString())
                      {
                        item = new QStandardItem(jsonObject.value(key).toString());
                      }
                      else if (jsonValue.isDouble())
                      {
                        item = new QStandardItem(QString::number(jsonValue.toInt()));
                      }
                      if (item != nullptr)
                      {
                        row << item;
                      }
                    }
                    rows.append(row);
                  }
                  else
                  {
                    LOG(ERROR) << "GET /students falied, UnknownContentError";
                    emit requestFailed("getAllRecordings, ",QNetworkReply::UnknownContentError);
                  }
                }
                if (!jsonArray.isEmpty() && jsonArray.at(0).isObject())
                {
                  QJsonObject firstObject = jsonArray.at(0).toObject();
                  QStringList keys = firstObject.keys();
                  LOG(INFO) << "GET /students success";
                  emit getAllRecordingsSuccessful(keys, rows);
                }
                else
                {
                  LOG(INFO) << "GET /students success, No recordings in DB";
                  emit getAllRecordingsSuccessful({}, {});
                }
              }
              else
              {
                LOG(ERROR) << "GET /students failed, ContentConflictError";
                emit requestFailed("getAllRecordings, ", QNetworkReply::ContentConflictError);
              }
          }
          else
          {
            LOG(ERROR) << "GET /students failed, " + reply->errorString().toStdString();
            emit requestFailed("getAllRecordings, ", reply->error());
          }
          reply->deleteLater();
        });
}

void BackendManager::deleteAllRecordings()
{
  LOG(INFO) << "DELETE /delete-all";
  QNetworkRequest request(QUrl(baseUrl + "/delete-all"));
  request.setRawHeader("Authorization", "Basic " + activeCredentials.toBase64());
  request.setTransferTimeout(500);
  QNetworkReply *reply = networkManager->deleteResource(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply]()
          {
            if (reply->error() == QNetworkReply::NoError) {
              QByteArray responseData = reply->readAll();
              QString responseString = QString::fromUtf8(responseData);
              bool ok;
              int deletedCount = responseString.toInt(&ok);
              if (ok)
              {
                LOG(INFO) << "DELETE /delete-all success";
                emit deleteAllRecordingsSuccessful(deletedCount);
              }
              else
              {
                LOG(ERROR) << "DELETE /delete-all, ContentConflictError";
                emit requestFailed("deleteAllRecordings, ", QNetworkReply::ContentConflictError);
              }
            }
            else
            {
              LOG(ERROR) << "DELETE /delete-all failed, " + reply->errorString().toStdString();
              emit requestFailed("deleteAllRecordings, ", reply->error());
            }
            reply->deleteLater();
          });
}

void BackendManager::deleteSelectedRecordings(const QVector<int> &studentIds)
{
  LOG(INFO) << "DELETE /delete-selected";
  QNetworkRequest request(QUrl(baseUrl + "/delete-selected"));
  request.setRawHeader("Authorization", "Basic " + activeCredentials.toBase64());
  request.setTransferTimeout(500);
  QJsonArray jsonArray;
  for (int studentId : studentIds)
  {
    jsonArray.append(studentId);
  }
  QByteArray data = QJsonDocument(jsonArray).toJson();

  QNetworkReply *reply = networkManager->sendCustomRequest(request, "DELETE", data);
  connect(reply, &QNetworkReply::finished, this, [this, reply]()
          {
            if (reply->error() == QNetworkReply::NoError) {
              LOG(INFO) << "DELETE /delete-selected success";
              emit deleteSelectedRecordingsSuccessful();
            }
            else {
              LOG(ERROR) << "DELETE /delete-selected failed, " + reply->errorString().toStdString();
              emit requestFailed("deleteSelectedRecordings, ",reply->error());
            }
            reply->deleteLater();
          });
}

void BackendManager::addRecording(const std::map<QString, std::variant<QString, int>> &dataMap)
{
  LOG(INFO) << "POST /add";
  QNetworkRequest request(QUrl(baseUrl + "/add"));
  request.setRawHeader("Authorization", "Basic " + activeCredentials.toBase64());
  request.setTransferTimeout(500);

  QJsonObject jsonObject;
  for (const auto &pair : dataMap)
  {
    QString key = pair.first;
    std::variant<QString, int> value = pair.second;

    if (std::holds_alternative<QString>(value))
    {
      QString stringValue = std::get<QString>(value);
      jsonObject[key] = QJsonValue(stringValue);
    }
    else if (std::holds_alternative<int>(value))
    {
      int intValue = std::get<int>(value);
      jsonObject[key] = QJsonValue(intValue);
    }
  }

  QByteArray jsonData = QJsonDocument(jsonObject).toJson();

  QNetworkReply *reply = networkManager->sendCustomRequest(request, "POST", jsonData);
  connect(reply, &QNetworkReply::finished, this, [this, reply]()
          {
            if (reply->error() == QNetworkReply::NoError)
            {
              LOG(INFO) << "POST /add success";
              emit addRecordingSuccessful();
            }
            else
            {
              LOG(ERROR) << "POST /add failed," + reply->errorString().toStdString();
              emit requestFailed("addRecording, ", reply->error());
            }
            reply->deleteLater();
          });
}

void BackendManager::filteredSelect(const std::map<QString, std::variant<QString, int>> &requestArgs)
{
  LOG(INFO) << "GET /filter?";

  QUrl url(baseUrl + "/filter");
  QUrlQuery query;

  for (const auto &pair : requestArgs)
  {
    QString key = pair.first;
    std::variant<QString, int> value = pair.second;

    if (std::holds_alternative<QString>(value))
    {
      QString stringValue = std::get<QString>(value);
      query.addQueryItem(key, stringValue);
    }
    else if (std::holds_alternative<int>(value))
    {
      int intValue = std::get<int>(value);
      query.addQueryItem(key, QString::number(intValue));
    }
  }

  url.setQuery(query);

  QNetworkRequest request(url);
  request.setRawHeader("Authorization", "Basic " + activeCredentials.toBase64());
  request.setTransferTimeout(500);

  QNetworkReply *reply = networkManager->get(request);
  connect(reply, &QNetworkReply::finished, this, [this, reply]()
          {
            if (reply->error() == QNetworkReply::NoError)
            {
              QByteArray responseData = reply->readAll();
              QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
              if (!jsonDoc.isNull() && jsonDoc.isArray())
              {
                QList<int> studentIds;
                QJsonArray jsonArray = jsonDoc.array();
                foreach (const QJsonValue &value, jsonArray)
                {
                  studentIds.append(value.toInt());
                }
                LOG(INFO) << "GET /filter success";
                emit filteredSelectSuccessful(studentIds);
              }
            }
            else
            {
              LOG(ERROR) << "GET /filter failed, " + reply->errorString().toStdString();
              emit requestFailed("filteredSelect, ", reply->error());
            }
            reply->deleteLater();
          });
}
