#ifndef EDITBOARD_H
#define EDITBOARD_H

#include <QWidget>
#include <QComboBox>
#include <QPointF>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QSplitter>
#include <QListWidget>
#include <QFileInfo>
#include <QVector>

#include "editboard.h"
#include "mylistwidget.h"
#include "myscene.h"
#include "myview.h"
#include "config.h"
#include "libhocr/hocr_types.h"
#include "mydatabase.h"
#include "clusterproof.h"
#include "itemrecognizer.h"
#include "proofboard.h"


namespace Ui {
class EditBoard;
}

class EditBoard : public QWidget
{
    Q_OBJECT

public:
    explicit EditBoard(QString projectid, QWidget *parent = 0,QString username = "");
    ~EditBoard();

    void setToolbar(QToolBar *value);
    MyDatabase*  database;
    MyListWidget* myListWidget;

    QListWidgetItem * currentSeletedItem;

    MyScene* mScene;
    MyView* mView;
    //下载来的图片
    QImage* image;
    ItemRecognizer* recognizer;

    void setBigCharFlag();
    void setSingleRowFlag();
    void setMainBodyFlag();
    void setTableFlag();
    void setImageFlag();
    void setDoubleRowFlag();
    void setSmallCharFlag();

    bool ratioSwitch;
    bool resizeRatioSwitch;
    qreal ratio;
    qreal reSizeRatio;
    //ratioIsAlready Changed By Resize parameter
    bool ratiolIsChangedByResize;
    int totalNumber;
    bool dialogOnOrNot;
    int startDialogCount;
    //设置到底申请多少图片
    int requestNumber;
    void setRequestNumber(int value);

private:
    Ui::EditBoard *ui;
    QAction* mAction;
    QPointF actionPoint;
    QToolBar* toolbar ;
    QPushButton* button ;
    QString userName;
    QSplitter* splitter;

    NetWorkClub* netRequest;
    NetWorkClub* netDownLoad;
    NetWorkClub *picUpload;

    PicEntityAndInfo* pic;
    QVector<PicEntityAndInfo*> pics;
    QString downloadPicName;
    PicEntityAndInfo* currentPic;

    QString projectId;

    void picDbRequest();
    //查询数据库，然后还远图片的
    void constructPic();
    //查数据库那里，把信息添加到左栏item里面
    void constructPicItem(QString filename,bool isFinishedOrNot);
    //接收了hocr 保存本地数据库，然后提交到服务器
    void receiveHocrToGo();
    //查看是否每一张图片都被处理完毕了，这里实在申请那里用的
    bool testToSeeIfEeveryPicIsFinished();

    void downLoadtheFirstPic();

    qreal fitWidth;
    void initRatio();
    void receiveEverythingFromPicsToGo();
    QProgressDialog* dialog;
public slots:
    void receiveItemName(QString all);
    void receivePicInfo(QString all);
    void netFinishedReponse();
public slots:
    void on_zoomOutButton_clicked();
    void on_zoomInButton_clicked();

    void on_editButton_clicked();
    void on_setColorButton_clicked();

    //网络请求图片的时候用的
    void on_requestButton_clicked();

    void on_comboBox_currentIndexChanged(int index);
    void on_getHocrButtom_clicked();
    void downLoadPic(QListWidgetItem *item);
    //调整那个进度条的进度的
    void changeProgressBarSlots();

    void setRequestCountNumber(QString str);
protected:
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *event);
signals:
    void downloadOksignal();
};

#endif // EDITBOARD_H
