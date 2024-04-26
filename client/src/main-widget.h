#ifndef MAINWIDGET_H
#define MAINWIDGET_H


#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <glog/logging.h>
#include <QStandardItemModel>
#include <QTableView>
#include <QRegularExpression>
#include <QSortFilterProxyModel>
#include <QCheckBox>
#include <QHeaderView>
#include <QNetworkAccessManager>
#include <QAuthenticator>
#include "add-element-dialog.h"


class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QNetworkAccessManager *networkManager,
               QAuthenticator *authenticator, QWidget *parent = nullptr);
    ~MainWidget();
private:
    AddElementDialog *addElementDialog;
    QVBoxLayout *widgetVLayout;
    QPushButton *disconnectButton;
    QPushButton *addButton;
    QPushButton *deleteSelectedButton;
    QPushButton *deleteAllButton;
    QLabel *responseLabel;
    QLabel *usernameLabel;
    QHBoxLayout *hHeaderLayout;
    QHBoxLayout *hResponseLabelLayout;
    QHBoxLayout *hTableViewLayout;
    QHBoxLayout *hFilterOptionsLayout;

    /*database*/
    QStandardItemModel *model;
    QTableView *tableView;
    /*filtering options*/
    QComboBox *startsWithLetterComboBox;
    QLabel *startsWithLetterLabel;
    QComboBox *alphabetComboBox;

    QComboBox *compareComboBox;
    QComboBox *compareElementsComboBox;
    QCheckBox *geqCheckBox;
    QCheckBox *leqCheckBox;

    QNetworkAccessManager *networkManager = nullptr;
    QAuthenticator *authenticator = nullptr;
    QMap<QString, QPair<QUrl, QNetworkReply*>> *requestMap; /*TODO: upd this later*/

    void initHeaderLayout();
    void initResponseLabelLayout();
    void initTableViewLayout();
    void initFilterOptionsLayout();
    void initWidgetVLayout();
    void initConnections();

public slots:
    void handleSettingsDialogOkButtonDone();
private slots:
    void slotDisconnectButtonClicked();
    void slotAddButtonClicked();
    void slotDeleteSelecterRows();
    //void slotFetchAllStudents();
    //void handleAllHeaderOrder();
signals:
    void disconnectButtonClicked();
    void addButtonClicked();
};


#endif // MAINWIDGET_H
