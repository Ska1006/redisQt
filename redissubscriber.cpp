#include "redissubscriber.h"




RedisSubscriber::RedisSubscriber(const QString &addr, quint16 port)
    :RedisConnector(addr, port)
{

}


void RedisSubscriber::subscribe(const QString &chanelName, PushCallbackFuncPtr callback)
{
    if(m_connected){
        m_subscribes.insert(chanelName, callback);
        redisAsyncCommand( m_context, subscribeCallback, this, "SUBSCRIBE %b", chanelName.toUtf8().data(), chanelName.size());
    }
}

void RedisSubscriber::unsubscribe(const QString &chanelName)
{
    if(m_connected){
        m_subscribes.take(chanelName);

        redisAsyncCommand( m_context, subscribeCallback, this, "UNSUBSCRIBE %b", chanelName.toUtf8().data(), chanelName.size());
    }
}

void RedisSubscriber::processReply(redisReply *reply)
{
    if(reply != nullptr){
        if(reply->type == REDIS_REPLY_ARRAY && reply->elements == 3){
            if(strcmp(reply->element[0]->str, "message") == 0){
                QString chanel = QString::fromUtf8(reply->element[1]->str, reply->element[1]->len);
                QString msg = QString::fromUtf8(reply->element[2]->str, reply->element[2]->len);
                PushCallbackFuncPtr func = m_subscribes.value(chanel, nullptr);
                if(func != nullptr){
                    func(chanel, msg);
                }
            }
        }
    }
}


void RedisSubscriber::subscribeCallback(redisAsyncContext* c, void* reply, void* data)
{
    Q_UNUSED(c)
    if(data != nullptr){
        redisReply *r = static_cast<redisReply*>(reply);
        RedisSubscriber *t = static_cast<RedisSubscriber*>(data);
        t->processReply(r);
    }
};
