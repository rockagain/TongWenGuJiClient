#include "networkclub.h"
#include "iostream"
using namespace std;

NetWorkClub::NetWorkClub(QString username)
{
    manager = new QNetworkAccessManager;
    connect(manager, &QNetworkAccessManager::finished, this, &NetWorkClub::replyFinished);
    this->userName = username;
}

/**
 * @brief 这个方法实现了把json转化为字符串的功能
 * @param str
 * @return
 */
QJsonObject NetWorkClub::getJsonObjectFromString(const QString str)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(str.toUtf8().data());


    QJsonObject jsonObject = jsonDocument.object();

    return jsonObject;
}


/**
 * @brief 把没用的信息给替换掉，soap和qt交互需要按照soap的格式。
 * @param re
 */
void NetWorkClub::replyFinished(QNetworkReply * re){

    //应该把if里面的所有类型，
    //读取所有返回的内容
    QString all = re->readAll();
    qDebug()<< all;
    if(all.contains("<faultstring>")){
        all = all.split("<faultstring>")[1];
        all = all.split("<</faultstring>>")[0];
        //qDebug()<<all;
        QMessageBox messagebox;
        messagebox.information(NULL, QString("提示"),all, QMessageBox::Yes);
        return;
    }

    all = all.split("<return>")[1];
    //qDebug()<< all;
    all = all.split("</return>")[0];
    //处理数据
    if (NetRequestType == PicRequest) {

        all.replace("<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Body><ns2:hbApplyResponse xmlns:ns2=\"http://webService.hbApply.server.module.platform.towen.com/\"><return>","");
        all.replace("</return></ns2:hbApplyResponse></soap:Body></soap:Envelope>","");
        returnedAll = all;
        QJsonObject jsonob = getJsonObjectFromString(all);
        QJsonValue jsonvalue =  jsonob["errorCode"];
        QString result = jsonvalue.toString();
        //qDebug()<< result;
        if(result == "success") {
            sentItemsName(returnedAll);
        } else {
            jsonvalue = jsonob["errorMsg"];
            result = jsonvalue.toString();
            QMessageBox messagebox;
            messagebox.information(NULL, QString("提示"),QString("文件数量不足"), QMessageBox::Yes);
        }
    }
    if(NetRequestType == PicDownload) {

        all.replace("<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Body><ns2:fileDownloadResponse xmlns:ns2=\"http://webService.fileDownload.server.module.platform.towen.com/\"><return>","");
        all.replace("</return></ns2:fileDownloadResponse></soap:Body></soap:Envelope>","");
        returnedAll = all;
        sentPicInfo(returnedAll);
    }

    if(NetRequestType == PicFinished) {
        QJsonObject jsonob = getJsonObjectFromString(all);
        QJsonValue jsonvalue =  jsonob["resultMsg"];
        QString result = jsonvalue.toString();
        //qDebug()<< result;
        if(result == "true") {
            uploadFinishedSignal();
            sentClusterFinishedProgressToGo();
        }
    }

    if(NetRequestType == ProofRequest) {
        all.replace("<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Body><ns2:bmjdApplyResponse xmlns:ns2=\"http://webService.bmjdApply.server.module.platform.towen.com/\"><return>","");
        all.replace("</return></ns2:bmjdApplyResponse></soap:Body></soap:Envelope>","");
        returnedAll = all;
        QJsonObject jsonob = getJsonObjectFromString(all);
        QJsonValue jsonvalue =  jsonob["errorCode"];
        QString result = jsonvalue.toString();
        //qDebug()<< result;
        if(result == "success") {
            setProofRequestInfo(returnedAll);
        } else {
            qDebug()<<returnedAll;
            jsonvalue = jsonob["errorMsg"];
            result = jsonvalue.toString();
            QMessageBox messagebox;
            messagebox.information(NULL, QString("提示"),QString("文件数量不足"), QMessageBox::Yes);
        }
    }

    if(NetRequestType == WholePicRequest) {
        all.replace("<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Body><ns2:zyjdApplyResponse xmlns:ns2=\"http://webService.zyjdApply.server.module.platform.towen.com/\"><return>","");
        all.replace("</return></ns2:zyjdApplyResponse></soap:Body></soap:Envelope>","");
        returnedAll = all;
        QJsonObject jsonob = getJsonObjectFromString(all);
        QJsonValue jsonvalue =  jsonob["errorCode"];
        QString result = jsonvalue.toString();
        //qDebug()<< result;
        if(result == "success") {
            setWholePicRequestInfo(returnedAll);
        } else {
            //qDebug()<<returnedAll;
            jsonvalue = jsonob["errorMsg"];
            result = jsonvalue.toString();
            QMessageBox messagebox;
            messagebox.information(NULL,QString("提示"),QString("文件数量不足"), QMessageBox::Yes);
        }
    }

    if(NetRequestType == ClusterRequest) {
        all.replace("<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\"><soap:Body><ns2:jljdApplyResponse xmlns:ns2=\"http://webService.jljdApply.server.module.platform.towen.com/\"><return>","");
        all.replace("</return></ns2:jljdApplyResponse></soap:Body></soap:Envelope>","");
        returnedAll = all;
        QJsonObject jsonob = getJsonObjectFromString(all);
        QJsonValue jsonvalue =  jsonob["errorCode"];
        QString result = jsonvalue.toString();
        qDebug()<< all;
        //qDebug()<< result;
        //￥￥￥￥
        if(result == "success") {
            setClusterRequestInfo(returnedAll);
            //sentClusterProgressToGo();
        } else {
            //qDebug()<<returnedAll;
            jsonvalue = jsonob["errorMsg"];
            result = jsonvalue.toString();
            QMessageBox messagebox;
            messagebox.information(NULL, QString("提示"),QString("文件数量不足"), QMessageBox::Yes);
        }
    }
    re->deleteLater();
}

