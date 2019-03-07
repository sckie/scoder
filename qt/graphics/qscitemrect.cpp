#include "qscitemrect.h"
#include "qscitemline.h"
#include <QGraphicsScene>
#include <QPainter>

QScItemRect::QScItemRect(const QPointF &point
                         , const QSizeF &size
                         , QGraphicsScene *scene
                         , QScItemBase *parent)
    : QScItemBase(ItemType_UserType, parent)
{
    setFlags(QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges);

    updatePaths(point, size);

    if (scene)
    {
        scene->addItem(this);
    }

    setBrush(Qt::lightGray);
    setPen(QColor(Qt::blue));
}

QPainterPath QScItemRect::createPaths(const QSizeF &size) const
{
    QPainterPath path;
    path.addRect(-size.width() / 2,
                 -size.height() / 2,
                 size.width(),
                 size.height());

    return path;
}

void QScItemRect::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawPath(shape());
}
