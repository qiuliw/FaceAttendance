#include "attendancewin.h"

#include <QApplication>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include "registerwin.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    // 数据库文件名
    db.setDatabaseName("server.db");
    // 打开数据库
    if (!db.open()) {
        qDebug() << db.lastError().text();
        return -1;
    }
    // 创建员工信息表
    QSqlQuery query;
    QString createEmployeeTable = "CREATE TABLE IF NOT EXISTS employee ("
                                  "employeeID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                  "name VARCHAR(50) NOT NULL, "
                                  "sex CHAR(1), "
                                  "birthday DATE, "
                                  "address VARCHAR(255), "
                                  "phone VARCHAR(20), "
                                  "faceID INTEGER UNIQUE, "
                                  "headfile TEXT)";
    if (!query.exec(createEmployeeTable)) {
        qDebug() << "创建员工信息表失败: " << query.lastError().text();
        return -1;
    }

    // 创建考勤信息表
    QString createAttendanceTable = "CREATE TABLE IF NOT EXISTS attendance ("
                                    "attendanceID INTEGER PRIMARY KEY AUTOINCREMENT, "
                                    "employeeID INTEGER NOT NULL, "
                                    "attTime DATETIME NOT NULL, "
                                    "notes VARCHAR(255), "
                                    "FOREIGN KEY (employeeID) REFERENCES employee(employeeID))";
    if (!query.exec(createAttendanceTable)) {
        qDebug() << "创建考勤信息表失败: " << query.lastError().text();
        return -1;
    }

    // 创建人脸信息表
    // AttendanceWin w;
    // w.show();

    RegisterWin r;
    r.show();

    return a.exec();

}