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
#include "add-element-dialog.h"

class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
private:
    AddElementDialog *addElementDialog;
    QVBoxLayout *widgetVLayout;
    QPushButton *disconnectButton;
    QPushButton *addButton;
    QPushButton *deleteButton;
    QPushButton *deleteAllButton;
    QLabel *responseLabel;

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
    QLineEdit *startsWithLetterLineEdit;

    QComboBox *compareComboBox;
    QLineEdit *compareLineEdit;
    QCheckBox *geqCheckBox;
    QCheckBox *leqCheckBox;

public slots:
    void slotOkButtonDone();
private slots:
    void slotDisconnectButtonClicked();
    void slotAddButtonClicked();
signals:
    void disconnectButtonClicked();
    void addButtonClicked();
};


#endif // MAINWIDGET_H
