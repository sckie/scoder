#include "qscitemdrawer.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>

static const qreal s_ctrl_width = 20;
static const qreal s_min_height = 10;
static const int s_default_delta = 4;
static const QString s_ctrl_comm = "CTRL";

class QScItemDrawerPrivate
{
    friend class QScItemDrawer;

    struct SIconState
    {
        QGraphicsPixmapItem *item;
        QList<QPixmap> pixmaps;
        int current;
        bool visible;
        QString command;
    };

    QList<SIconState> states;
    int height;
    int current;
    int check;
    QTimer *timer;
};

QScItemDrawer::QScItemDrawer(const QPointF &point,
                             const QSizeF &size,
                             QGraphicsScene *scene,
                             QGraphicsObject *parent)
    : QScItemBase(ItemType_Drawer, parent)
    , p(new QScItemDrawerPrivate)
{
    p->height = qMax(size.height(), s_min_height);
    p->current = -1;
    p->check = -1;
    p->timer = NULL;

    updatePaths(point, size);

    if (scene)
    {
        scene->addItem(this);
    }

    setControlVisible(false);
    addIcon(s_ctrl_comm, QStringList() << "");
}

QScItemDrawer::~QScItemDrawer()
{
    delete p;
}

QPainterPath QScItemDrawer::createPaths(const QSizeF &size) const
{
    Q_UNUSED(size)

    int width = visibleCount() * p->height;
    int height = p->height;
    if (controlVisible())
    {
        width -= height;
        width += s_ctrl_width;
    }

    QPainterPath path;
    path.addRoundedRect(0, -height / 2, width, height, s_default_delta, s_default_delta);

    qreal x = 0;
    for (int index = 0; index < p->states.count(); index++)
    {
        QScItemDrawerPrivate::SIconState state = p->states.at(index);
        if (index == 0)
        {
            if (controlVisible())
            {
                state.item->setPos(x, -height / 2);
                x += s_ctrl_width;
            }
            else
            {
                continue;
            }
        }
        else
        {
            if (state.visible)
            {
                state.item->setPos(x + s_default_delta, -height/ 2 + s_default_delta);
                x += height;
            }
        }
    }

    return path;
}

bool QScItemDrawer::controlVisible() const
{
    if (p->states.isEmpty())
    {
        return false;
    }

    return p->states.first().visible;
}

int QScItemDrawer::indexOf(const QString &comm) const
{
    if (comm.isEmpty())
    {
        return -1;
    }

    int index = -1;
    for (int idx = 1; idx < p->states.count(); idx++)
    {
        if (!p->states.at(idx).visible)
        {
            continue;
        }

        index++;
        if (p->states.at(idx).command == comm)
        {
            return index;
        }
    }

    return -1;
}

int QScItemDrawer::currentIndex() const
{
    return p->current;
}

bool QScItemDrawer::addIcon(const QString &comm, const QStringList &states, const QString &tip)
{
    if (states.isEmpty() ||
        comm.isEmpty())
    {
        return false;
    }

    foreach (const QScItemDrawerPrivate::SIconState &state, p->states)
    {
        if (state.command == comm)
        {
            return true;
        }
    }

    QStringList _states = states;
    switch (_states.count())
    {
    case 1:
    {
        _states << _states.first();
        _states << _states.first();
        _states << _states.first();
        break;
    }
    case 2:
    {
        _states << _states.last();
        _states << _states.first();
        break;
    }
    case 3:
    {
        _states << _states.first();
        break;
    }
    }

    QScItemDrawerPrivate::SIconState state;
    for (int index = 0; index < 4; index++)
    {
        QPixmap pixmap(_states.at(index));
        if (pixmap.isNull())
        {
            return false;
        }

        if (p->states.isEmpty())
        {
            pixmap = pixmap.scaled(s_ctrl_width - s_default_delta,
                                   p->height - 2 * s_default_delta,
                                   Qt::KeepAspectRatioByExpanding,
                                   Qt::SmoothTransformation);
        }
        else
        {
            pixmap = pixmap.scaled(p->height - 2 * s_default_delta,
                                   p->height - 2 * s_default_delta,
                                   Qt::KeepAspectRatioByExpanding,
                                   Qt::SmoothTransformation);

        }

        state.pixmaps << pixmap;
    }

    state.item = new QGraphicsPixmapItem(state.pixmaps.first(), this);
    state.item->setTransformationMode(Qt::SmoothTransformation);
    state.item->setToolTip(tip);
    state.current = 0;
    state.visible = true;
    state.command = comm;

    if (p->states.isEmpty())
    {
        state.item->setVisible(controlVisible());
    }

    p->states << state;
    updatePaths(pos(), size());

    return true;
}

