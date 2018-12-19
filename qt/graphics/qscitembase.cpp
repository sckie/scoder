#include <qscitembase.h>
#include <QTimer>
#include <QPainter>
#include <QEvent>
#include <QAction>
#include <QActionGroup>
#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QFocusEvent>
#include <QMenu>
#if 0
#include <QGraphicsSceneContextMenuEvent>
#include <QStyleOptionGraphicsItem>
#endif

class QScItemBasePrivate
{
    friend class QScItemBase;

    int type;
    QPen pen;
    QBrush brush;
    QFont font;
    QString text;
    QString name;
    QSizeF size;
    bool checkable;
    bool checked;
    QTimer *timer;
    qulonglong times;
    int elapsed;
    QPainterPath graphicsPath;
    Qt::Orientation orientation;
    bool maskable;
    QScItemBase *mask;
    QList<QScItemBase *> prevItems, nextItems;
};


QScItemBase::QScItemBase(int type, QGraphicsObject *parent)
    : QGraphicsObject(parent)
    , p(new QScItemBasePrivate)
{
    p->type = type;
    p->checkable = false;
    p->checked = false;
    p->timer = NULL;
    p->times = 0;
    p->elapsed = 0;
    p->orientation = Qt::Horizontal;
    p->maskable = false;
    p->mask = NULL;

    setAcceptDrops(true);
    setAcceptHoverEvents(true);
#ifdef Q_OS_ANDROID
    setAcceptTouchEvents(true);
#endif
}

QScItemBase::~QScItemBase()
{
    delete p;
}

int QScItemBase::type() const
{
    return p->type;
}

QRectF QScItemBase::boundingRect() const
{
    return p->graphicsPath.boundingRect();
}

QPainterPath QScItemBase::shape() const
{
    return p->graphicsPath;
}

void QScItemBase::updatePaths(const QPointF &point, const QSizeF &size)
{
    setPos(point);

    p->graphicsPath = createPaths(size);
    p->size = boundingRect().size();

    update();
}

QPen QScItemBase::pen() const
{
    return p->pen;
}

void QScItemBase::setPen(const QPen &pen)
{
    if (p->pen != pen)
    {
        p->pen = pen;
        update();
    }
}

QBrush QScItemBase::brush() const
{
    return p->brush;
}

void QScItemBase::setBrush(const QBrush &brush)
{
    if (p->brush != brush)
    {
        p->brush = brush;
        update();
    }
}

QFont QScItemBase::font() const
{
    return p->font;
}

void QScItemBase::setFont(const QFont &font)
{
    if (p->font != font)
    {
        p->font = font;
        update();
    }
}

QString QScItemBase::name() const
{
    return p->name;
}

void QScItemBase::setName(const QString &name)
{
    if (p->name != name)
    {
        p->name = name;
        update();
    }
}

QString QScItemBase::text() const
{
    return p->text;
}

void QScItemBase::setText(const QString &text)
{
    if (p->text != text)
    {
        p->text = text;
        update();
    }
}

Qt::Orientation QScItemBase::orientation() const
{
    return p->orientation;
}

void QScItemBase::setOrientation(Qt::Orientation orientation)
{
    if (p->orientation != orientation)
    {
        p->orientation = orientation;
        updatePaths(pos(), boundingRect().size());
    }
}

QSizeF QScItemBase::size() const
{
    return p->size;
}

void QScItemBase::setSize(const QSizeF &size)
{
    if (p->size != size)
    {
        updatePaths(pos(), size);
    }
}

bool QScItemBase::checkable() const
{
    return p->checkable;
}

void QScItemBase::setCheckable(bool checkable)
{
    if (p->checkable != checkable)
    {
        p->checkable = checkable;
        p->checked = false;
        update();
    }
}

bool QScItemBase::checked() const
{
    return p->checked;
}

