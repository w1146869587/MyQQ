#include "writethread.h"
#include"global.h"
#include <qsqlquery.h>
#include <qdir.h>
#include <qjsondocument.h>
#include<qjsonobject.h>
#include <qpixmap.h>
#include <qtimer.h>
#include<QXmlStreamReader>
#include <qjsonarray.h>
#include <qdatetime.h>
#include<QDomDocument>
WriteThread::WriteThread(qintptr socketDescriptor, qint64 count,  QObject *parent)
    :QObject(parent),socketDescriptor(socketDescriptor),count(count)
{
    size=0;FT=NoFile;
    tcpsocket=new QTcpSocket(this);//�̳и��࣬�߳��븸��һ��
    if(!tcpsocket->setSocketDescriptor(this->socketDescriptor)) {
        qDebug()<<QStringLiteral("�����׽���ʧ�ܣ�");
        emit error(tcpsocket->error());
        return;
    }
    tcpsocket->setSocketOption(QAbstractSocket::KeepAliveOption,true);//windows����
    connect(tcpsocket,&QTcpSocket::readyRead,this,&WriteThread::readD);
    connect(this,&WriteThread::startTimer,this,&WriteThread::timer);
    connect(tcpsocket,&QTcpSocket::disconnected,this,&WriteThread::disconnected);

}

WriteThread::~WriteThread()
{
    qDebug()<<"~WriteThread()";
}

bool WriteThread::adjustHistoryImg(QByteArray &bytes, const QString &filePath, const QString &fileType)
{
    QPixmap head;
    if(!head.loadFromData(bytes,"png"))return false;
    QDir dir(filePath.left(filePath.lastIndexOf("/")));
    if(!dir.exists()||dir.isEmpty()){
        qDebug()<<"adjustHistoryImg:folder is empty or null"<<dir.path();
        return false;
    }
    QStringList temp;temp<<"*.png";
    QString newName,name;qint8 two;
    QFileInfoList info=dir.entryInfoList(temp,QDir::Files,QDir::Name);
    for (int i = info.length()-1; i >=0;--i) {
        name=info.at(i).fileName();
        two=name.left(2).toShort();
        QFile file(dir.filePath(name));
        if(two>=18)file.remove();
        ++two;
        if(two<10)newName="0"+QString("%1.png").arg(two);
        else newName=QString("%1.png").arg(two);
        bool ok=file.rename(dir.filePath(newName));
        if(!ok)qDebug()<<"warning: alter file is of fuilure, named "<<name;
    }
    bool ok= head.save(dir.filePath("01.png"),"png");
    if(!ok)return ok;
    dir.cdUp();

    QFile infoxml(dir.path()+"/info.xml");
    if(!infoxml.open(QIODevice::ReadOnly)){
        qDebug()<<dir.path()+"/info.xml"<<":opened unsuceessful";
    }
    qDebug()<<"readed "<<infoxml.fileName();
    QXmlStreamReader reader;
    reader.setDevice(&infoxml);
    QVector<QString>friends;
    while (!reader.atEnd()) {
        reader.readNext();
        qDebug()<<"name="<<reader.name().toString();
        if(reader.name().toString()=="friend"&&reader.isStartElement()){
            QString mq=reader.attributes().value("myqq").toString();
            qDebug()<<"found a friend,it's MyQQ is"<<mq;
            friends.append(mq);
        }
    }
    qDebug()<<"number of friends is equal to"<<friends.length();
    //ѭ������ȫ���ļ����󣬲����Ƿ��б���ļ�������ļ���������Щ����ͷ�����˱仯
    for (QString&v :friends) {
        qDebug()<<"marked MyQQ is "<<v<<",this represent to need updating image of head while next to accesses marked floder";
        QDir tagDir("../userData/"+v+"/friendsInfo");
        if(!tagDir.exists()){
            tagDir.mkpath("./");//������ǰĿ¼
        }
        QString tagName="markedHeadImg.json";
        historyImgMuter.lock();
        if(historyImgFiles.contains(v)){
            QFile* markedFile=historyImgFiles.value(v);
            if(markedFile->open(QIODevice::ReadWrite)){
                QByteArray markData=markedFile->readAll();
                QJsonDocument jsonDoc=QJsonDocument::fromJson(markData);
                if(!jsonDoc.isObject()){
                    qDebug()<<"error:file "<<markedFile->fileName()<<" is not a object";
                }
                QJsonObject obj=jsonDoc.object();
                QJsonArray arr=obj.value("myqq").toArray();
                QJsonObject tempObj;
                tempObj.insert("number",QJsonValue(myqq));
                arr.append(QJsonValue(tempObj));
                obj.insert("myqq",QJsonValue(arr));
                jsonDoc=QJsonDocument();
                jsonDoc.setObject(obj);
                markedFile->write(jsonDoc.toJson());
                markedFile->close();
            }else
                qDebug()<<"json write is of failure,named  "<<markedFile->fileName();
        }else {
            QFile* markedFile=new QFile(tagDir.filePath(tagName));
            if(markedFile->open(QIODevice::ReadWrite)){
                QJsonDocument jsonDoc;
                QJsonObject obj;
                QJsonArray arr;
                QJsonObject tempObj;
                tempObj.insert("number",QJsonValue(myqq));
                arr.append(QJsonValue(tempObj));
                obj.insert("myqq",QJsonValue(arr));
                jsonDoc;
                jsonDoc.setObject(obj);
                markedFile->write(jsonDoc.toJson());
                markedFile->close();
                historyImgFiles.insert(v,markedFile);
            }else
                qDebug()<<"json write is of failure,named  "<<markedFile->fileName();
        }
        historyImgMuter.unlock();
    }
    return true;
}

