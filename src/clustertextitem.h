
#ifndef CLUSTERTEXTITEM_H
#define CLUSTERTEXTITEM_H
#include <QObject>
#include "mytextitem.h"

class ClusterTextItem : public MyTextItem
{
public:
    ClusterTextItem();
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};

#endif // CLUSTERTEXTITEM_H
