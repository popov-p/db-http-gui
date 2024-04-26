#ifndef BACKENDMANAGER_H
#define BACKENDMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QAuthenticator>

class BackendManager : public QObject {
    Q_OBJECT
public:
    explicit BackendManager(QObject *parent = nullptr);
    ~BackendManager();

    QNetworkReply::NetworkError login();
    void retrieveDbFields();
    /*methods to access database fields here*/
    QNetworkReply::NetworkError	logout();

private:
    QNetworkAccessManager *networkManager;
    QAuthenticator *authenticator;

};

#endif // BackendEMANAGER_H
