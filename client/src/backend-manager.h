#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H
#include <glog/logging.h>
#include <QNetworkAccessManager>
#include <QStandardItem>
#include <QNetworkReply>

class BackendManager : public QObject
{
  Q_OBJECT

public:
  explicit BackendManager(QObject *parent = nullptr);
  ~BackendManager() = default;
  void getHeaders();
  void getAllRecordings();
  QString getActiveUser();
  void setBaseURL(QString url);

  void login(QString username, QString password);
  void deleteAllRecordings();
  void deleteSelectedRecordings(const QList<int> &studentIds);
  void addRecording(const std::map<QString, std::variant<QString, int>> &dataMap);
  void filteredSelect(const std::map<QString, std::variant<QString, int>> &requestArgs);
  void logout();

private:
  QString baseUrl;
  QByteArray activeCredentials;
  QString activeUser;
  QNetworkAccessManager *networkManager;
signals:
  void loginSuccessful();
  void loginFailed(QNetworkReply::NetworkError errcode);
  void getHeadersSuccessful(QMap<QString, QStringList> fields);
  void getAllRecordingsSuccessful(QStringList currentKeyOrder, QList<QList<QStandardItem *>> rows);
  void deleteAllRecordingsSuccessful(int countDeleted);
  void deleteSelectedRecordingsSuccessful();
  void addRecordingSuccessful();
  void filteredSelectSuccessful(QList<int> studentIds);
  void requestFailed(QString requestName, QNetworkReply::NetworkError errcode);
};

void multiarg(QByteArray &ba);

template <class T, class... Args>
void multiarg(QByteArray &ba, const T &arg, const Args &...args)
{
  ba.append(QString(arg).toUtf8());
  multiarg(ba, args...);
};

#endif // BACKENDMANAGER_H
