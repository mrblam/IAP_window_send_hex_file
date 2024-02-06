#ifndef LOCALDB_H
#define LOCALDB_H

#include <QObject>

class LocalDB : public QObject
{
    Q_OBJECT
public:
    explicit LocalDB(QObject *parent = nullptr);

signals:
};

#endif // LOCALDB_H
