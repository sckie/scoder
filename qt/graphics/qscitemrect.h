#ifndef QSCITEMRECT_H
#define QSCITEMRECT_H

#include <qscitembase.h>

class QScItemRect : public QScItemBase
{
    Q_OBJECT

public:
    QScItemRect(const QPointF &point,
                const QSizeF &size,
                QGraphicsScene *scene,
                QScItemBase *parent = 0);


protected:
    QPainterPath createPaths(const QSizeF &size) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

#endif // QSCITEMRECT_H
