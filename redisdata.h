#ifndef REDISDATA_H
#define REDISDATA_H

#include <QObject>
#include "redisconnector.h"

class RedisData : public RedisConnector
{
    Q_OBJECT
public:
    RedisData(const QString &addr, quint16 port = 6379);
    QVariant getValue(const QString &key);
    void setValue(const QString &key, const QVariant &value);

    QHash<QString, QVariant> getValues(const QVector<QString> &keys);
    void setValues(const QHash<QString, QVariant> &values);

    void publish(const QString &chanelName, const QString &message);


};

#endif // REDISDATA_H