void NetWorkClub::editboardRequest(QString projectId, QString userName = "" , QString fileTotal = "")
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;

    TargetNamespace = "http://webService.hbApply.server.module.platform.towen.com/";
    wrapper = "hbApply";
    QString url = Config::ServerUrl + "webService/pictureApplyController?wsdl";

    QString nodeCode="2";

    //封装成方法
    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',projectId:\'");
    jString.append(projectId);
    jString.append("\',fileTotal:\'");
    jString.append(fileTotal);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));

    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());
}

//版面校对申请
void NetWorkClub::proofBoardRequest(QString projectId, QString userName, QString fileTotal)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;

    TargetNamespace = "http://webService.bmjdApply.server.module.platform.towen.com/";
    wrapper = "bmjdApply";
    QString url = Config::ServerUrl + "webService/bmjdApplyController?wsdl";

    QString nodeCode="4";

    //封装成方法
    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',projectId:\'");
    jString.append(projectId);
    jString.append("\',fileTotal:\'");
    jString.append(fileTotal);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));

    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());

}

//整页校对申请
void NetWorkClub::wholePicRequest(QString projectId, QString userName, QString fileTotal)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;

    TargetNamespace = "http://webService.zyjdApply.server.module.platform.towen.com/";
    wrapper = "zyjdApply";
    QString url = Config::ServerUrl + "webService/pageApplyController?wsdl";

    QString nodeCode="7";

    //封装成方法
    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',projectId:\'");
    jString.append(projectId);
    jString.append("\',fileTotal:\'");
    jString.append(fileTotal);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));

    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());

}


void NetWorkClub::downLoad(QString userName,QString filePath)
{

    QString jString;
    QString TargetNamespace;
    QString wrapper;
    QString url;

    TargetNamespace =  "http://webService.fileDownload.server.module.platform.towen.com/";
    wrapper =  "fileDownload";
    url = Config::ServerUrl +  "webService/fileDownloadController?wsdl";
    // url = "http://172.16.0.153:8080/webService/fileDownloadController?wsdl";


    jString.append("{userName:\'");
    jString.append(userName);

    jString.append("\',filePath:\'");
    jString.append(filePath);
    jString.append("\'}");

    QString data= "";

    data.append(constructPostData(TargetNamespace,wrapper,jString));
    //qDebug()<<"DDDDDDDDDDDDDDDDDDDDDUUUUUUUUUUUUGGGGGGGGGGGGGGGG                 " <<data;
    //请求部分
    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());

}

