#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>

#include "mainwindow.h"
#include "message.h"
#include "networkclub.h"
#include "config.h"

namespace Ui {
class login;
}

//这是一个用来登录的类
class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = 0);
    ~login();

    // 检查，如何信息完整的话发送
public slots:
    void  checkAndLog();

private:
    Ui::login *ui;
    //管理 request 和 reply
    QNetworkAccessManager *manager;
    //字符转换为json
    QJsonObject getJsonObjectFromString(const QString str);

private slots:
    //reply请求之后的动作
    void replyFinished(QNetworkReply*);
};

#endif // LOGIN_H



















