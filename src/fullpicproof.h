#ifndef WHOLENIGHTPROOF_H
#define WHOLENIGHTPROOF_H

#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QStringList>
#include <QComboBox>
#include <QDebug>
#include <QtAlgorithms>

#include "myscene.h"
#include "myview.h"
#include "mylistwidget.h"
#include "mydatabase.h"
#include "config.h"
#include "libhocr/hocr_types.h"
#include "libhocr/tinyxml2/tinyxml2.h"
#include "mytextscene.h"
#include "proofboard.h"
#include "mytextitem.h"

namespace Ui {
class FullPicProof;
}

class FullPicProof : public QWidget
{
    Q_OBJECT

public:
    explicit FullPicProof(QString projectid ,QWidget *parent = 0,QString username = "");
    ~FullPicProof();
    MyDatabase* database;
    MyListWidget *myListWidget;

    MyScene *mScene;
    MyView *mView;
    int totalNumber;

    int requestNumber;
    void setRequestNumber(int value);
    QProgressDialog * dialog;
    bool onlyPicture;
    int lineNum;  //这个参数是为了避除总行数里面的非line的

private:
    bool dialogOnOrNot;

    int startDialogCount;

    Ui::FullPicProof *ui;
    QString userName;

    QString projectId;

    MyTextScene* textScene;
    MyView* textView;
    QAction *mAction;

    QPointF actionPoint;
    QToolBar *toolbar ;
    QPushButton *button ;

    QSplitter * splitter;
    NetWorkClub* netRequest;
    NetWorkClub* netDownLoad;
    NetWorkClub *picUpload;

    PicEntityAndInfo* pic;
    QVector<PicEntityAndInfo*> pics;
    QString downloadPicName;
    PicEntityAndInfo* currentPic;

    ItemRecognizer* itemrecognizer;
    //和 Editboard 一样， 去建立数据库 和各种操作
    void picDbRequest();
    //查询数据库，然后还远图片的
    void constructPic();
    //查数据库那里，把信息添加到左栏item里面
    void constructPicItem(QString filename,bool isFinishedOrNot);
    //接收了hocr 保存本地数据库，然后提交到服务器
    //    void receiveHocrToGo(QString hocr);
    //查看是否每一张图片都被处理完毕了，这里实在申请那里用的
    bool testToSeeIfEeveryPicIsFinished();
    //把所有的hocr信息都转化为Item
    void changeHocrToItemTheFirst();
    //把所有的东西都改完了，这里是上传
    void okToGo();

    void downLoadtheFirstPic();
    void initRatio();
    bool ratioSwitch;
    bool resizeRatioSwitch;
    qreal ratio;
    qreal reSizeRatio;
    bool ratioIsChangedByResize;
    HocrNode* PageNode;
    HocrNode* MainBodyNode;
    QList<int> sortedLineLeft;
    QList<int> sortedWordTop;
    void showSingleColumn(int Column);
    MyPixmapItem* ColumnPic;
    QImage* image;
    void showSingleColumnText(HocrNode *line);
    MyTextItem* textItem;
    QImage _image ;

    void SaveAndPass();
    void receiveEverythingFromPicsToGo();
    QListWidgetItem * currentSeletedItem;
    //这个函数是设置wordNode的beforePunc bool值的
    void setWordNodeBeforePunc();
    //处理,生成的外部PuncNode
    void managePuncNodeToSave();

    void managePuncNodeToReshowRight();
public slots:
    void receivePicInfo(QString all);

    void uploadFinishedRespose();
public slots:
    void on_zoomOutButton_clicked();
    void on_zoomInButton_clicked();

    void on_editButton_clicked();
    void on_setColorButton_clicked();

    void on_comboBox_currentIndexChanged(int index);
    void on_getHocrButtom_clicked();
    void downLoadPic(QListWidgetItem *item);
    void on_testButton_clicked();
    void on_requestButton_clicked();
    void setRequestCountNumber(QString str);
    void changeProgressBarSlots();
private slots:
    void receiveWholePicRequestInfo(QString info);
signals:
    void downloadOksignal();

protected:
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *event);
};

#endif // WHOLENIGHTPROOF_H
