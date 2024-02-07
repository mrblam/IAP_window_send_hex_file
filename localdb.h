#ifndef LOCALDB_H
#define LOCALDB_H

#include <QObject>
#include <QSqlDatabase>

class LocalDB : public QObject
{
    Q_OBJECT
public:
    explicit LocalDB(QObject *parent = nullptr);
    LocalDB(const QString path);
    bool insertToDb(const QString name);
signals:
private:
    QSqlDatabase m_db;
};

#endif // LOCALDB_H
