#ifndef ANOTHERLOGIN_H
#define ANOTHERLOGIN_H

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
#include <QDir>
#include <QSettings>

#include "mainwindow.h"
#include "message.h"
#include "networkclub.h"
#include "config.h"
#include "procedureselect.h"

namespace Ui {
class AnotherLogin;
}

class AnotherLogin : public QDialog
{
    Q_OBJECT

public:
    explicit AnotherLogin(QWidget *parent = 0);
    ~AnotherLogin();
    QMap<QString,QString> projectId;
    QMap<QString,QString> nodeId;
    // 用来保存
    QString userName;
    QString userCode;
    QSettings* settings;
    // 检查，如何信息完整的话发送
public slots:
    void  checkAndLog();

private:
    //管理 request 和 reply
    QNetworkAccessManager *manager;
    //字符转换为json
    QJsonObject getJsonObjectFromString(const QString str);

private slots:
    //reply请求之后的动作
    void replyFinished(QNetworkReply*);
private:
    Ui::AnotherLogin *ui;
};

#endif // ANOTHERLOGIN_H
