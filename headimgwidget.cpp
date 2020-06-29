#include "headimgwidget.h"
#include <QEvent>
#include<QHBoxLayout>
#include <QSlider>
#include<qdebug.h>
#include <qpushbutton.h>
#include<QApplication>
//#pragma comment(lib,"qtquickcontrols2plugind.dll")//载入qml插件库

//qpoint的外联函数 用于qhash
inline uint qHash(const QPoint &key, uint seed)
{
    return qHash(key.x(),seed)*qHash(key.y(),seed);

}


HeadImgWidget::HeadImgWidget(QWidget *parent) : QWidget(parent)
{
    qDebug()<<"init headimgwidget";
    //初始化图片
    imgOut.load(":/images/QWidget/headImgSubtraction.png","png");
    imgIn.load(":/images/QWidget/headImgAddition.png","png");
    imgCw.load(":/images/QWidget/headImgCw.png","png");
    imgAcw.load(":/images/QWidget/headImgAcw.png","png");
    //初始化放大图片信息，用于改变图片颜色
    for (int i = 0; i < imgOut.height(); ++i) {
        QRgb*line=(QRgb*)imgOut.scanLine(i);
        for (int j = 0; j < imgOut.width(); ++j) {
            QPoint pos(j,i);
            QRgb rgb=line[j];
            if(qAlpha(rgb)!=0){
                rgbOut[0].insert(pos,rgb);//原色
                rgbOut[1].insert(pos,qRgba(97,207,248,255));//hover水蓝色
            }else{
                rgbOut[0].insert(pos,rgb);//原色透明
                rgbOut[1].insert(pos,rgb);//hover透明
            }
        }
    }
    for (int i = 0; i < imgIn.height(); ++i) {
        QRgb*line=(QRgb*)imgIn.scanLine(i);
        for (int j = 0; j < imgIn.width(); ++j) {
            QPoint pos(j,i);
            QRgb rgb=line[j];
            if(qAlpha(rgb)!=0){
                rgbIn[0].insert(pos,rgb);//原色
                rgbIn[1].insert(pos,qRgba(97,207,248,255));//hover水蓝色
            }else{
                rgbIn[0].insert(pos,rgb);//原色透明
                rgbIn[1].insert(pos,rgb);//hover透明
            }
        }
    }
    for (int i = 0; i < imgCw.height(); ++i) {
        QRgb*line=(QRgb*)imgCw.scanLine(i);
        for (int j = 0; j < imgCw.width(); ++j) {
            QPoint pos(j,i);
            QRgb rgb=line[j];
            if(qAlpha(rgb)!=0){
                rgbCw[0].insert(pos,rgb);//原色
                rgbCw[1].insert(pos,qRgba(142,146,161,255));//hover灰色
                rgbCw[2].insert(pos,qRgba(97,207,248,255));//press水蓝色
            }else{
                rgbCw[0].insert(pos,rgb);//原色透明
                rgbCw[1].insert(pos,rgb);//hover透明
                rgbCw[2].insert(pos,rgb);//press透明
            }
        }
    }
    for (int i = 0; i < imgAcw.height(); ++i) {
        QRgb*line=(QRgb*)imgAcw.scanLine(i);
        for (int j = 0; j < imgAcw.width(); ++j) {
            QPoint pos(j,i);
            QRgb rgb=line[j];
            if(qAlpha(rgb)!=0){
                rgbAcw[0].insert(pos,rgb);//原色
                rgbAcw[1].insert(pos,qRgba(142,146,161,255));//hover灰色
                rgbAcw[2].insert(pos,qRgba(97,207,248,255));//press水蓝色
            }else{
                rgbAcw[0].insert(pos,rgb);//原色透明
                rgbAcw[1].insert(pos,rgb);//hover透明
                rgbAcw[2].insert(pos,rgb);//press透明
            }
        }
    }
    QPalette p=this->palette();
    this->setAutoFillBackground(true);
    p.setColor(QPalette::Window,QColor::fromRgba(qRgb(255,255,255)));
    this->setPalette(p);
    setFixedSize(350,390);
    //视图
    view=new HeadImgView(this);

    zoomout=new QPushButton(this);
    zoomout->setFixedSize(18,16);
    zoomout->setFlat(true);
    zoomout->setIcon(QIcon(QPixmap::fromImage(imgOut)));
    //这里用palette不行,奇怪
    zoomout->move(0,362);
    QString str="QPushButton{\
            background-color:rgba(255, 255,255);;\
    border.width:0px;\
border: none; /* no border */\
}\
";
zoomout->setStyleSheet(str);


slider=new QSlider(this);
slider->setOrientation(Qt::Horizontal);
slider->resize(230,10);
slider->setRange(1,300);
slider->setTickInterval(10);
slider->setValue(5);
slider->setStyleSheet("QSlider::groove:horizontal {\
                      background: lightgray;\
height:4;\
margin: 2px 0;\
}\
QSlider::handle:horizontal {\
                    background: rgba(97,207,248,255);\
width:10;\
margin: -3px -3px;\
border-radius: 5px;\
}\
");
slider->move( zoomout->x()+ zoomout->width()+6, zoomout->y()+zoomout->height()/2-4);

zoomin=new QPushButton(this);
zoomin->setFixedSize(18,16);
zoomin->setIcon(QIcon(QPixmap::fromImage(imgIn)));
zoomin->move(slider->x()+slider->width()+6,zoomout->y());
zoomin->setStyleSheet(str);
cw=new QPushButton(this);
cw->setFixedSize(16,16);
cw->setIcon(QIcon(QPixmap::fromImage(imgCw)));
cw->move(350-cw->width(),zoomin->y());
cw->setStyleSheet(str);
acw=new QPushButton(this);
acw->setFixedSize(16,16);
acw->setIcon(QIcon(QPixmap::fromImage(imgAcw)));
acw->move(cw->x()-10-acw->width(),cw->y());
acw->setStyleSheet(str);
//加载所有事件到widget，处理hover clicked
zoomin->installEventFilter(this);
zoomout->installEventFilter(this);
cw->installEventFilter(this);
acw->installEventFilter(this);

view->setSlider(slider);
connect(zoomout,&QPushButton::clicked,view,&HeadImgView::zoomOutClicked);
connect(zoomin,&QPushButton::clicked,view,&HeadImgView::zoomInClicked);
connect(cw,&QPushButton::clicked,view,&HeadImgView::cwClicked);
connect(acw,&QPushButton::clicked,view,&HeadImgView::acwClicked);
}

