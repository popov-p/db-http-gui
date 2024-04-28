#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>
#include <glog/logging.h>

class BackendManager : public QObject {
    Q_OBJECT
public:
    explicit BackendManager(QObject *parent = nullptr);
    ~BackendManager();

    void login(QString username, QString password);
    void retrieveDbFields();
    /*methods to access database fields here*/
    void logout();
private:
    QNetworkAccessManager *networkManager;
    QMap<QString, std::function<void(QNetworkReply*)>> replyHandlers;
    void handleLogin(QNetworkReply *reply);

    void initConnections();
private slots:
    void slotRequestFinished(QNetworkReply *reply);
signals:
    void authSuccessful();
    void authFailed(QNetworkReply::NetworkError errcode);
    void retrieveDBFieldsSuccessful();
    //void
};

void multiarg(QByteArray& ba);

template<class T, class... Args>
void multiarg(QByteArray& ba, const T& arg, const Args&... args) {
    ba.append(QString(arg).toUtf8());
    multiarg(ba, args...);
};

#endif // BACKENDMANAGER_H