void QScItemBase::setChecked(bool checked)
{
    if (p->checkable &&
        p->checked != checked)
    {
        p->checked = checked;
        update();
    }
}

void QScItemBase::setTimer(int elasped)
{
    if (elasped < 0)
    {
        return;
    }

    if (elasped == 0)
    {
        p->times = 0;
        QTimer::singleShot(0, this, SLOT(timeout()));
    }

    if (!p->timer)
    {
        p->timer = new QTimer(this);
        connect(p->timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    if (p->timer->isActive())
    {
        p->timer->stop();
    }

    p->times = 0;
    p->elapsed = elasped;
    p->timer->start(elasped);
}

void QScItemBase::stopTimer()
{
    if (p->timer)
    {
        p->timer->stop();
        p->times = 0;
    }
}

qulonglong QScItemBase::times() const
{
    return p->times;
}

int QScItemBase::elasped() const
{
    return p->elapsed;
}

QScItemBase* QScItemBase::mask() const
{
    return p->mask;
}

void QScItemBase::setMask(QScItemBase *mask)
{
    if (mask == this)
    {
        return;
    }

    if (!mask &&
        p->mask)
    {
        p->mask->setParentItem(0);
        p->mask = NULL;
        return;
    }

    mask->setParentItem(0);
    if (mask->scene() != scene())
    {
        mask->scene()->removeItem(mask);
    }

    mask->setParentItem(this);
    mask->setZValue(zValue() + 0xffff);
    p->mask = mask;

    if (p->maskable)
    {
        updateItemsPos();
    }
}

bool QScItemBase::maskable() const
{
    return p->maskable;
}

void QScItemBase::setMaskable(bool maskable)
{
    p->maskable = maskable;

    if (!p->mask)
    {
        return;
    }

    p->mask->setVisible(maskable);
    if (maskable)
    {
        p->mask->setTimer(p->mask->elasped());
    }
    else
    {
        p->mask->stopTimer();
    }

    updateItemsPos();
}

QScItemBase *QScItemBase::parentItem() const
{
    QGraphicsItem *item = QGraphicsObject::parentItem();
    if (item)
    {
        return dynamic_cast<QScItemBase *>(item);
    }

    return NULL;
}

QList<QScItemBase *> QScItemBase::childItems(int type) const
{
    QList<QScItemBase *> items;
    foreach (QGraphicsItem *item, QGraphicsObject::childItems())
    {
        if (item->type() == type)
        {
            items << dynamic_cast<QScItemBase *>(item);
        }
    }

    return items;
}

QList<QScItemBase *> QScItemBase::prevItems() const
{
    return p->prevItems;
}

QList<QScItemBase *> QScItemBase::nextItems() const
{
    return p->nextItems;
}

bool QScItemBase::isSibling(QScItemBase *item) const
{
    if (!item ||
        !scene())
    {
        return false;
    }

    if (!parentItem() &&
        scene() != item->scene())
    {
        return false;
    }

    if (item->parentItem() != parentItem())
    {
        return false;
    }

    return true;
}

bool QScItemBase::isParent(QScItemBase *item) const
{
    if (!item ||
        !scene())
    {
        return false;
    }

    if (item->parentItem() != this)
    {
        return false;
    }

    return true;
}

bool QScItemBase::addNext(QScItemBase *next)
{
    if (!next)
    {
        return false;
    }

    if (p->nextItems.contains(next))
    {
        return true;
    }

    p->nextItems << next;
    next->addPrev(this);

    return true;
}

bool QScItemBase::addPrev(QScItemBase *prev)
{
    if (!prev)
    {
        return false;
    }

    if (p->prevItems.contains(prev))
    {
        return true;
    }

    p->prevItems << prev;
    prev->addNext(this);

    return true;
}

void QScItemBase::take(QScItemBase *item)
{
    p->prevItems.removeOne(item);
    p->nextItems.removeOne(item);
}

void QScItemBase::takeAll()
{
    p->prevItems.clear();
    p->nextItems.clear();
}

QVariant QScItemBase::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionHasChanged ||
        change == ItemScaleHasChanged)
    {
        updateItemsPos();

        foreach (QScItemBase *prevItem, p->prevItems)
        {
            if (prevItem->isSibling(this))
            {
                prevItem->updateItemsPos();
            }
        }

        foreach (QScItemBase *nextItem, p->nextItems)
        {
            if (nextItem->isSibling(this))
            {
                nextItem->updateItemsPos();
            }
        }
    }

    return QGraphicsObject::itemChange(change, value);
}

