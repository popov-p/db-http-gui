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
    QNetworkReply::NetworkError	logout();
private:
    QNetworkAccessManager *networkManager;
    QAuthenticator *authenticator;
    /*db fields*/;
    QString activeUser;
    void initConnections();
private slots:
    void slotLoginFinished();
};

#endif // BackendEMANAGER_H
