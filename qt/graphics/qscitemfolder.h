#ifndef QSCITEMFOLDER_H
#define QSCITEMFOLDER_H

#include <qscitembase.h>

class QScItemFolderPrivate;
class QScItemFolder : public QScItemBase
{
    Q_OBJECT

public:
    QScItemFolder(const QPointF &point,
                  const QSizeF &size,
                  QGraphicsScene *scene,
                  QGraphicsObject *parent = 0);
    virtual ~QScItemFolder();

public:
    bool addItem(QScItemBase *item);
    bool switchPage(int page);

public:
    virtual void updateItemsPos();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QPainterPath createPaths(const QSizeF &size) const;
    virtual void updateState(QEvent::Type type, const QPointF &point);

protected slots:
    void slot_switchPage(QString, bool);

protected:
    QList<QScItemBase *> pageItems(int page) const;

private:
    QScItemFolderPrivate *p;
};

#endif // QSCITEMFOLDER_H
