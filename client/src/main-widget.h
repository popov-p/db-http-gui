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


class MainWidget : public QWidget {
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();
private:
    QVBoxLayout *widgetVLayout;
    QPushButton *disconnectButton;
    QLabel *responseLabel;

    QHBoxLayout *hDisconnectLayout;
    QHBoxLayout *hResponseLabelLayout;
public slots:
    void slotOkButtonDone();
private slots:
    void slotDisconnectButtonClicked();
signals:
    void disconnectButtonClicked();
};


#endif // MAINWIDGET_H
