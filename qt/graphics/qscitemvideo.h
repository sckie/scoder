#ifndef QSCITEMVIDEO_H
#define QSCITEMVIDEO_H

#include <qscitembase.h>
#include <QMediaPlayer>
#include<QMediaPlaylist>

class QScItemVideoPrivate;
class QScItemVideo : public QScItemBase
{
    Q_OBJECT

public:
    QScItemVideo(const QStringList &playlist,
                 const QPointF &point,
                 const QSizeF &size,
                 QGraphicsScene *scene,
                 QGraphicsObject *parent = 0);
    virtual ~QScItemVideo();

public:
    QMediaPlayer *player() const;
    QMediaPlaylist *playlist() const;

    QMediaPlaylist::PlaybackMode playbackMode() const;
    void setPlaybackMode(QMediaPlaylist::PlaybackMode mode);

    Qt::AspectRatioMode aspectRatioMode() const;
    void setAspectRatioMode(Qt::AspectRatioMode mode);

    void addPlaylist(const QStringList &playlist);

public slots:
    void play(int current = -1);
    void pause();
    void stop();

protected slots:
    virtual void updateDuration(qint64);
    virtual void updatePosition(qint64);
    virtual void updatePlaylist(int);
    virtual void updateVideoState(QMediaPlayer::State);
    virtual void updatePlayback(QMediaPlaylist::PlaybackMode);

protected:
    virtual QPainterPath createPaths(const QSizeF &size) const;
    virtual void updatePaths(const QPointF &point, const QSizeF &size = QSizeF(0, 0));
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QScItemVideoPrivate *p;
};

#endif // QSCITEMVIDEO_H
