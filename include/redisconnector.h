#ifndef REDISCONNECTOR_H
#define REDISCONNECTOR_H

#include <QString>
#include <QThread>
#include <QVariant>
#include <hiredis/hiredis.h>
#include <hiredis/async.h>
#include <hiredis/adapters/qt.h>

class RedisConnector : public QThread
{
    Q_OBJECT
public:
    using PushCallbackFuncPtr = void (*)(const QString &, const QString &);

    RedisConnector(const QString &addr, quint16 port = 6379);

    bool isConnected() const;
signals:
    void connected();
    void disconnected();

protected:
    friend void subscribeCallback(redisAsyncContext* , void* , void* );
    QString m_addr{};
    quint16 m_port{};

    redisAsyncContext *m_context;
    bool m_connected{false};


    QSocketNotifier *m_readNotifier;
    QSocketNotifier *m_writeNotifier;

    static void commandCallback(redisAsyncContext* c, void* reply, void* data);

    void setupEventHandlers();


    bool init();

    void run();
};

#endif // REDISCONNECTOR_H
