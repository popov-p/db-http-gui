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

    void /*bool*/login(QString username, QString password);
    void /*QList*/retrieveDbFields();
    /*methods to access database fields here*/
    void logout();
private:
    QNetworkAccessManager *networkManager;
    QMap<QString, std::function<void(QNetworkReply*)>> replyHandlers;

    void initConnections();
private slots:
    void slotRequestFinished(QNetworkReply *reply);
signals:
    void authSuccessful();
    void authFailed(QNetworkReply::NetworkError errcode);
};

#endif // BACKENDMANAGER_H
