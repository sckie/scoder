#include "qscitemline.h"
#include <QGraphicsScene>
#include <QPainter>
#include <math.h>

class QScItemLinePrivate
{
    friend class QScItemLine;

    QScItemLine::LineType lineType;
    QList<QPointF> points;
    qreal arrowLength;
    qreal startLength;
};

QScItemLine::QScItemLine(QScItemBase *start,
                         QScItemBase *end,
                         QGraphicsScene *scene,
                         QGraphicsObject *parent)
    : QScItemBase(ItemType_Line, parent)
    , p(new QScItemLinePrivate)
{
    p->lineType = LineType_Direct;
    p->lineType |= LineType_Arrow;
    p->arrowLength = 8;
    p->startLength = 20;

    setFlags(QGraphicsItem::ItemSendsGeometryChanges);

    if (scene)
    {
        scene->addItem(this);
    }

    addPrev(start);
    addNext(end);

    setZValue(-1);

    QRectF rect(start->controlPosOut(end), end->controlPosIn(start));
    updatePaths(rect.center(), rect.size());
}

QScItemLine::~QScItemLine()
{
    delete p;
}

QPainterPath QScItemLine::createPaths(const QSizeF &size) const
{
    Q_UNUSED(size)

    QPainterPath path;

    QPointF start = this->start()->controlPosOut(this->end());
    QPointF end = this->end()->controlPosIn(this->start());

    if (p->points.count() > 2)
    {
        p->points.removeFirst();
        p->points.removeLast();
    }

    p->points.prepend(start);
    p->points.append(end);

    if (p->lineType & LineType_Cublic)
    {
        path.moveTo(start);
        path.cubicTo(start, QPointF(start.x(), end.y()), end);
    }
    else if (p->lineType & LineType_Direct)
    {
        for (int index = 0; index < p->points.count(); index++)
        {
            if (index == 0)
            {
                path.moveTo(p->points.first());
            }
            else
            {
                path.lineTo(p->points.at(index));
            }
        }
    }
    else if (p->lineType & LineType_Broken)
    {
        p->points.clear();
        p->points << start;
        if (end.y() <= start.y())
        {
            if (end.x() >= start.y())
            {
                p->points << QPointF(start.x() + p->startLength, start.y());
                p->points << QPointF(start.x() + p->startLength, end.y());
            }
            else
            {
                p->points << QPointF(start.x() - p->startLength, start.y());
                p->points << QPointF(start.x() - p->startLength, end.y());
            }
        }
        else
        {
            p->points << QPointF(start.x(), start.y() + p->startLength);
            p->points << QPointF(end.x(), start.y() + p->startLength);
        }
        p->points << end;

        for (int index = 0; index < p->points.count(); index++)
        {
            if (index == 0)
            {
                path.moveTo(p->points.first());
            }
            else
            {
                path.lineTo(p->points.at(index));
            }
        }
    }

    if (p->lineType & LineType_Arrow)
    {
        path.addPolygon(arrow());
    }

    return path.translated(-pos().x(), -pos().y());
}

QScItemLine::LineType QScItemLine::lineType() const
{
    return p->lineType;
}

void QScItemLine::setLineType(LineType lineType)
{
    if (lineType == p->lineType)
    {
        return;
    }

    p->lineType = lineType;

    QPointF start = p->points.first();
    QPointF end = p->points.last();
    p->points.clear();
    p->points << start << end;

    updatePaths(pos(), boundingRect().size());
}

QScItemBase *QScItemLine::start() const
{
    if (prevItems().isEmpty())
    {
        return NULL;
    }

    return prevItems().first();
}

QScItemBase *QScItemLine::end() const
{
    if (nextItems().isEmpty())
    {
        return NULL;
    }

    return nextItems().first();
}

QList<QPointF> QScItemLine::points() const
{
    return p->points;
}

bool QScItemLine::removeAt(int index)
{
    if (index < 0 ||
        index >= p->points.count() - 1)
    {
        return false;
    }

    p->points.removeAt(index);
    return true;
}

bool QScItemLine::updateAt(int index, const QPointF &point)
{
    if (index < 0 ||
        index >= p->points.count())
    {
        return false;
    }

    p->points[index] = point;
    update();

    return true;
}

void QScItemLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(pen());
    painter->drawPath(shape());

    painter->setBrush(brush());
    painter->drawPolygon(arrow().translated(-pos().x(), -pos().y()));
}

void QScItemLine::updateItemsPos()
{
    p->points.clear();

    QRectF rect(start()->controlPosOut(end()), end()->controlPosIn(start()));
    updatePaths(rect.center(), rect.size());
}

QPolygonF QScItemLine::arrow() const
{
    QPolygonF arrow;
    if (p->points.count() < 2)
    {
        return arrow;
    }

    if (!(p->lineType & LineType_Arrow))
    {
        return arrow;
    }

    QPointF start = p->points.at(p->points.count() - 2),
            end = p->points.last();

    arrow << end;
    if (p->lineType & LineType_Cublic)
    {
        if (start.x() < end.x())
        {
            arrow << QPointF(end.x() - p->arrowLength, end.y() - p->arrowLength / 2);
            arrow << QPointF(end.x() - p->arrowLength, end.y() + p->arrowLength / 2);
        }
        else
        {
            arrow << QPointF(end.x() + p->arrowLength, end.y() - p->arrowLength / 2);
            arrow << QPointF(end.x() + p->arrowLength, end.y() + p->arrowLength / 2);
        }
    }
    else if ((p->lineType & LineType_Direct) ||
             (p->lineType & LineType_Broken))
    {
        qreal x1 = start.x();
        qreal y1 = start.y();
        qreal x2 = end.x();
        qreal y2 = end.y();

        qreal qRadius = p->arrowLength;
        qreal qA = 0.6;

        qreal x3 = x2 - qRadius * cos(atan2((y2 - y1), (x2 - x1)) - qA);
        qreal y3 = y2 - qRadius * sin(atan2((y2 - y1), (x2 - x1)) - qA);
        qreal x4 = x2 - qRadius * sin(atan2((x2 - x1), (y2 - y1)) - qA);
        qreal y4 = y2 - qRadius * cos(atan2((x2 - x1), (y2 - y1)) - qA);

        arrow << QPointF(x3, y3);
        arrow << QPointF(x4, y4);
    }

    arrow << end;
    return arrow;
}
