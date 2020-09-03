#ifndef MYPIXMAPITEM_H
#define MYPIXMAPITEM_H
#include <QObject>
#include <QGraphicsPixmapItem>

#include "config.h"

class MyPixmapItem : public QGraphicsPixmapItem
{
public:
    MyPixmapItem();
    //每一个图片的信息
    CharPicInfo *picInfo;

    void setPicInfo(CharPicInfo *value);
    CharPicInfo *getPicInfo() const;
    //现在的字，和要改的字
    QString currentWord;
    QString rightWord;
    void setCurrentWord(const QString &value);
};

#endif // MYPIXMAPITEM_H
