#ifndef ATTENDANCEWIN_H
#define ATTENDANCEWIN_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <qtcpsocket.h>
#include <QMap>
#include <memory>

// 前置声明
QT_BEGIN_NAMESPACE
namespace Ui {
class AttendanceWin;
}
QT_END_NAMESPACE

// 客户端资源结构体
struct ClientResource {
    quint64 bsize = 0;
    QByteArray data;
    // 可以添加更多客户端相关的资源字段
};

class AttendanceWin : public QMainWindow
{
    Q_OBJECT

public:
    AttendanceWin(QWidget *parent = nullptr);
    ~AttendanceWin();

private:
    Ui::AttendanceWin *ui;

    QTcpServer server_;
    QTcpSocket *socket_ = nullptr;
    
    // 为每个客户端连接维护独立的资源结构体
    QMap<QTcpSocket*, std::shared_ptr<ClientResource>> clientResources_;

};
#endif // ATTENDANCEWIN_H