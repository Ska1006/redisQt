#include "../include/redisconnector.h"
#include <QDebug>
#include <QAbstractEventDispatcher>

QVariant getReplyValue(redisReply *reply)
{
    if(reply != nullptr){
        switch (reply->type) {
        case REDIS_REPLY_INTEGER:
            return QVariant( reply->integer );
        case REDIS_REPLY_NIL:
        case REDIS_REPLY_STATUS:
            return QVariant();
        case REDIS_REPLY_BIGNUM :
            switch(reply->len){
            case 1:
                qint8 val;
                memcpy(&val, reply->str, sizeof val);
                return QVariant(val);
            case 2:{
                qint16 val;
                memcpy(&val, reply->str, sizeof val);
                return QVariant(val);
            }
            case 4:{
                qint32 val;
                memcpy(&val, reply->str, sizeof val);
                return QVariant(val);
            }
            case 8:{
                qint64 val;
                memcpy(&val, reply->str, sizeof val);
                return QVariant(val);
            }
            }
        case REDIS_REPLY_ERROR:
        case REDIS_REPLY_STRING:
        case REDIS_REPLY_VERB:
            return QVariant( QString::fromUtf8( reply->str, reply->len ) );
        case REDIS_REPLY_DOUBLE:
            return QVariant( reply->dval );
        case REDIS_REPLY_BOOL:
            return QVariant( bool(reply->integer) );
        case REDIS_REPLY_ATTR:
        case REDIS_REPLY_PUSH:
        case REDIS_REPLY_MAP:
        case REDIS_REPLY_SET:
        case REDIS_REPLY_ARRAY:
            QVector<QVariant> vect(reply->elements);
            for(size_t i = 0; i < reply->elements; i++){
                vect[i] = getReplyValue(reply->element[i]);
            }
            return QVariant::fromValue(vect);
            break;
        }
    }
    return QVariant();
}


void RedisConnector::commandCallback(redisAsyncContext* c, void* reply, void* data){
    Q_UNUSED(c)
    if(data != nullptr){
        redisReply *r = static_cast<redisReply*>(reply);
        std::promise<QVariant> *promise = static_cast<std::promise<QVariant>*>(data);
        promise->set_value(getReplyValue(r));
    }
};




RedisConnector::RedisConnector(const QString &addr, quint16 port)
    :   QThread()
    , m_addr(addr)
    , m_port(port)
{
    connect(this, &QThread::started, this, &RedisConnector::init);
    this->start();
    this->moveToThread(this);
}



bool RedisConnector::connected() const
{
    return m_connected;
}

void RedisConnector::setupEventHandlers() {
    int fd = m_context->c.fd;
    m_readNotifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    m_writeNotifier = new QSocketNotifier(fd, QSocketNotifier::Write, this);

    connect(m_readNotifier, &QSocketNotifier::activated, this, [this]() {
        redisAsyncHandleRead(m_context);
    });

    connect(m_writeNotifier, &QSocketNotifier::activated, this, [this]() {
        redisAsyncHandleWrite(m_context);
    });
}

bool RedisConnector::init()
{

    auto onConnect = [](const redisAsyncContext * context, int status){
        RedisConnector *t = static_cast<RedisConnector*>(context->data);
        t->m_connected = true;
        qDebug() << "Connected";
    };
    auto onDisconnect = [](const redisAsyncContext * context, int status){
        RedisConnector *t = static_cast<RedisConnector*>(context->data);
        t->m_connected = false;
        qDebug() << "Disconnected";
    };
    redisOptions options = {0};
    REDIS_OPTIONS_SET_TCP(&options, m_addr.toLatin1().data(), m_port);
    m_context = redisAsyncConnectWithOptions(&options);
    if(m_context){
        if(m_context->err){
            qDebug() << QString("Redis connect ERR[%2]:%1").arg(m_context->errstr).arg(m_context->err);
        }else{
            m_context->data = this;
            redisAsyncSetConnectCallback(m_context, onConnect);
            redisAsyncSetDisconnectCallback(m_context, onDisconnect);

            setupEventHandlers();
            return true;
        }
    }
    return false;
}


void RedisConnector::run()
{
    init();
    QThread::run();
}

