#include "qscitemvideo.h"
#include <QPainterPath>
#include <QPainter>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QGraphicsVideoItem>
#include <QGraphicsScene>

class QScItemVideoPrivate
{
    friend class QScItemVideo;

    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QGraphicsVideoItem *view;
};

QScItemVideo::QScItemVideo(const QStringList &playlist,
                           const QPointF &point,
                           const QSizeF &size,
                           QGraphicsScene *scene,
                           QGraphicsObject *parent)
    : QScItemBase(ItemType_Video, parent)
    , p(new QScItemVideoPrivate)
{
    setFlags(ItemIsSelectable |
             ItemSendsGeometryChanges |
             ItemIsMovable |
             ItemIsFocusable);

    p->view = new QGraphicsVideoItem(this);
    p->view->setAspectRatioMode(Qt::KeepAspectRatio);

    updatePaths(point, size);

    if (scene)
    {
        scene->clearSelection();
        scene->addItem(this);
    }

    p->player = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    p->player->setVideoOutput(p->view);

    p->playlist = new QMediaPlaylist(this);
    p->playlist->setPlaybackMode(QMediaPlaylist::Loop);

    p->player->setPlaylist(p->playlist);

    foreach (QString list, playlist)
    {
        p->playlist->addMedia(QUrl::fromLocalFile(list));
    }

    connect(p->player, SIGNAL(durationChanged(qint64)), this, SLOT(updateDuration(qint64)));
    connect(p->player, SIGNAL(positionChanged(qint64)), this, SLOT(updatePosition(qint64)));
    connect(p->player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updateVideoState(QMediaPlayer::State)));
    connect(p->playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePlaylist(int)));
    connect(p->playlist, SIGNAL(playbackModeChanged(QMediaPlaylist::PlaybackMode)), this, SLOT(updatePlayback(QMediaPlaylist::PlaybackMode)));
}

QScItemVideo::~QScItemVideo()
{
    delete p;
}

QPainterPath QScItemVideo::createPaths(const QSizeF &size) const
{
    QPainterPath path;
    path.addRect(-size.width() / 2,
                 -size.height() / 2,
                 size.width(),
                 size.height());

    return path;
}

void QScItemVideo::updatePaths(const QPointF &point, const QSizeF &size)
{
    if (p->view)
    {
        p->view->setSize(size);
        p->view->setPos(-size.width() / 2, -size.height() / 2);
    }

    QScItemBase::updatePaths(point, size);
}

void QScItemVideo::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setPen(pen());
    painter->setBrush(brush());
    painter->setFont(font());

    painter->drawPath(shape());
}

QMediaPlaylist::PlaybackMode QScItemVideo::playbackMode() const
{
    return p->playlist->playbackMode();
}

QMediaPlayer *QScItemVideo::player() const
{
    return p->player;
}

QMediaPlaylist *QScItemVideo::playlist() const
{
    return p->playlist;
}

void QScItemVideo::setPlaybackMode(QMediaPlaylist::PlaybackMode mode)
{
    p->playlist->setPlaybackMode(mode);
}

Qt::AspectRatioMode QScItemVideo::aspectRatioMode() const
{
    return p->view->aspectRatioMode();
}

void QScItemVideo::setAspectRatioMode(Qt::AspectRatioMode mode)
{
    p->view->setAspectRatioMode(mode);
}

void QScItemVideo::play(int current)
{
    if (current >= 0 &&
        p->playlist->mediaCount() > current + 1)
    {
        return;
    }

    if (p->playlist->currentIndex() == current &&
        p->player->state() == QMediaPlayer::PlayingState)
    {
        return;
    }

    if (current >= 0)
    {
        p->playlist->setCurrentIndex(current);
    }

    p->player->play();
}

void QScItemVideo::pause()
{
    p->player->pause();
}

void QScItemVideo::stop()
{
    p->player->stop();
}

void QScItemVideo::addPlaylist(const QStringList &playlist)
{
    foreach (QString list, playlist)
    {
        p->playlist->addMedia(QUrl::fromLocalFile(list));
    }
}

void QScItemVideo::updateDuration(qint64)
{

}

void QScItemVideo::updatePosition(qint64)
{

}

void QScItemVideo::updatePlaylist(int)
{

}

void QScItemVideo::updateVideoState(QMediaPlayer::State)
{
}

void QScItemVideo::updatePlayback(QMediaPlaylist::PlaybackMode)
{

}
