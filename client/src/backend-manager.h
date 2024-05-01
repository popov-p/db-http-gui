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
    void setBaseURL(QString url);
    void login(QString username, QString password);
    void getDbFields();
    void getAllDbRecordings();
    /*methods to access database fields here*/
    void logout();
private:
    QString baseUrl;
    QNetworkAccessManager *networkManager;
    QMap<QString, std::function<void(QNetworkReply*)>> replyHandlers;
    void handleLogin(QNetworkReply *reply);
    void handleDBFields(QNetworkReply *reply);
    void handleAllDbRecordings(QNetworkReply *reply);
    void initConnections();
private slots:
    void slotRequestFinished(QNetworkReply *reply);
signals:
    void loginSuccessful();
    void loginFailed(QNetworkReply::NetworkError errcode);
    void getFieldsSuccessful(QMap<QString, QStringList> fields);
    void getFieldsFailed(QNetworkReply::NetworkError errcode);
    void getAllDbRecordingsSuccessful(QList<QList<QStandardItem*>> rows);
    void getAllDbRecordingsFailed(QNetworkReply::NetworkError errcode);

};

void multiarg(QByteArray& ba);

template<class T, class... Args>
void multiarg(QByteArray& ba, const T& arg, const Args&... args) {
    ba.append(QString(arg).toUtf8());
    multiarg(ba, args...);
};

#endif // BACKENDMANAGER_H
