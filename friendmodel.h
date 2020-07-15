#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include <QObject>
#include<QAbstractListModel>



class FriendData{
public:
    FriendData();
    QString myqq()const;
    QString name()const;
    QString signature()const;
    QString imgPath()const;
    QString tag()const;
    QString grade()const;
    QString status()const;
    QString infoSet()const;
    QString statusSet()const;
    void setMyqq(const QString&);
    void setName(const QString&);
    void setSignature(const QString&);
    void setImgPath(const QString&);
    void setTag(const QString&);
    void setGrade(const QString&);
    void setStatus(const QString&);
    void setInfoSet(const QString&);
    void setStatusSet(const QString&);
private:
    QString m_myqq;//MyQQ
    QString m_name;//�ǳ�
    QString m_signature;//����ǩ��
    QString m_imgPath;//ͷ��·��
    QString m_tag;//��ע
    QString m_grade;//myqq�ȼ�
    QString m_status;//״̬
    QString m_infoSet;//��Ϣ����
    QString m_statusSet;//״̬����
};

class FriendModel:public QAbstractListModel
{
    Q_OBJECT
public:
    enum DataRoles{
        MyQQRole=Qt::DisplayRole,//0
        NameRole,
        SignatureRole,
        ImgPathRole,
        TagRole,
        GradeRole,
        StatusRole,
        InfoSetRole,
        StatusSetRole
    };
    FriendModel(QObject *parent = nullptr);
    ~FriendModel();
    //��������
    Q_INVOKABLE  int rowCount(const QModelIndex &parent=QModelIndex()) const;
    QHash<int,QByteArray>roleNames()const;
    Q_INVOKABLE QString data(const int &, int ) const;
    QVariant data(const QModelIndex &index, int role) const;//���麯������ʵ��
   Q_INVOKABLE int rowOf(const QVariant&var , int role=MyQQRole) const;//���ؽ�ɫ����ֵ������
    //�޸�����
    void insert(int row, FriendData *value);
    Q_INVOKABLE  void remove(const int& row,const int&count=1);
    Q_INVOKABLE  void setData(const int &row,const QString& value, int role=MyQQRole);
    Q_INVOKABLE void append(const QString &myqq, const QString&name, const QString&signature, const QString&imgPath,
                            const QString&tag,const QString&grade, const QString&status, const QString&infoSet, const QString&statusSet);
private:
    QList<FriendData*>m_dataList;
};

#endif // FRIENDMODEL_H