#include "../include/redisdata.h"

RedisData::RedisData(const QString &addr, quint16 port)
    :RedisConnector(addr, port)
{

}

QVariant RedisData::getValue(const QString &key)
{
    if(m_connected){
        std::promise<QVariant> promise;
        std::future<QVariant> future = promise.get_future();
        redisAsyncCommand(m_context, RedisData::commandCallback, &promise,"GET %b", key.toUtf8().data(), key.size() );
        if(future.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready){
            return future.get();
        }else{
            QVariant("EXPIRED");
        }
    }
    return QVariant();
}

void RedisData::setValue(const QString &key, const QVariant &value)
{
    if(m_connected){
        QString str;
        if(value.type() == QVariant::String){
            str = value.toString();
        }else if(value.type() == QVariant::Int){
            str = QString::number(value.toInt());
        }else if(value.type() == QVariant::Double){
            str = QString::number(value.toDouble(), 'f', 16);
        }

        redisAsyncCommand( m_context, RedisData::commandCallback, nullptr, "SET %b %b", key.toUtf8().data(), key.size(), str.toUtf8().data(), str.size() );
    }
}

QHash<QString, QVariant> RedisData::getValues(const QVector<QString> &keys)
{
    QHash<QString, QVariant> res;
    for(const QString &key : keys){
        res.insert(key, getValue(key));
    }
    return res;
}

void RedisData::setValues(const QHash<QString, QVariant> &values)
{
    redisAsyncCommand( m_context, RedisData::commandCallback, nullptr, "MULTI" );
    auto it = values.constBegin();
    while(it != values.constEnd()){
        setValue(it.key(), it.value());
        ++it;
    }
    redisAsyncCommand( m_context, RedisData::commandCallback, nullptr, "EXEC" );
}


void RedisData::publish(const QString &chanelName, const QString &message)
{
    if(m_connected){
        redisAsyncCommand( m_context, commandCallback, nullptr,  "PUBLISH %b %b", chanelName.toUtf8().data(), chanelName.size(), message.toUtf8().data(), message.size() );
    }
}
