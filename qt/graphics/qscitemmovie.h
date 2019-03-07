#ifndef QSCITEMMOVIE_H
#define QSCITEMMOVIE_H

#include <QMovie>
#include <qscitembase.h>
#include <QMediaPlayer>
#include <QVideoWidget>

class QScItemMoviePrivate;
class QScItemMovie : public QScItemBase
{
    Q_OBJECT

public:
    QScItemMovie(const QString &fileName,
                 const QPointF &point,
                 const QSizeF &size,
                 QGraphicsScene *scene,
                 QGraphicsObject *parent = 0);
    virtual ~QScItemMovie();

public:
    QString errorString() const;

    QMovie* movie() const;
    void setMovie(const QString &movie);

    bool autoFill() const;
    void setAutoFill(bool autoFill);

protected:
    QPainterPath createPaths(const QSizeF &size) const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private slots:
    void _q_movieError(QImageReader::ImageReaderError);
    void _q_movieUpdated();

private:
    QScItemMoviePrivate *p;
};

#endif // QSCITEMMOVIE_H
