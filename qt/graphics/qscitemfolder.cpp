#include "qscitemfolder.h"
#include "qscitemdrawer.h"
#include <QGraphicsScene>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QGraphicsDropShadowEffect>
#include <QPainter>

static const int s_columnCount = 2;
static const int s_rowCount = 3;
static const int s_pageHeight = 15;

class QScItemFolderPrivate
{
    friend class QScItemFolder;

    QList<QScItemBase *> items;
    QScItemDrawer *pages;
    int current;
    int count;
};

QScItemFolder::QScItemFolder(const QPointF &point,
                  const QSizeF &size,
                  QGraphicsScene *scene,
                  QGraphicsObject *parent)
    : QScItemBase(ItemType_Folder, parent)
    , p(new QScItemFolderPrivate)
{
    p->pages = NULL;
    p->current = 0;
    p->count = 1;

    updatePaths(point, size);

    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemSendsGeometryChanges |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsFocusable);

    setZValue(101);

    if (scene)
    {
        scene->addItem(this);
        scene->clearSelection();
    }

    setOrientation(Qt::Vertical);
    setBrush(Qt::lightGray);
    setPen(QColor(Qt::lightGray));

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setBlurRadius(6);
    effect->setColor(QColor("#cccccc"));
    effect->setXOffset(-3);
    effect->setYOffset(1);
    setGraphicsEffect(effect);

    QRectF rect = boundingRect();
    p->pages = new QScItemDrawer(QPointF(rect.left() + 10, rect.bottom() - 20), QSizeF(200, 20), scene, this);
    p->pages->setCheckable(true);
    p->pages->setControlVisible(false);
    p->pages->setPen(Qt::NoPen);

    connect(p->pages, SIGNAL(clicked(QString, bool)), this, SLOT(slot_switchPage(QString, bool)));
}

QScItemFolder::~QScItemFolder()
{
    delete p;
}

QPainterPath QScItemFolder::createPaths(const QSizeF &size) const
{
    QPainterPath path;
    path.addRoundedRect(-size.width() / 2,
                        -size.height() / 2,
                        size.width(),
                        size.height(),
                        6,
                        6);

    return path;
}

bool QScItemFolder::addItem(QScItemBase *item)
{
    if (!item)
    {
        return false;
    }

    item->setParentItem(this);

    if (!p->items.contains(item))
    {
        p->items << item;
    }

    updateItemsPos();

    return true;
}

bool QScItemFolder::switchPage(int page)
{
    if (page < 0 ||
        page > p->count - 1)
    {
        return false;
    }

    if (page == p->current)
    {
        return true;
    }

    foreach (QScItemBase *item, pageItems(p->current))
    {
        item->setOpacity(0);
    }

    p->current = page;

    QParallelAnimationGroup *group = new QParallelAnimationGroup(this);
    foreach (QScItemBase *item, pageItems(p->current))
    {
        QPropertyAnimation *animation = new QPropertyAnimation(item, "opacity", this);
        animation->setDuration(500);
        animation->setEndValue(1);
        animation->setEasingCurve(QEasingCurve::Linear);
        group->addAnimation(animation);
    }

    group->start(QAbstractAnimation::DeleteWhenStopped);
    return true;
}

void QScItemFolder::updateItemsPos()
{
    QRectF rect = boundingRect();
    qreal qWidth = rect.width() / s_columnCount;
    qreal qHeight = (rect.height() - 2 * s_pageHeight) / s_rowCount;

    int currentIdx = p->pages->currentIndex();
    p->pages->clear();

    int pageIdx = 0;
    int index = 0;

    bool done = false;
    while(!done)
    {
        if (p->items.isEmpty())
        {
            break;
        }

        for (int rowIdx = 0; rowIdx < s_rowCount && !done; rowIdx++)
        {
            for (int colIdx = 0; colIdx < s_columnCount && !done; colIdx++)
            {
                index = rowIdx * s_columnCount + colIdx + pageIdx * s_rowCount * s_columnCount;
                if (p->items.count() == index + 1)
                {
                    done = true;
                }

                QScItemBase *item = p->items.at(index);
                if (!item)
                {
                    continue;
                }

                item->updatePaths(QPointF(colIdx * qWidth + qWidth / 2 + rect.left(), rowIdx * qHeight + qHeight / 2 + rect.top()),
                                  QSizeF(qWidth - 8, qHeight - 8));
            }
        }

        p->count = ++pageIdx;
        p->pages->addIcon(tr("PAGE_%1").arg(pageIdx), QStringList() << ":/radio.png");
    }

    p->pages->setCurrentIndex(currentIdx);
    p->pages->setPos(rect.center().x() - p->pages->boundingRect().width() / 2, rect.bottom() - s_pageHeight);
}

void QScItemFolder::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setClipPath(shape());
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawPath(shape());
}

void QScItemFolder::updateState(QEvent::Type type, const QPointF &point)
{
    Q_UNUSED(point)

    if (type == QEvent::GraphicsSceneHoverEnter)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "scale", this);
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::InCurve);
        animation->setEndValue(0.8);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    else if (type == QEvent::GraphicsSceneDragLeave)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(this, "scale", this);
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::OutCurve);
        animation->setEndValue(0.5);
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }

    update();
}

void QScItemFolder::slot_switchPage(QString comm, bool down)
{
    if (!down)
    {
        return;
    }

    int currentPage = p->pages->indexOf(comm);
    currentPage = qMax(currentPage, 0);

    switchPage(currentPage);
}

QList<QScItemBase *> QScItemFolder::pageItems(int page) const
{
    QList<QScItemBase *> items;
    if (page < 0 ||
        page >= p->count)
    {
        return items;
    }

    int startIdx = page * s_columnCount * s_rowCount;
    int count = qMin(p->items.count() - startIdx, s_rowCount * s_columnCount);
    for (int index = 0; index < count; index++)
    {
        items << p->items.at(index + startIdx);
    }

    return items;
}
