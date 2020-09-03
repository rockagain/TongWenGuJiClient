#ifndef CONFIG_H
#define CONFIG_H
#include <QString>
#include <string>
#include <QImage>

#include "libhocr/hocr_types.h"
#include "libhocr/hocr_node.h"
using namespace hocr;

//设置mode以便调控

enum ManageMode{
    NORMAL = 0,
    ZOOMIN,
    ZOOMOUT,
    EDIT,
    SELECT
};

enum ClientRequestMode{
    LOGIN = 0,
    PicRequest,
    PicDownload,
    PicFinished,
    ProofRequest,
    proofFinished,
    ClusterRequest,
    ClusterFinished,
    WholePicRequest,
    WholePicFinished,
};

struct PicEntityAndInfo{
    QString id;         //已废弃不用
    QString fileCode;
    QString fullName;
    QString fileName;
    QString filePath;
    QString fileOcr;
    QString base64;      //记录下这个接口
    bool isFinishedOrNot;
    HocrNode *picPageNode;   //一个图片的HOCRNODE
};

enum MOVEORNOT{
    MOVEABLE = 0,
    UNMOVEABLE,
};

enum BOARDTYPE{
    EDITBOARD = 0,
    PROOFBOARD,
    CLUSTERPROOF,
    FULLPICPROOF,
};

struct CharPicInfo{
    QImage image;
    int index;
    hocr::bbox bbox;
};   //每切一个字块 就加入到 charAndRegion 里面


struct CharAndCharRegionList{
    QColor textColor;
    QString character;
    QVector<CharPicInfo*> regionList;
};  //切图之前实例化


namespace Config {
//管理端服务器地址
//const QString ServerUrl = "http://172.16.0.30:8081/";
const QString ServerUrl = "http://172.16.0.22:8081/";
const QString dbName = "root";
const QString dbPassword = "123456";
}

struct ItemMode
{
    //是属于什么类型，比方说大字，或者单排，主体？
    int nodeType;
    //可不可以change
    bool isChange;
    //可不可以drag
    bool isDrag;
    //是否被选中了
    bool isSelectOrNot;
    //可不可以被删除
    bool isDeleteAble;
    //属于哪一个板子的
    int boardType;
};

//鼠标悬浮在Item上时候四个点的方位
enum LocateDirection{
    inThecenter = 0,
    rightUnder,
    rightUpper,
    leftUnder,
    leftUpper
};

#endif // CONFIG_H


