#ifndef NETWORKCLUB_H
#define NETWORKCLUB_H
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
#include <QRegExp>
#include <QString>
#include <QException>
#include "config.h"


class NetWorkClub  : public QObject
{
    Q_OBJECT
public:
    NetWorkClub(QString username);
    //管理 request 和 reply
    QNetworkAccessManager *manager;
    //字符转化为json
    QJsonObject getJsonObjectFromString(const QString str);
    
    void clientRequest(QString jString,  QString userName,    QString url,
                       QString nodeCode, QString fileTotal,   QString fileList,
                       QString filename, QString fileContent, QString filePath,QString fileCode);
    
    
    void editboardRequest(QString projectId,QString username ,QString fileTotal);
    
    void proofBoardRequest(QString projectId,QString userName,QString fileTotal);
    
    void wholePicRequest(QString projectId,QString userName,QString fileTotal);
    
    void clustproofRequest(QString projectId,QString userName,QString fileTotal);
    
    void downLoad(QString userName,QString filePath);
    
    void editboardupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent);
    
    void proofboardupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent);
    
    void fullpicupload(QString userName,QString nodeCode,QString filename,QString fileCode,QString fileContent);
    
    QString constructPostData(QString TargetNamespace,QString wrapper ,QString jString);
  
    int NetRequestType;
    QString returnedAll;
    
    void clusterProofupload(QString userName, QString nodeCode, QString filename, QString fileCode, QString fileContent);
private slots:
    //reply请求之后的动作
    void replyFinished(QNetworkReply*);
    
signals:
    void sentItemsName(QString all);
    void sentPicInfo(QString all);
    void setProofRequestInfo(QString all);
    void setWholePicRequestInfo(QString all);
    void setClusterRequestInfo(QString info);
    void uploadFinishedSignal();
    void sentClusterProgressToGo();
    void sentClusterFinishedProgressToGo();
private:
    QString userName;
    
};

#endif // NETWORKCLUB_H
