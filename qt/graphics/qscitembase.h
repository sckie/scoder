#ifndef QSCITEMBASE_H
#define QSCITEMBASE_H

/*
 * 图形项定义
 * 	add by scoder 20171201
 */

#include <QGraphicsObject>
#include <QBrush>
#include <QPen>
#include <QFont>
#include <QAction>
#include <QActionGroup>
#include <QEvent>

class QScItemBasePrivate;
class QScItemBase : public QGraphicsObject
{
    Q_OBJECT

    Q_PROPERTY(QBrush brush READ brush WRITE setBrush)
    Q_PROPERTY(QPen pen READ pen WRITE setPen)
    Q_PROPERTY(QFont font READ font WRITE setFont)
    Q_PROPERTY(QString name READ name WRITE setName)
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QSizeF size READ size WRITE setSize)
    Q_PROPERTY(bool checkable READ checkable WRITE setCheckable)
    Q_PROPERTY(bool checked READ checked WRITE setChecked)
    Q_PROPERTY(bool maskable READ maskable WRITE setMaskable)

public:
    enum ItemType
    {
        ItemType_Line = UserType,
        ItemType_Drawer,
        ItemType_Folder,
        ItemType_UserType,
    };
    Q_ENUMS(ItemType)

public:
    QScItemBase(int type, QGraphicsObject *parent = 0);
    virtual ~QScItemBase();

public:
    virtual int type() const;
    virtual QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    virtual void updatePaths(const QPointF &point, const QSizeF &size = QSizeF(0, 0));

public:
    QPen pen() const;
    void setPen(const QPen &pen);

    QBrush brush() const;
    void setBrush(const QBrush &brush);

    QFont font() const;
    void setFont(const QFont &font);

    QString name() const;
    void setName(const QString &name);

    QString text() const;
    void setText(const QString &text);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

    QSizeF size() const;
    void setSize(const QSizeF &size);

    bool checkable() const;
    void setCheckable(bool checkable);

    bool checked() const;
    void setChecked(bool checked);

    void setTimer(int elasped);
    void stopTimer();
    qulonglong times() const;
    int elasped() const;

    QScItemBase* mask() const;
    void setMask(QScItemBase *mask);

    bool maskable() const;
    void setMaskable(bool maskable);

signals:
    void stateChanged(QEvent::Type, QPointF);

public:
    QScItemBase *parentItem() const;
    QList<QScItemBase *> childItems(int type) const;
    QList<QScItemBase *> prevItems() const;
    QList<QScItemBase *> nextItems() const;

    bool isSibling(QScItemBase *item) const;
    bool isParent(QScItemBase *item) const;

public:
    bool addNext(QScItemBase *next);
    bool addPrev(QScItemBase *prev);
    void take(QScItemBase *item);
    void takeAll();

protected:
    virtual bool event(QEvent *ev);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);

public:
    virtual QPointF controlPosIn(QScItemBase *ref = 0);
    virtual QPointF controlPosOut(QScItemBase *ref = 0);
    virtual void updateItemsPos();
    virtual void updateState(QEvent::Type type, QPointF point);
    virtual bool clear();
    virtual void detachItem(QScItemBase *);

protected:
    virtual void timedTask();
    virtual QPainterPath createPaths(const QSizeF &size) const = 0;

protected:
    static QAction *createMenuAction(QMenu *menu,
                                     const QIcon &icon,
                                     const QString &text,
                                     const QObject *receiver,
                                     const char *member,
                                     QActionGroup *group = 0,
                                     const QVariant &variant = QVariant());

private slots:
    void timeout();

private:
    QScItemBasePrivate *p;
};

#endif // QSCITEMBASE_H
