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
#include <QTimer>
#include "add-element-dialog.h"
#include "backend-manager.h"

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(BackendManager* backendManager, QWidget *parent = nullptr);
    ~MainWidget() = default;
public slots:
    void slotLoginSuccessful();
private:
    AddElementDialog *addElementDialog;
    QVBoxLayout *widgetVLayout;
    QPushButton *disconnectButton;
    QPushButton *addButton;
    QPushButton *deleteSelectedButton;
    QPushButton *deleteAllButton;
    QLineEdit *responseLabel;
    QTimer *responseLabelTimer;
    QLabel *usernameLabel;
    QHBoxLayout *hHeaderLayout;
    QHBoxLayout *hResponseLabelLayout;
    QHBoxLayout *hTableViewLayout;
    QHBoxLayout *hAlphabeticOptionsLayout;
    QHBoxLayout *hComparableOptionsLayout;
    QHBoxLayout *hFilterButtonsLayout;
    QHBoxLayout *hFilterLabelLayout;

    /*database*/
    QStandardItemModel *model;
    QTableView *tableView;
    /*filtering options*/
    QLabel *filterLabel;
    QLabel *startsWithLetterLabel;
    QComboBox *startsWithLetterComboBox;
    QComboBox *alphabetComboBox;

    QComboBox *compareComboBox;
    QComboBox *compareElementsComboBox;
    QCheckBox *geqCheckBox;
    QCheckBox *leqCheckBox;
    QPushButton *filterButton;
    QPushButton *dropFilterButton;
    QMap<QString, QStringList> alphCompMap;
    QStringList desiredHeaderOrder;
    int idLogicalIndex = -1;
    int photoLogicalIndex = -1;
    QMap<QString, QPair<int, QList<int>>> compHeaderData;
    BackendManager *backendManager = nullptr;

    void initHeaderLayout();
    void initResponseLabelLayout();
    void initTableViewLayout();
    void initFilterOptionsLayout();
    void initWidgetVLayout();
    void initConnections();

private slots:
    void slotAddButtonClicked();
    void slotDisconnectButtonClicked();
    void slotFilterButtonClicked();
    void slotUpdateCompareElementsComboBox(const QString& changedField);
    void slotClearComparableFields();
    void slotGetHeadersSuccessful(QMap<QString, QStringList> fieldsMap);
    void slotGetAllRecordingsSuccessful(QStringList currentKeyOrder, QList<QList<QStandardItem*>> rows);
    void slotDeleteAllRecordingsSuccessful(int countDeleted);
    void slotDeleteSelectedRecordingsSuccessful();
    void slotAddRecordingSuccessful();
    void slotFilterSelectSuccessful(QList<int> studentIds);
signals:
    void disconnectButtonClicked();
    void addButtonClicked();
};

void fillColumnData(QMap<QString, QPair<int, QList<int>>>& compHeaderIds, QStandardItemModel* model);
#endif // MAINWIDGET_H
