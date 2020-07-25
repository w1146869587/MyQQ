#include "bigfilesocket.h"
#include<qdebug.h>
#include<QJsonDocument>
#include<qjsonobject.h>
#include<qjsondocument.h>
#include<qjsonobject.h>
#include <qimage.h>
#include<qdir.h>
#include <qtimer.h>
BigFileSocket::BigFileSocket(QObject *parent)
    :QTcpSocket(parent),instruct(QJsonDocument())
{
    connect(this,SIGNAL(error(QAbstractSocket::SocketError)),this ,SLOT(err(QAbstractSocket::SocketError)));
    connect(this,SIGNAL(readyRead()),this ,SLOT(readD()));
    connect(this,SIGNAL(connected()),this ,SLOT(writeD()));
    connect(this,SIGNAL(start()),this ,SLOT(post()));
    connect(this,SIGNAL(result(int,QString,QString)),this ,SLOT(resultSlot(int,QString,QString)));
    connect(this,&QTcpSocket::bytesWritten,this,[=](qint64 bytes){
        qDebug()<<bytes<<" bytes has been readed";
        loop.quit();
        qDebug()<<" loop.quit()";
    });

    //初始化temp，用来保存分批到来的数据
    carrier=QByteArray();
    size=0;
    timeout=40000;//默认40s
}

BigFileSocket::~BigFileSocket()
{
    if(this->isOpen())
        this->close();
    qDebug()<<"~BigFileSocket()";
}

void BigFileSocket::setInstruct(const QString &arg)
{
    QStringList l=arg.split(" ",QString::SkipEmptyParts);
    QJsonObject o;
    try{
        if(l.length()>4)throw -1;
        else if(l.length()<=2)throw -2;
        o.insert("instruct",QJsonValue(l.at(0)));
        o.insert("content",QJsonValue(l.at(1)));
        o.insert("myqq",QJsonValue(l.at(2)));
    }catch(int &except){
        qDebug()<<"this list is overflow,code:"<<except;
    }catch(...){
        qDebug()<<"unkown exception in:32row:bigfilesocket";
    }
    myqq=l.at(2);
    if(l.length()==4){
        o.insert("headerSize",QJsonValue(true));
    }
    //建立目录
    QDir mk;
    QString dir;
    qDebug()<<"has writen headImg";
    dir="../user/"+myqq+"/friends";
    mk.mkpath(dir);
    if(!instruct.isEmpty())
        instruct=QJsonDocument();
    instruct.setObject(o);
}

void BigFileSocket::setIp(const QString &arg)
{
    ip=arg;
}

void BigFileSocket::setPort(const quint16 &arg)
{
    port=arg;
}


bool BigFileSocket::writeImg(const QByteArray &content, const QString &filepath, const char *format)
{

    QImage img;
    qDebug()<<filepath;
    if(!img.loadFromData(content,format))
        return false;
    if(!img.save(filepath,nullptr,0))
        return false;
    return true;
}

void BigFileSocket::setTimeout(const qint64 &t)
{
    timeout=t;
}

