#ifndef QSCITEMLINE_H
#define QSCITEMLINE_H

#include <qscitembase.h>

class QScItemLinePrivate;
class QScItemLine : public QScItemBase
{
    Q_OBJECT

    Q_PROPERTY(LineType lineType READ lineType WRITE setLineType)

public:
    enum LineTypeFlag
    {
        LineType_Cublic = 0x0001,
        LineType_Direct = 0x0002,
        LineType_Broken = 0x0004,
        LineType_Arrow	= 0x1000,
    };
    Q_DECLARE_FLAGS(LineType, LineTypeFlag)

public:
    QScItemLine(QScItemBase *start,
                QScItemBase *end,
                QGraphicsScene *scene,
                QGraphicsObject *parent = 0);

    virtual ~QScItemLine();

public:
    LineType lineType() const;
    void setLineType(LineType lineType);

public:
    QScItemBase *start() const;
    QScItemBase *end() const;
    QList<QPointF> points() const;

public:
    bool removeAt(int index);
    bool updateAt(int index, const QPointF &point);

public:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void updateItemsPos();

protected:
    virtual QPainterPath createPaths(const QSizeF &size) const;
    QPolygonF arrow() const;

private:
    QScItemLinePrivate *p;
};

#endif // QSCITEMLINE_H
