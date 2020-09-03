#ifndef PROOFBOARD_H
#define PROOFBOARD_H

#include <QWidget>
#include <QSplitter>
#include <QToolBar>
#include <QStringList>
#include <QComboBox>
#include <QPlainTextEdit>


#include "myscene.h"
#include "myview.h"
#include "mylistwidget.h"
#include "mydatabase.h"
#include "config.h"
#include "libhocr/hocr_types.h"
#include "config.h"
#include "libhocr/tinyxml2/tinyxml2.h"
#include "itemrecognizer.h"
#include "clusterproof.h"


namespace Ui {
class ProofBoard;
}

class ProofBoard : public QWidget
{
    Q_OBJECT

public:
    explicit ProofBoard(QString projectid ,QWidget *parent = 0,QString username="");
    ~ProofBoard();
    MyScene *mScene;
    MyView *mView;
    QAction *mAction;

    QPointF actionPoint;
    QToolBar *toolbar ;
    QPushButton *button ;

    MyListWidget *myListWidget;

    QSplitter * splitter;
    NetWorkClub* netRequest;
    NetWorkClub* netDownLoad;
    NetWorkClub* picUpload;
    MyDatabase*  database;

    PicEntityAndInfo* pic;
    QVector<PicEntityAndInfo*> pics;
    QString downloadPicName;
    PicEntityAndInfo* currentPic;

    QString userName;

    ItemRecognizer* itemrecognizer;

    QImage* image;

    QWidget* debugWidget;
    QPlainTextEdit*  debugTextEdit;
    //和 Editboard 一样， 去建立数据库 和各种操作
    void picDbRequest();
    //查询数据库，然后还远图片的
    void constructPic();
    //查数据库那里，把信息添加到左栏item里面
    void constructPicItem(QString filename,bool isFinishedOrNot);
    //接收了hocr 保存本地数据库，然后提交到服务器
    void receiveHocrToGo(QString hocr);
    //查看是否每一张图片都被处理完毕了，这里实在申请那里用的
    bool testToSeeIfEeveryPicIsFinished();
    //把所有的hocr信息都转化为Item
    void changeHocrToItem();

    static QString convertTheHocrFormats(QString hocr);

    void setBigCharFlag();
    void setSingleRowFlag();
    void setMainBodyFlag();
    void setTableFlag();
    void setImageFlag();
    void setDoubleRowFlag();
    void setSmallCharFlag();

    int totalNumber;
    int requestNumber;
    int startDialogCount;   //进度条 调控那里
    void setRequestNumber(int value);
    bool  dialogOnOrNot ;

public slots:
    void receivePicInfo(QString all);

    void uploadFininsdResponse();
private slots:
    void downLoadPic(QListWidgetItem *item);
public slots:
    void on_zoomOutButton_clicked();
    void on_zoomInButton_clicked();

    void on_editButton_clicked();
    void on_setColorButton_clicked();

    void on_comboBox_currentIndexChanged(int index);
    void on_getHocrButtom_clicked();
    void on_requestButton_clicked();
    void receiveProofRequestInfo(QString info);

    void setRequestCountNumber(QString str);
    void changeProgressBarSlots();
    void generateAndSaveHocrToGo();
private:
    QListWidgetItem * currentSeletedItem;
    Ui::ProofBoard *ui;
    void downLoadtheFirstPic();
    QString projectId;
    //管ratio的，控制ratio的开关等等
    bool ratioSwitch;
    bool resizeRatioSwitch;
    qreal ratio;
    qreal reSizeRatio;
    bool ratioIsChangedByResize;
    //ratio的初始化函数
    void initRatio();
    void receiveEverythingFromPicsToGo();
    QProgressDialog* dialog;
signals:
    void downloadOksignal();
protected:
    void resizeEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *event);
};

#endif // PROOFBOARD_H