void BigFileSocket::readD()
{
    while (bytesAvailable()>=size&&bytesAvailable()>0) {
        if(size==0){
            QByteArray data=this->read(1);
            QDataStream stream(&data,QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_4_0);
            quint8 l;
            stream>>l;
            size=l;
            if(l<=0){
                qDebug()<<"size is less than or equal to zero";
            }
            continue;
        }
        QByteArray data=this->read(size);
        QJsonDocument doc=QJsonDocument::fromJson(data);
        if(doc.isObject()){
            QJsonObject obj=doc.object();
            qDebug()<<obj;
            //好友头像
            if(obj.value("instruct").toString()=="20"){
                qDebug()<<"obj"<<obj.value("result").toString();
                if(obj.value("result").toString()=="writing"){ //写好友头像
                    m_write=Img;
                    qDebug()<<"writing"<<m_write<<size;
                    imgName=obj.value("name").toString();
                    qDebug()<<"img "<<imgName;
                    size=obj.value("size").toVariant().toLongLong();
                    if(size<=0){
                        qDebug()<<"size is less than or equal to zero";
                        emit result(-1);
                    }
                    continue;
                }else if(obj.value("result").toString()=="true"){ //写头像成功
                    m_write=NoFile;//初始化标记枚举
                    size=0;
                    emit result(0,"../user/"+myqq+"/friends/","png");
                    return;
                }else if(obj.value("result").toString()=="false"){ //写头像失败
                    m_write=NoFile;
                    emit result(-1);
                    return;
                }
                //个人历史头像
            }else if(obj.value("instruct").toString()=="40"){
                qDebug()<<"obj"<<obj.value("result").toString();
                if(obj.value("result").toString()=="writing"){ //写头像
                    m_write=Img;
                    qDebug()<<"writing"<<m_write<<size;
                    imgName=obj.value("name").toString();
                    qDebug()<<"img "<<imgName;
                    size=obj.value("size").toVariant().toLongLong();
                    if(size<=0){
                        qDebug()<<"size is less than or equal to zero";
                        emit result(-1);
                    }
                    continue;
                }else if(obj.value("result").toString()=="true"){ //写头像成功
                    m_write=NoFile;//初始化标记枚举
                    size=0;
                    emit result(0,"../user/"+myqq+"/historyHeadImg/","png");
                    return;
                }else if(obj.value("result").toString()=="false"){ //写头像失败
                    m_write=NoFile;
                    emit result(-1);
                    return;
                }
                //个人资料处理
            }else if(obj.value("instruct").toString()=="80"){
                qDebug()<<"obj"<<obj.value("result").toString();
                if(obj.value("result").toString()=="writingJson"){ //写个人资料
                    m_write=SingleJson;//json信息
                    qDebug()<<"writing"<<m_write<<size;
                    qDebug()<<"img "<<imgName;
                    size=obj.value("size").toVariant().toLongLong();
                    if(size<=0){
                        qDebug()<<"size is less than or equal to zero";
                        emit result(-1);
                    }
                    continue;
                }else if(obj.value("result").toString()=="writingPhotoWall"){ //照片墙
                    m_write=Img;//图片
                    qDebug()<<"writing"<<m_write<<size;
                    imgName=obj.value("name").toString();
                    qDebug()<<"img "<<imgName;
                    size=obj.value("size").toVariant().toLongLong();
                    if(size<=0){
                        qDebug()<<"size is less than or equal to zero";
                        emit result(-1);
                    }
                    continue;
                }else if(obj.value("result").toString()=="writingCover"){ //封面
                    m_write=Img;
                    qDebug()<<"writing"<<m_write<<size;
                    imgName=obj.value("name").toString();
                    qDebug()<<"img "<<imgName;
                    size=obj.value("size").toVariant().toLongLong();
                    if(size<=0){
                        qDebug()<<"size is less than or equal to zero";
                        emit result(-1);
                    }
                    continue;
                }else if(obj.value("result").toString()=="true"){ //成功
                    m_write=NoFile;//初始化标记枚举
                    size=0;
                    emit finished(0);
                    return;
                }else if(obj.value("result").toString()=="false"){ //失败
                    m_write=NoFile;
                    size=0;
                    emit finished(-1);
                    return;
                }
            }
        }
        switch (m_write) {
        case Img://传IMG
            img.insert(imgName,data);
            size=0,imgName="";
            break;
        case SingleJson:
            carrier.append(data);
            size=0;
            break;
        default:
            break;
        }
    }
}

void BigFileSocket::writeD()
{
    if(instruct.object().value("headerSize").toBool()){
        qDebug()<<"length "<<4;
        QByteArray length;
        QDataStream stream(&length,QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_0);
        quint8 l=instruct.toBinaryData().size();
        stream<<l;
        this->write(length+instruct.toBinaryData());
    }else
        this->write(instruct.toBinaryData());
    qDebug()<<"started event loop";
    loop.exec();
    qDebug()<<"exited event loop";
    emit writtenInstruction();
}

void BigFileSocket::err(QAbstractSocket::SocketError code)
{
    switch (code) {
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<<"The connection was refused by the peer (or timed out).";
        break;
    case QAbstractSocket::RemoteHostClosedError:
        qDebug()<<"The remote host closed the connection.";
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<"The host address was not found.";
        break;
    case QAbstractSocket::SocketAccessError:
        qDebug()<<"The socket operation failed because the application lacked the required privileges.";
        break;
    case QAbstractSocket::NetworkError:
        qDebug()<<"network error.";
        break;
    case    QAbstractSocket::UnknownSocketError:
        qDebug()<<"An unidentified error occurred.";
        break;
    }
    emit result(-1);
}

void BigFileSocket::post()
{
    this->connectToHost(ip,port);
    QTimer::singleShot(timeout,this,[=](){
        qDebug()<<"QTimer::singleShot "<<timeout<<"s";
        emit finished(-1);
    });
}

void BigFileSocket::resultSlot(int code, const QString& folder, const QString& type)
{

    if(code==-1){
        emit finished(-1);
        return;
    }
    qDebug()<<"result slot viaed";
    foreach(QString key,img.keys()){
        QByteArray data=img.value(key);
        qDebug()<<data.data();
        if(!data.isEmpty()){
            qDebug()<<key<<folder<<type;
            try{
                if(!writeImg(data,folder+key,type.toUtf8().data()))throw false;
            }catch(bool&b){
                qDebug()<<"saved file is of failure,code:"<<b;
            }catch(...){
                qDebug()<<"curred a unkown exception";
                return;
            }
        }
    }
    emit finished(code);
}
