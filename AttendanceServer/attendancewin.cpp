#include "attendancewin.h"
#include "./ui_attendancewin.h"
#include <qpixmap.h>

AttendanceWin::AttendanceWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::AttendanceWin)
{
    ui->setupUi(this);

    /*
        = 捕捉 this 指针
    */

    // qtcpserver当有客户端链接时，会触发newConnection信号
    connect(&server_, &QTcpServer::newConnection, [this](){ // 接受客户端连接
        QTcpSocket* clientSocket = server_.nextPendingConnection(); // 取出一个客户端连接
        // 为新客户端创建资源结构体
        this->clientResources_[clientSocket] = std::make_shared<ClientResource>();
        
        // 当客户端有数据到达会触发readyRead信号
        connect(clientSocket, &QTcpSocket::readyRead, [this, clientSocket](){  // 接收数据
           
            // 读取所有数据
            QDataStream stream(clientSocket);
            stream.setVersion(QDataStream::Qt_5_14);
            
            // 获取客户端资源
            auto& resource = this->clientResources_[clientSocket];
            
            // 获取数据大小
            if(resource->bsize == 0){
                if(clientSocket->bytesAvailable() < (int)sizeof(quint64)){
                    return;
                }
                stream >> resource->bsize;
            }
            
            // 获取数据
            if(clientSocket->bytesAvailable() < resource->bsize){ // 数据不完整，等待
                return;
            }
            stream >> resource->data;

            if(resource->data.size() == 0) // 数据为空
                return;
            // 数据完整
            resource->bsize = 0; 
            QPixmap mmp;
            mmp.loadFromData(resource->data, "jpg");
            // 修复图像缩放问题：需要使用scaled方法的返回值
            QPixmap scaledMmp = mmp.scaled(ui->pictureLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            ui->pictureLabel->setPixmap(scaledMmp);
        });
        
        // 客户端断开连接时清理资源
        connect(clientSocket, &QTcpSocket::disconnected, [this, clientSocket](){
            this->clientResources_.remove(clientSocket);
            clientSocket->deleteLater();
        });

    });
    server_.listen(QHostAddress::Any, 9999); // 监听，启动服务器
}

AttendanceWin::~AttendanceWin()
{
    delete ui;
}