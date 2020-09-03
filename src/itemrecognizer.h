#ifndef ITEMRECOGNIZER_H
#define ITEMRECOGNIZER_H

#include <QMap>

#include "myscene.h"
#include "myitem.h"
#include "mytextitem.h"
#include "libhocr/hocr_types.h"
#include "libhocr/hocr_node.h"
#include "libhocr/tinyxml2/tinyxml2.h"
#include "libhocr/hocr_parser.h"
#include "mytextscene.h"
#include "config.h"

class ItemRecognizer :public QObject
{
    Q_OBJECT
public:
    ItemRecognizer(MyScene* sceneAtWork);
    ItemRecognizer(MyTextScene* sceneAtWork);
    ~ItemRecognizer();
    void constructNodefromPicItem();
    void constructHocrfromTextItemsNode();
    void constructHocrfromTextItemsNode(HocrNode* pageNode);
    QString constructHocrfromNode();
    QString constructHocrfromNode(HocrNode* rootNode);
    MyScene* myscene;
    MyTextScene* textscene;
    HocrNode* rootNode;
    HocrNode* mainBodyNode;
    //    QVector<HocrNode*>lineNodes;
    //    QVector<HocrNode*>wordNodes;
    QImage* mImage;
    void setImage(QImage *image);
    MyItem* myitem;
    QVector<MyItem*> myItems;
    QVector<MyItem*> myCharItems;
    //parser
    HocrParser* parser;
    //改变所有item的数据
    void changeMyItems();
    //下面两个是生成node的函数
    void makeMainBodyNode();
    void makeLineNodeAndWordNode();
    void makeSortedLineNodeAndWordNode();
    //从hocr到item
    void makePicItemfromHocr(QString hocr);
    void makeTextItemfromHocr(QString hocr);
    //pageNode 主NODE
    HocrNode* pageNode;
    //提交前生成的hocr
    QString hocr;
    QString getHocr() const;

    static HocrNode* changeHocrToPageNode(QString hocr);

    static void fromNodeFillInMaps(QImage image, HocrNode* node, QVector<CharAndCharRegionList *> *charRegionList, int index);

    static bool isExist(QVector<CharAndCharRegionList *> *listVector, QString str);

    void setMyscene(MyScene *value);

    HocrNode *returnHocrNode(QString hocr);

    void sortFullPicNode();
signals:
    void recognizerSendHocr(QString str);

private:
    QList<int> xsForSortItemForNodeRange;
    QList<int> fullPicLineList;
    QList<int> fullPicWordList;

    QVector<MyItem *> sortingSingleRowItem();
    QVector<MyItem *> sortingCharItem();
};

#endif // ITEMRECOGNIZER_H