bool WriteThread::updateSignature(QByteArray &bytes)
{
    //�þ�ȷ�����ʱ���100��ֵ��Χ��count����������1���ڿ��Դ򿪵����ݿ����ӣ���100��
    QSqlDatabase db=QSqlDatabase::addDatabase("QODBC",QDateTime::currentDateTime().toString("yy-M-d h:m:s")+QString("%1").arg(count));

    QString connectString = QStringLiteral(
                "DRIVER={sql server};"
                "SERVER=127.0.0.1;"
                "DATABASENAME:qtmanager;"
                "PORT:1433;"
                "UID=sa;"
                "PWD=123456x;");
    db.setDatabaseName(connectString);
    if(db.open())
        qDebug()<<QStringLiteral("�����ݿ�ɹ���");
    else {
        qDebug()<<QStringLiteral("�����ݿ�ʧ�ܣ�");
        return false;
    }
    qDebug()<<"login"<<db.connectionName()<<db.databaseName();
    QSqlQuery query(" use myqq ",db);
    query.prepare(" update userInfo set personalizedSignature=?  where myqq=? ");
    QString sig=QString::fromUtf8(bytes);
    query.addBindValue(QVariant(sig.isEmpty()?"":sig));
    query.addBindValue(QVariant(myqq));
    bool ok= query.exec();
    if(ok){
        qDebug()<<"query.exec() successfully"  ;
    }else  {
        qDebug()<<"query.exec() unsuccessfully";
        return ok;
    }
    QFile infoxml("../userData/"+myqq+"/info.xml");
    if(!infoxml.open(QIODevice::ReadWrite)){
        qDebug()<<myqq<<":opened info.xml  unsuceessful";
    }
    qDebug()<<"readed "<<infoxml.fileName();
    QDomDocument doc;
    doc.setContent(&infoxml);
    infoxml.close();
    QDomElement dele=doc.documentElement();
dele=dele.firstChildElement("friendGroup");
QDomNodeList listGroup=dele.childNodes();
QVector<QString>friends;//������Ѻ���
for (int var = 0; var < listGroup.size(); ++var) {
    QDomElement ele=listGroup.at(var).toElement();
    qDebug()<<ele.tagName();
    if(ele.hasChildNodes()){
        QDomElement friendEle=ele.firstChild().toElement();
        while(!friendEle.isNull()){
            QString mq=friendEle.attribute("myqq");
            if(!mq.isEmpty()){
                qDebug()<<"a number is found:"<<mq;
                friends.append(mq);
                if(mq==myqq){
                   QDomElement myself=friendEle.firstChildElement(QStringLiteral("����ǩ��"));
                   if(!myself.isNull()){
                       qDebug()<<"find myself "<<mq<<" signature:"<<myself.text();
                     QDomElement newele=doc.createElement(QStringLiteral("����ǩ��"));
                     QDomText t=doc.createTextNode(QString::fromUtf8(bytes));//����ǩ��
                     newele.appendChild(t);
                     friendEle.replaceChild(newele,myself);//�滻
                   }
                }
            }
            friendEle=friendEle.nextSiblingElement();
        }
    }
}
infoxml.open(QIODevice::WriteOnly);//ֻд����
QTextStream stream(&infoxml);
stream.setCodec("utf-8");
doc.save(stream,4,QDomNode::EncodingFromTextStream);
infoxml.close();

    qDebug()<<"number of friends is equal to"<<friends.length();
    //ѭ������ȫ���ļ����󣬲����Ƿ��б���ļ�������ļ���������Щ����ͷ�����˱仯�ĺ���
    for (QString&v :friends) {
        QDir tagDir("../userData/"+v+"/friendsInfo");
        qDebug()<<"marked MyQQ is "<<v<<",this represent to need updating user' infomation of signature   while next to accesses marked floder";
        if(!tagDir.exists()){
            tagDir.mkpath("./");//������ǰĿ¼
        }
        QString tagName="markedSignatureAndName.json";
        sigMuter.lock();
        if(sigFiles.contains(v)){
            QFile* markedFile=sigFiles.value(v);
            if(markedFile->open(QIODevice::ReadWrite)){
                QByteArray markData=markedFile->readAll();
                QJsonDocument json=QJsonDocument::fromJson(markData);
                QJsonObject obj=json.object();
                QJsonArray arr=obj.value("signature").toArray();
                QJsonObject temp;
                temp.insert("signature",QJsonValue(QString::fromUtf8(bytes)));
                temp.insert("number",QJsonValue(myqq));
                arr.append(temp);
                json=QJsonDocument();
                obj.insert("signature",QJsonValue(arr));
                json.setObject(obj);
                markedFile->write(json.toJson());
                markedFile->close();
            }else
                qDebug()<<"json write is of failure,named  "<<markedFile->fileName();
        }else {
            QFile* markedFile=new QFile(tagDir.filePath(tagName));
            if(markedFile->open(QIODevice::ReadWrite)){
                QJsonDocument json;
                QJsonObject obj;
                QJsonArray arr;
                QJsonObject temp;
                temp.insert("signature",QJsonValue(QString::fromUtf8(bytes)));
                temp.insert("number",QJsonValue(myqq));
                arr.append(temp);
                obj.insert("signature",QJsonValue(arr));
                json.setObject(obj);
                markedFile->write(json.toJson());
                markedFile->close();
                sigFiles.insert(v,markedFile);
            }else{
                qDebug()<<"json write is of failure,named  "<<markedFile->fileName();
             delete markedFile,markedFile=nullptr;
            }
        }
        sigMuter.unlock();
    }
    return ok;
}

