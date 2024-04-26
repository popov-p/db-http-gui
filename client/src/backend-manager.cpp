#include "backend-manager.h"

BackendManager::BackendManager(QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager();
    authenticator = new QAuthenticator();
}

BackendManager::~BackendManager() {

}