void QScItemDrawer::setControlVisible(bool visible)
{
    setItemVisible(s_ctrl_comm, visible);
}

void QScItemDrawer::setCurrentIndex(int index)
{
    if (!checkable())
    {
        return;
    }

    if (index < 0)
    {
        if (p->check >= 0)
        {
            QScItemDrawerPrivate::SIconState &state = p->states[p->check];
            state.item->setPixmap(state.pixmaps.at(0));
            state.current = 0;

            update();
        }

        return;
    }

    int visibleIdx = -1;
    for (int idx = 0; idx < p->states.count(); idx++)
    {
        QScItemDrawerPrivate::SIconState &state = p->states[idx];
        if (state.visible)
        {
            if (++visibleIdx == index)
            {
                setCurrentState(idx, 1);
                return;
            }
        }
    }
}

void QScItemDrawer::setItemVisible(const QString &comm, bool visible)
{
    for (int index = 0; index < p->states.count(); index++)
    {
        QScItemDrawerPrivate::SIconState &state = p->states[index];
        if (state.command == comm &&
            state.item)
        {
            if (state.visible == visible)
            {
                return;
            }

            state.visible = visible;
            state.item->setVisible(visible);

            updatePaths(pos(), size());
            return;
        }
    }
}

void QScItemDrawer::setItemTip(const QString &comm, const QString &tip)
{
    foreach (const QScItemDrawerPrivate::SIconState &state, p->states)
    {
        if (state.command == comm &&
            state.item)
        {
            state.item->setToolTip(tip);
            return;
        }
    }
}

bool QScItemDrawer::clear()
{
    p->height = s_min_height;
    p->check = -1;
    p->current = -1;

    if (p->timer)
    {
        delete p->timer;
        p->timer = NULL;
    }

    foreach (const QScItemDrawerPrivate::SIconState &state, p->states)
    {
        delete state.item;
    }

    p->states.clear();
    updatePaths(pos(), size());

    return true;
}

void QScItemDrawer::push()
{
    int index = 0;
    for (index = 1; index < p->states.count(); index++)
    {
        if (!p->states.at(index).visible)
        {
            break;
        }
    }

    if (index > 0 &&
        index < p->states.count())
    {
        if (index == p->states.count() - 1 &&
            p->timer &&
            p->timer->isActive())
        {
            p->timer->stop();
        }

        p->states[index].visible = true;
        p->states[index].item->setVisible(true);

        updatePaths(pos(), size());
    }
}

void QScItemDrawer::pop()
{
    int index = p->states.count() - 1;
    for (; index > 0; index--)
    {
        if (p->states.at(index).visible)
        {
            break;
        }
    }

    if (index > 0 &&
        index < p->states.count())
    {
        p->states[index].visible = false;
        p->states[index].item->setVisible(false);

        if (index == 1 &&
            p->timer &&
            p->timer->isActive())
        {
            p->timer->stop();
        }

        updatePaths(pos(), size());
    }
}

void QScItemDrawer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipPath(shape());
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawPath(shape());

    if (p->current > 0)
    {
        QRectF rect = currentItemRect(p->current);
        rect.adjust(1, 1, -1, -1);

        QRadialGradient linear(rect.center(), rect.height(), rect.center());
        linear.setColorAt(0, Qt::white);
        linear.setColorAt(0.9, Qt::white);
        linear.setColorAt(1, Qt::lightGray);
        painter->setBrush(linear);
        painter->setPen(Qt::lightGray);
        painter->drawRect(rect);
    }

    if (checkable())
    {
        for (int index = 0; index < p->states.count(); index++)
        {
            const QScItemDrawerPrivate::SIconState &state = p->states.at(index);
            if (state.current == 1 &&
                state.visible)
            {
                QRectF rect = currentItemRect(index);
                rect.adjust(1, 1, -1, -1);

                QRadialGradient linear(rect.center(), rect.height(), rect.center());
                linear.setColorAt(0, Qt::white);
                linear.setColorAt(0.9, Qt::lightGray);
                linear.setColorAt(1, Qt::lightGray);
                painter->setBrush(linear);
                painter->setPen(Qt::lightGray);
                painter->drawRect(rect);
            }
        }
    }
}