void NetWorkClub::editboardupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;
    QString url;

    TargetNamespace = "http://webService.serverComplete.server.module.platform.towen.com/";
    wrapper = "hbComplete";
    url = Config::ServerUrl + "webService/hbCompleteController?wsdl";

    nodeCode="2";


    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',fileName:\'");
    jString.append(filename);
    jString.append("\',fileCode:\'");
    jString.append(fileCode);
    jString.append("\',fileContent:\'");
    jString.append(fileContent);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));

    //请求部分
    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());
}


//proofboard 版面校对 最后提交
void NetWorkClub::proofboardupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;
    QString url;

    TargetNamespace = "http://webService.serverComplete.server.module.platform.towen.com/";
    wrapper = "bmjdComplete";
    url = Config::ServerUrl + "webService/bmjdCompleteController?wsdl";

    nodeCode="4";


    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',fileName:\'");
    jString.append(filename);
    jString.append("\',fileCode:\'");
    jString.append(fileCode);
    jString.append("\',fileContent:\'");
    jString.append(fileContent);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));
    //qDebug()<<"********************************************";
    qDebug()<< data;
    //qDebug()<<"********************************************";
    //请求部分
    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());
}

//整页校对上传
void NetWorkClub::fullpicupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;
    QString url;

    TargetNamespace = "http://webService.serverComplete.server.module.platform.towen.com/";
    wrapper = "zyjdComplete";
    url = Config::ServerUrl + "webService/zyjdCompleteController?wsdl";

    fileContent =  fileContent.replace("\n","\\n").toHtmlEscaped();   //转化一下杜绝编码问题




    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',fileName:\'");
    jString.append(filename);
    jString.append("\',fileCode:\'");
    jString.append(fileCode);
    jString.append("\',fileContent:\'");
    jString.append(fileContent);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));
    qDebug()<<data;

    //请求部分
    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());
}


//聚类校对

void NetWorkClub::clustproofRequest(QString projectId, QString userName, QString fileTotal)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;

    TargetNamespace = "http://webService.jljdApply.server.module.platform.towen.com/";
    wrapper = "jljdApply";
    QString url = Config::ServerUrl + "webService/jljdApplyController?wsdl";

    QString nodeCode="6";

    //封装成方法
    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',projectId:\'");
    jString.append(projectId);
    jString.append("\',fileTotal:\'");
    jString.append(fileTotal);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));

    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());

}

/**
 * @brief 构建soup的post申请
 * @param TargetNamespace
 * @param wrapper
 * @param jString
 * @return
 */
QString NetWorkClub::constructPostData(QString TargetNamespace,QString wrapper ,QString jString)
{
    QString data = "";

    data.append("<soapenv:Envelope xmlns:soapenv=\"http://schemas.xmlsoap.org/soap/envelope/\" xmlns:web=\"" + TargetNamespace);
    data.append( "\">");
    data.append("<soapenv:Header/>");
    data.append(" <soapenv:Body>");
    data.append("<web:" + wrapper);
    data.append(">");
    data.append("<arg0>" + jString + "</arg0>");
    data.append("</web:"+ wrapper);
    data.append(">");
    data.append(" </soapenv:Body>");
    data.append("</soapenv:Envelope>");

    return data;
}


//聚类校对 最后提交
void NetWorkClub::clusterProofupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent)
{
    QString jString;
    QString TargetNamespace;
    QString wrapper;
    QString url;

    TargetNamespace = "http://webService.serverComplete.server.module.platform.towen.com/";
    wrapper = "jljdComplete";
    url = Config::ServerUrl + "webService/jljdCompleteController?wsdl";


    nodeCode="6";


    jString.append("{userName:\'");
    jString.append(userName);
    jString.append("\',nodeCode:\'");
    jString.append(nodeCode);
    jString.append("\',fileName:\'");
    jString.append(filename);
    jString.append("\',fileCode:\'");
    jString.append(fileCode);
    jString.append("\',fileContent:\'");
    jString.append(fileContent);
    jString.append("\'}");

    QString data= "";
    data.append(constructPostData(TargetNamespace,wrapper,jString));
    //qDebug()<<data;
    //请求部分
    QNetworkRequest request;
    QUrl loginUrl(url);
    request.setUrl(loginUrl);
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    manager->post(request,data.toUtf8());
}




