#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QStandardItem>
#include <QNetworkReply>
#include <QAuthenticator>
#include <glog/logging.h>

class BackendManager : public QObject {
    Q_OBJECT
public:
    explicit BackendManager(QObject *parent = nullptr);
    ~BackendManager();
    void getHeaders();
    void getAllRecordings();
    QString getActiveUser();
    void setBaseURL(QString url);

    void login(QString username, QString password);
    void deleteAllRecordings();
    void deleteSelectedRecordings(const QList<int>& studentIds);
    void addRecording(const std::map<QString, std::variant<QString, int>>& dataMap);
    void filteredSelect(const std::map<QString, std::variant<QString, int>>& requestArgs);
    /*methods to access database fields here*/
    void logout();
private:
    QString baseUrl;
    QByteArray activeCredentials;
    QString activeUser;
    QNetworkAccessManager *networkManager;
    /*credentials*/
signals:
    void loginSuccessful();
    void loginFailed(QNetworkReply::NetworkError errcode);
    void getHeadersSuccessful(QMap<QString, QStringList> fields);
    void getHeadersFailed(QNetworkReply::NetworkError errcode);
    void getAllRecordingsSuccessful(QStringList currentKeyOrder, QList<QList<QStandardItem*>> rows);
    void getAllRecordingsFailed(QNetworkReply::NetworkError errcode);
    void deleteAllRecordingsSuccessful(int countDeleted);
    void deleteAllRecordingsFailed(QNetworkReply::NetworkError errcode);
    void deleteSelectedRecordingsSuccessful();
    void deleteSelectedRecordingsFailed(QNetworkReply::NetworkError errcode);
    void addRecordingSuccessful();
    void addRecordingFailed(QNetworkReply::NetworkError errcode);
    void filteredSelectSuccessful(QList<int> studentIds);
    void filteredSelectFailed(QNetworkReply::NetworkError errcode);
};

void multiarg(QByteArray& ba);

template<class T, class... Args>
void multiarg(QByteArray& ba, const T& arg, const Args&... args) {
    ba.append(QString(arg).toUtf8());
    multiarg(ba, args...);
};

#endif // BACKENDMANAGER_H
