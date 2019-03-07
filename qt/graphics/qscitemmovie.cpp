#include "qscitemmovie.h"
#include <QGraphicsScene>
#include <QPainter>
#include <QPaintEvent>

class QScItemMoviePrivate
{
    friend class QScItemMovie;

    QString		fileName;
    QMovie		*movie;
    QString		error;
    bool		autoFill;
};

QScItemMovie::QScItemMovie(const QString &fileName,
                           const QPointF &point,
                           const QSizeF &size,
                           QGraphicsScene *scene,
                           QGraphicsObject *parent)
    : QScItemBase(ItemType_Movie, parent)
    , p(new QScItemMoviePrivate)
{
    p->fileName = fileName;
    p->autoFill = false;

    updatePaths(point, size);

    setFlags(ItemIsSelectable |
             ItemSendsGeometryChanges |
             ItemIsMovable |
             ItemIsFocusable);

    if (scene)
    {
        scene->clearSelection();
        scene->addItem(this);
    }

    p->movie = new QMovie(this);
    p->movie->setFileName(fileName);
    connect(p->movie, SIGNAL(updated(QRect)), this, SLOT(_q_movieUpdated()));
    connect(p->movie, SIGNAL(error(QImageReader::ImageReaderError)), this, SLOT(_q_movieError(QImageReader::ImageReaderError)));

    p->movie->start();
}

QScItemMovie::~QScItemMovie()
{
    delete p;
}

QPainterPath QScItemMovie::createPaths(const QSizeF &size) const
{
    QPainterPath path;
    path.addRect(-size.width() / 2,
                 -size.height() / 2,
                 size.width(),
                 size.height());

    return path;
}

void QScItemMovie::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRectF rect = boundingRect();
    painter->setClipPath(shape());

    painter->setFont(font());
    painter->setBrush(brush());
    painter->setPen(pen());
    painter->drawPath(shape());

    if (p->movie &&
        p->movie->isValid())
    {
        painter->drawImage(rect, p->movie->currentImage().scaled(rect.width(), rect.height()));
    }
}

QString QScItemMovie::errorString() const
{
    return p->error;
}

QMovie* QScItemMovie::movie() const
{
    return p->movie;
}

void QScItemMovie::setMovie(const QString &movie)
{
    p->error.clear();
    p->movie->setFileName(movie);
    p->movie->start();
}

bool QScItemMovie::autoFill() const
{
    return p->autoFill;
}

void QScItemMovie::setAutoFill(bool autoFill)
{
    if (p->autoFill == autoFill)
    {
        return;
    }

    p->autoFill = autoFill;
    if (p->autoFill)
    {
        updatePaths(pos(), p->movie->currentPixmap().size());
    }
}

void QScItemMovie::_q_movieUpdated()
{
    update();
}

void QScItemMovie::_q_movieError(QImageReader::ImageReaderError err)
{
    switch (err)
    {
    case QImageReader::UnknownError:
    {
        p->error = QString::fromUtf8("未知错误");
        return;
    }
    case QImageReader::FileNotFoundError:
    {
        p->error = QString::fromUtf8("文件无法找到");
        return;
    }
    case QImageReader::DeviceError:
    {
        p->error = QString::fromUtf8("设备错误");
        return;
    }
    case QImageReader::UnsupportedFormatError:
    {
        p->error = QString::fromUtf8("无法支持当前文件格式");
        return;
    }
    case QImageReader::InvalidDataError:
    {
        p->error = QString::fromUtf8("数据错误");
        return;
    }
    }
}
