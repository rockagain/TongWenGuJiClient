#include "ui_login.h"
#include "login.h"
#include <QDir>

/**
 * @brief 添加开始验证，  和发送请求完毕后的   信号和槽
 * @param parent
 */
login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    qDebug()<<QDir::currentPath();
    //  ui->username->setText("gaoyang");
    //  ui->username->setText("xuwentao");
    //  ui->username->setText("jiayanjie");
    //  ui->username->setText("lihuanpeng");
    ui->password->setText("123");
    ui->username->setTextMargins(3,0,0,0);
    ui->password->setTextMargins(3,0,0,0);
    //manager  和  clint  还有 server  三套件
    manager = new QNetworkAccessManager;
    connect(ui->loginButton, &QPushButton::clicked, this, &login::checkAndLog);
    connect(manager, &QNetworkAccessManager::finished, this, &login::replyFinished);
    this->setWindowTitle(QStringLiteral("登陆"));
}

login::~login()
{
    delete ui;
}

/**
 * @brief 验证和发送
 */
void login::checkAndLog()
{
    if (ui->username->text() == "") {
        QMessageBox::warning(this, Message::warning, Message::username_empty);
    } else if (ui->password->text() == "") {
        QMessageBox::warning(this, Message::warning, Message::code_empty);
    } else {
        //需要发送的数据，必须附加他们Web的框架内容
        QString dt = "{userName:\'" + (ui->username->text()) + "\',passWord:\'" + (ui->password->text())+ "\'}";
        QString data = "";

        //  platform.towen.com       互换          platform.towen.com
        data.append("<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:web=\"http://webService.login.system.module.platform.towen.com/\">");
        data.append("<soapenv:Header/>");
        data.append("<soapenv:Body>");
        data.append("<web:userLogin>");
        data.append("<arg0>" + dt + "</arg0>");
        data.append("</web:userLogin>");
        data.append("</soapenv:Body>");
        data.append("</soapenv:Envelope>");
        //请求部分
        QNetworkRequest request;
        QUrl url(Config::ServerUrl + "webService/loginController?wsdl");

        request.setUrl(url);
        request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
        manager->post(request,data.toUtf8());
        //qDebug() << data;
    }
}

/**
 * @brief 这个方法实现了把json转化为字符串的功能
 * @param str
 * @return
 */
QJsonObject login::getJsonObjectFromString(const QString str)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8().data());
    if(jsonDocument.isNull()) {
    }
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject;
}

/**
 * @brief 把没用的信息给替换掉，soup和qt交互需要按照soup的格式。
 * @param re
 */
void login::replyFinished(QNetworkReply * re){

    //读取所有返回的内容
    QString all = re->readAll();

    //处理数据
    all.replace("<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Body><ns2:userLoginResponse xmlns:ns2=\"http://webService.login.system.module.platform.towen.com/\"><return>","");
    all.replace("</return></ns2:userLoginResponse></soap:Body></soap:Envelope>","");
    all.replace("><soapenv:Body><web:userLogin><arg0>{userName:'gaoyang',password:'123'}</arg0></web:userLogin></soapenv:Body></soapenv:Envelope>","");
    //qDebug()<<all;
    QString resultCode = "";
    QString resultMsg = (getJsonObjectFromString(all)["resultMsg"]).toString();
    // 返回结果是真
    if(resultMsg == "true") {
        resultCode = (getJsonObjectFromString(all)["resultCode"]).toString();
        //qDebug() << resultCode;
        QString successMsg = (getJsonObjectFromString(all)["successMsg"]).toString();
    } else {
        QString errorCode = (getJsonObjectFromString(all)["errorCode"]).toString();
        QString errorMsg = (getJsonObjectFromString(all)["errorMsg"]).toString();
        if (errorCode == "EC:4001") {
            QMessageBox::warning(this, Message::error_4000, Message::error_4001);
        }
    }
    re->deleteLater();
    if (resultMsg == "true") {
//        MainWindow *window = new MainWindow(0,resultCode,ui->username->text());
//        this->hide();
//        window->show();
    }

}