void WriteThread::timer()
{
    //���̵߳��ö�ʱ��
    QTimer::singleShot(60000,Qt::CoarseTimer,this,[=](){
        qDebug()<<"timer 60s exit";
        emit finished();
        qDebug()<<"thread had exited";
    });
}



void WriteThread::readD()
{
    while (tcpsocket->bytesAvailable()>size&&size>=0) {
        if(size==0){
            QByteArray header=tcpsocket->read(1);
            QDataStream stream(&header,QIODevice::ReadOnly);
            stream.setVersion(QDataStream::Qt_4_0);
            quint8 l;
            stream>>l;
            if(l<=0){
                qDebug()<<"l="<<l;
                continue;
            }
            size=l;
        }
        if(size!=1){
            QByteArray data=tcpsocket->read(size);
            QJsonDocument jsondoc=QJsonDocument::fromBinaryData(data);
            if(jsondoc.isObject()){
                QJsonObject obj=jsondoc.object();
                QString in=obj.value("instruct").toString();
                //4 ������ʷͷ��
                if(in=="4"){
                    QString content=obj.value("content").toString();
                    if(content=="historyHeadImg"){
                        myqq=obj.value("myqq").toString();
                        if(myqq.isEmpty()){
                            tcpsocket->disconnectFromHost();
                            return;
                        }
                        fileName="../userData/"+myqq+"/historyHeadImg/01.png";
                        FT=HistoryHeadImage;
                        size=1;
                        continue;
                    }
                    //���¸���ǩ��
                }else if(in=="5"){
                    QString content=obj.value("content").toString();
                    if(content=="updateSianature"){
                        myqq=obj.value("myqq").toString();
                        if(myqq.isEmpty()){
                            tcpsocket->disconnectFromHost();
                            return;
                        }
                        FT=Signature;
                        size=1;
                        continue;
                    }
                }
            }
        }else{
            QByteArray data=tcpsocket->readAll();
            switch (FT) {
            case HistoryHeadImage:
                bytes.append(data);
                break;
            case Signature:
                bytes.append(data);
                break;
            default:
                return;
            }
        }
    }
}

void WriteThread::disconnected()
{
    qDebug()<<"  Write QTcpSocket disconnected";
    size=0;
    switch (FT) {
    case HistoryHeadImage:
        if(adjustHistoryImg(bytes,fileName,"png")){
            qDebug()<<"added history image successfully";
        }
        break;
    case Signature:
        if(updateSignature(bytes)){
            qDebug()<<"updated signature successfully";
        }
        break;
    default:
        break;
    }
    qDebug()<<"emit finished()";
    emit finished();
}