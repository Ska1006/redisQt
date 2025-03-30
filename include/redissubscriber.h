#ifndef REDISSUBSCRIBER_H
#define REDISSUBSCRIBER_H

#include <QObject>
#include "redisconnector.h"

class RedisSubscriber : public RedisConnector
{
    Q_OBJECT
public:
    RedisSubscriber(const QString &addr, quint16 port = 6379);

    QMap<QString, PushCallbackFuncPtr> m_subscribes;
    void subscribe(const QString &chanelName, PushCallbackFuncPtr callback);
    void unsubscribe(const QString &chanelName);

protected:
    void processReply(redisReply *reply);
    static void subscribeCallback(redisAsyncContext* c, void* reply, void* data);
};

#endif // REDISSUBSCRIBER_H
