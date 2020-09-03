#ifndef CLUSTERPROOF_H
#define CLUSTERPROOF_H
#include <QWidget>
#include <QFileInfo>
#include <QSplitter>
#include <QLineEdit>
#include <QString>
#include <QProgressDialog>
#include <QDir>
#include <QtAlgorithms>

#include "myview.h"
#include "myscene.h"
#include "myitem.h"
#include "mypixmapitem.h"
#include "mytextitem.h"
#include "itemrecognizer.h"
#include "clustertextitem.h"
#include "mydatabase.h"

namespace Ui {
class ClusterProof;
}

class ClusterProof : public QWidget
{
    Q_OBJECT

public:
    explicit ClusterProof(QString projectid ,QWidget *parent = 0,QString username = "");
    ~ClusterProof();

    QString userName;
    static QString removeFileNamesSuffix(QString name);
    void keyPressEvent(QKeyEvent *event);
    MyDatabase* database;
    bool dialogOnOrNot;
    int requestNumber;
    void setRequestNumber(int value);
    //这张图片里面没有字
    bool onlyPicture;

private:
    QString projectId;

    int startDialogCount;
    QString currentRightWord;
    Ui::ClusterProof *ui;
    //下载的总数
    int totalNumber;
    //进度条的数据
    MyTextScene* leftScene;
    MyScene* uperScene;
    MyScene* underScene;
    MyView* leftView;
    MyView* uperView;
    MyView* underView;
    QAction* mAction;

    NetWorkClub* netRequest;
    NetWorkClub* netDownLoad;
    NetWorkClub *picUpload;
    
    PicEntityAndInfo* pic;
    QVector<PicEntityAndInfo*> pics;

    QString downloadPicName;

    PicEntityAndInfo* currentPic;
    //转换名字的函数
    void receivePicsToFillTheScene();

    QSplitter* rightSplitter;
    QSplitter* cornerSplitter;
    QSplitter* leftSplitter;

    QLineEdit* lineEdit;

    void resizeEvent(QResizeEvent *event);
    QVector<MyPixmapItem*> pixmapItems;

    void handlePicture();
    //下图片用的progress进度条
    QProgressDialog* dialog;
    QProgressDialog* finishedDialog;
    //图片和字们的数据集
    QVector<CharAndCharRegionList*>* CharAndCharRegionListVector;
    //把他们show出来
    void showThePicAndText();
    void fixNodeAndVector();
    void fixNode(int index, hocr::bbox bbox);
    void fixVector(QString word, CharPicInfo *picInfo);
    bool isBboxEqual(hocr::bbox box1, hocr::bbox box2);
    void picDbRequest();
    int uploadFinishedDialog;
    QGraphicsTextItem* leftLaseItem;
    void constructPic();
    void showLeftCharacter();
public slots:
    void on_requestButton_clicked();
    void receiveClustRequestToGo(QString info);
    void receivePicInfoToStore(QString info);
    void textChangedResponse(QString text);
    void changeProgressBarSlots();
    void on_upDateButtom_clicked();

    void setRequestCountNumber(QString str);
private slots:
    void setLineEditFocused();
    void changeFinishedProgressBarSlots();
    void receiveDatabaseToGo(QVector<PicEntityAndInfo*> pics);
signals:
    void downloadOksignal();
    void databaseRequestInfo(QVector<PicEntityAndInfo*> pics);
};

#endif // CLUSTERPROOF_H












