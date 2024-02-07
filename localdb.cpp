#include "localdb.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDateTime>

LocalDB::LocalDB(QObject *parent)
    : QObject{parent}
{}
LocalDB::LocalDB(const QString path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    bool dbExists = QFile::exists(path);
    if(!dbExists){
        if (!m_db.open())
        {
            qDebug() << "Cannot create new database. Error:" << m_db.lastError().text();
        }
        else
        {
            QSqlQuery query;
            query.exec("create table history ([Thời gian] DATETIME,[Trạng thái nạp] TEXT)");
            qDebug() << "Database: connection ok";
        }
    }else{
        if (!m_db.open()) {
            qDebug() << "Cannot open exist database. Error:" << m_db.lastError().text();
        }else{
            qDebug() << QObject::tr("Database is open!");
        }
    }

}
bool LocalDB::insertToDb(const QString status)
{
    bool success = false;
    QDateTime time = QDateTime::currentDateTime();
    QSqlQuery query;
    query.prepare("INSERT INTO history VALUES (:time,:name)");
    query.bindValue(":time", time);
    query.bindValue(":name", status);
    if(query.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "addPerson error:"
                 << query.lastError();
    }

    return success;
}
