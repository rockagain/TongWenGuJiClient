#include "mypixmapitem.h"

MyPixmapItem::MyPixmapItem()
{

}

CharPicInfo *MyPixmapItem::getPicInfo() const
{
    return picInfo;
}

void MyPixmapItem::setCurrentWord(const QString &value)
{
    currentWord = value;
}

void MyPixmapItem::setPicInfo(CharPicInfo *value)
{
    picInfo = value;
}