bool QScItemBase::event(QEvent *ev)
{
    QPointF point;

    switch (ev->type())
    {
    case QEvent::GraphicsSceneMouseMove:
    case QEvent::GraphicsSceneMousePress:
    case QEvent::GraphicsSceneMouseRelease:
    case QEvent::GraphicsSceneMouseDoubleClick:
    {
        QGraphicsSceneContextMenuEvent *evt = dynamic_cast<QGraphicsSceneContextMenuEvent *>(ev);
        if (evt)
        {
            point = evt->pos();
        }

        updateState(ev->type(), point);
        break;
    }
    case QEvent::GraphicsSceneContextMenu:
    {
        QGraphicsSceneContextMenuEvent *evt = dynamic_cast<QGraphicsSceneContextMenuEvent *>(ev);
        if (evt)
        {
            point = evt->pos();
        }

        updateState(ev->type(), point);
        break;
    }
    case QEvent::GraphicsSceneHoverEnter:
    case QEvent::GraphicsSceneHoverMove:
    case QEvent::GraphicsSceneHoverLeave:
    {
        QGraphicsSceneHoverEvent *evt = dynamic_cast<QGraphicsSceneHoverEvent *>(ev);
        if (evt)
        {
            point = evt->pos();
        }

        updateState(ev->type(), point);
        break;
    }

    case QEvent::FocusIn:
    case QEvent::FocusOut:
    {
        updateState(ev->type(), point);
        break;
    }
    }

    return QGraphicsObject::event(ev);
}

QPointF QScItemBase::controlPosIn(QScItemBase *ref)
{
    QRectF rect = boundingRect();
    QPointF point;

    if (ref &&
        isSibling(ref))
    {
        point = pos();
    }
    else
    {
        point = rect.center();
    }

    if (orientation() == Qt::Horizontal)
    {
        point.rx() += scale() * rect.width() / 2;
    }
    else
    {
        point.ry() += scale() * rect.height() / 2;
    }

    return point;
}

QPointF QScItemBase::controlPosOut(QScItemBase *ref)
{
    Q_UNUSED(ref)

    QPointF point = pos();
    if (orientation() == Qt::Horizontal)
    {
        point.rx() -= scale() * boundingRect().width() / 2;
    }
    else
    {
        point.ry() -= scale() * boundingRect().height() / 2;
    }

    return point;
}

void QScItemBase::updateItemsPos()
{

}

void QScItemBase::updateState(QEvent::Type type, QPointF point)
{
    update();

    emit stateChanged(type, point);
}

bool QScItemBase::clear()
{
    return true;
}

void QScItemBase::detachItem(QScItemBase *)
{

}

void QScItemBase::timeout()
{
    timedTask();
}

void QScItemBase::timedTask()
{
    update();
}

QAction *QScItemBase::createMenuAction(QMenu *menu,
                          const QIcon &icon,
                          const QString &text,
                          const QObject *receiver,
                          const char *member,
                          QActionGroup *group,
                          const QVariant &variant)
{
    QAction *action;
    if (receiver &&
        member)
    {
        action = menu->addAction(icon, text, receiver, member);
    }
    else
    {
        action = menu->addAction(icon, text);
    }

    action->setCheckable(true);
    if (group)
    {
        group->addAction(action);
    }

    action->setData(variant);
    return action;
}
