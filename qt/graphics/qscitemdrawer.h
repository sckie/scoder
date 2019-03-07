#ifndef QSCITEMDRAWER_H
#define QSCITEMDRAWER_H

#include <qscitembase.h>

class QScItemDrawerPrivate;
class QScItemDrawer : public QScItemBase
{
    Q_OBJECT

public:
    QScItemDrawer(const QPointF &point,
                  const QSizeF &size,
                  QGraphicsScene *scene,
                  QGraphicsObject *parent = 0);
    virtual ~QScItemDrawer();

public:
    bool controlVisible() const;
    int indexOf(const QString &comm) const;
    int currentIndex() const;

public:
    bool addIcon(const QString &comm, const QStringList &states, const QString &tip = "");
    void setControlVisible(bool visible);
    void setCurrentIndex(int index);
    void setVisible(const QString &comm, bool visible);
    void setTooltip(const QString &comm, const QString &tip);

public:
    virtual bool clear();

public slots:
    void push();
    void pop();

signals:
    void clicked(QString, bool);
    void dbclicked(QString);

protected:
    virtual QPainterPath createPaths(const QSizeF &size) const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void updateState(QEvent::Type type, const QPointF &point);

protected:
    int visibleCount() const;
    int currentItem(const QPointF &point) const;
    QRectF currentItemRect(int index) const;
    bool setCurrentState(int index, int state);

private:
    QScItemDrawerPrivate *p;
};

#endif // QSCITEMDRAWER_H
