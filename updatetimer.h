#ifndef UpdateTimer_H
#define UpdateTimer_H

#include <QObject>
#include<qmap.h>

//���ڶ�ʱ���º�������
class QTcpSocket;
class  QTimer;
class QThread;
class QEventLoop;
class UpdateTimer:public QObject
{
    Q_OBJECT
public:
    enum ReceiveType{
      NoType,
       SignatureAndName,
        HeadImage,
    };
    UpdateTimer(QObject *parent = nullptr);
    ~UpdateTimer();
    void setMyqq(const QString &arg);
    void stop();
    void start();
    void setTimerInterval(const int&);
    void startTcpScoket();
    void setIp(const QString&);
    void setPort(const quint16&);
private:
    void splitSignatureAndName(QByteArray&data);
    void makePixmap(const QByteArray&);
signals:
    void stopTimer();
    void startTimer();
    void emitResult(const bool &ok);
private slots:
    void writeInstruct();
    void readD();
public:
    QMap<QString,QPixmap>historyMap;//��¼����ͷ��
    QMap<QString,QString>sigMap;//��¼���Ѹ���ǩ��
    QMap<QString,QString>nameMap;//��¼�����ǳ�
private:
    QTimer*timer;
    QThread*t;
    QTcpSocket*tcpsocket;
    QString myqq;
    QString ip;
    quint16 port;
    QEventLoop* loop;

    qint64 size;
    QString number;//����MyQQ
    ReceiveType r_type;//���ܵ����ݱ�ʶ
};

#endif // UpdateTimer_H