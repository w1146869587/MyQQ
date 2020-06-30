#include "qmlimageprovider.h"
#include<qdebug.h>

QmlImageProvider::QmlImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)

{

}

const QPixmap &QmlImageProvider::valueOf(const QString &id)
{
    return  images.value(id);
}

QPixmap QmlImageProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    qDebug()<<"id:"<<id<<*size<<requestedSize;
        return images.value(id);
}