HeadImgWidget::~HeadImgWidget()
{
    if(slider){
        delete slider,slider=nullptr;
        qDebug()<<"slider delete";
    }
    if(view){
        delete view,view=nullptr;
        qDebug()<<"view delete";
    }
    if(zoomin){
        delete zoomin,zoomin=nullptr;
        qDebug()<<"zoomin delete";
    }
    if(zoomout){
        delete zoomout,zoomout=nullptr;
        qDebug()<<"zoomout delete";
    }
    if(cw){
        delete cw,cw=nullptr;
        qDebug()<<"cw delete";
    }
    if(acw){
        delete acw,acw=nullptr;
        qDebug()<<"acw delete";
    }
}



void HeadImgWidget::openFile(const QString &filename)
{
    qDebug()<<"opened a file,named: "<<filename;
    QPixmap img;
    img.load(filename);
    view->setImage(img);
}





bool HeadImgWidget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched==zoomout){
        if(event->type()==QEvent::HoverEnter){
            transColor(imgOut,rgbOut[1]);//设置水蓝色
            zoomout->setIcon(QIcon(QPixmap::fromImage(imgOut)));
        }else if(event->type()==QEvent::HoverLeave) {
            transColor(imgOut,rgbOut[0]);//设置原色
            zoomout->setIcon(QIcon(QPixmap::fromImage(imgOut)));
        }
        return false;//继续穿透事件
    }else if(watched==zoomin){
        if(event->type()==QEvent::HoverEnter){
            transColor(imgIn,rgbIn[1]);
            zoomin->setIcon(QIcon(QPixmap::fromImage(imgIn)));//设置水蓝色
        }else if(event->type()==QEvent::HoverLeave) {
            transColor(imgIn,rgbIn[0]);
            zoomin->setIcon(QIcon(QPixmap::fromImage(imgIn)));//设置原色
        }
        return false;//继续穿透事件
    }else if(watched==cw){
        if(event->type()==QEvent::HoverEnter){
            transColor(imgCw,rgbCw[1]);
            cw->setIcon(QIcon(QPixmap::fromImage(imgCw)));//设置灰色
        }else if(event->type()==QEvent::HoverLeave) {
            transColor(imgCw,rgbCw[0]);
            cw->setIcon(QIcon(QPixmap::fromImage(imgCw)));//设置原色
        }else if(event->type()==QEvent::MouseButtonPress){
            transColor(imgCw,rgbCw[2]);
            cw->setIcon(QIcon(QPixmap::fromImage(imgCw)));//设置水蓝色
        }else if(event->type()==QEvent::MouseButtonPress){//鼠标放下恢复hover颜色
            transColor(imgCw,rgbCw[1]);
            cw->setIcon(QIcon(QPixmap::fromImage(imgCw)));//设置灰色
        }
        return false;//继续穿透事件
    }else if(watched==acw){
        if(event->type()==QEvent::HoverEnter){
            transColor(imgAcw,rgbAcw[1]);
            acw->setIcon(QIcon(QPixmap::fromImage(imgAcw)));//设置灰色
        }else if(event->type()==QEvent::HoverLeave) {
            transColor(imgAcw,rgbAcw[0]);
            acw->setIcon(QIcon(QPixmap::fromImage(imgAcw)));//设置原色
        }else if(event->type()==QEvent::MouseButtonPress){
            transColor(imgAcw,rgbAcw[2]);
            acw->setIcon(QIcon(QPixmap::fromImage(imgAcw)));//设置水蓝色
        }else if(event->type()==QEvent::MouseButtonPress){//鼠标放下恢复hover颜色
            transColor(imgAcw,rgbAcw[1]);
            acw->setIcon(QIcon(QPixmap::fromImage(imgAcw)));//设置灰色
        }
        return false;//继续穿透事件
    }
    return QWidget::eventFilter(watched,event);
}
//遍历对图片的像素赋值
void HeadImgWidget::transColor(QImage &img, const QHash<QPoint, QRgb> &colors)
{
    QHash<QPoint, QRgb >::const_iterator i =colors.constBegin();
    while (i != colors.constEnd()) {
        img.setPixelColor(i.key().x(),i.key().y(),QColor(i.value()));
        ++i;
    }
}