void QScItemDrawer::updateState(QEvent::Type type, const QPointF &point)
{
    switch (type)
    {
    case QEvent::GraphicsSceneHoverMove:
    {
        setCurrentState(currentItem(point), 2);
        break;
    }
    case QEvent::GraphicsSceneHoverLeave:
    {
        setCurrentState(-1, 0);
        break;
    }
    case QEvent::GraphicsSceneMousePress:
    {
        int index = currentItem(point);
        setCurrentState(index, 1);

        if (index >= 0)
        {
            emit clicked(p->states.at(index).command, true);
        }

        if (index == 0)
        {
            if (!p->timer)
            {
                p->timer = new QTimer(this);
            }

            if (!p->timer->isActive())
            {
                if (p->states.first().visible &&
                    visibleCount() == 1)
                {
                    disconnect(p->timer, SIGNAL(timeout()), this, SLOT(pop()));
                    connect(p->timer, SIGNAL(timeout()), this, SLOT(push()));
                }
                else
                {
                    disconnect(p->timer, SIGNAL(timeout()), this, SLOT(push()));
                    connect(p->timer, SIGNAL(timeout()), this, SLOT(pop()));
                }

                p->timer->start(500);
            }
        }

        break;
    }
    case QEvent::GraphicsSceneMouseRelease:
    {
        int index = currentItem(point);
        setCurrentState(index, 1);

        if (index >= 0)
        {
            emit clicked(p->states.at(index).command, false);
        }

        break;
    }
    case QEvent::GraphicsSceneMouseDoubleClick:
    {
        int index = currentItem(point);
        setCurrentState(index, 1);

        if (index >= 0)
        {
            emit dbclicked(p->states.at(index).command);
        }

        break;
    }
    }

    QScItemBase::updateState(type, point);
}

int QScItemDrawer::visibleCount() const
{
    int count = 0;
    foreach (const QScItemDrawerPrivate::SIconState &state, p->states)
    {
        if (state.visible)
        {
            count++;
        }
    }

    return count;
}

int QScItemDrawer::currentItem(const QPointF &point) const
{
    if (point.x() < 0 ||
        point.x() > size().width() ||
        point.y() < -p->height / 2 ||
        point.y() > p->height / 2)
    {
        return -1;
    }

    int index = -1;
    if (controlVisible())
    {
        if (point.x() < s_ctrl_width)
        {
            return 0;
        }

        index = int((point.x() - s_ctrl_width) / p->height) + 1;
    }
    else
    {
        index = int(point.x() / p->height);
    }

    int tmp = -1;
    for (int idx = 0; idx < p->states.count(); idx++)
    {
        if (p->states.at(idx).visible &&
            ++tmp == index)
        {
            return idx;
        }
    }

    return -1;
}

QRectF QScItemDrawer::currentItemRect(int index) const
{
    QRectF rect;
    if (index < 0 ||
        index > p->states.count() - 1)
    {
        return rect;
    }

    if (!p->states.at(index).visible)
    {
        return rect;
    }

    int visibleIndex = -1;
    foreach (const QScItemDrawerPrivate::SIconState &state, p->states)
    {
        if (state.visible)
        {
            visibleIndex++;
        }
    }

    if (controlVisible())
    {
        rect.setX(index == 0 ? 0 : s_ctrl_width + (visibleIndex - 1) * p->height);
        rect.setY(-p->height / 2);
        rect.setWidth(index == 0 ? s_ctrl_width : p->height);
        rect.setHeight(p->height);
    }
    else
    {
        rect.setX(visibleIndex * p->height);
        rect.setY(-p->height / 2);
        rect.setWidth(p->height);
        rect.setHeight(p->height);
    }

    return rect;
}

bool QScItemDrawer::setCurrentState(int index, int current)
{
    if (current < 0 ||
        current > 3)
    {
        return false;
    }

    int check = -1;
    if (index >= 0)
    {
        QScItemDrawerPrivate::SIconState &state = p->states[index];
        if (checkable())
        {
            if (current == 1)
            {
                check = index;
            }
        }
        else
        {
            if (state.current != current)
            {
                state.item->setPixmap(state.pixmaps.at(current));
                state.current = current;
            }

            setCursor((current == 0 && controlVisible()) ? Qt::OpenHandCursor : Qt::ArrowCursor);
        }
    }

    if (checkable() &&
        p->check != check &&
        check >= 0)
    {
        if (p->check >= 0)
        {
            QScItemDrawerPrivate::SIconState &state = p->states[p->check];
            state.item->setPixmap(state.pixmaps.at(0));
            state.current = 0;
        }
    }

    p->current = current;
    update();

    return true;
}
