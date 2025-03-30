#include <QCoreApplication>
#include "../../include/redisdata.h"
#include "qdebug.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Set up code that uses the Qt event loop here.
    // Call a.quit() or a.exit() to quit the application.
    // A not very useful example would be including
    // #include <QTimer>
    // near the top of the file and calling
    // QTimer::singleShot(5000, &a, &QCoreApplication::quit);
    // which quits the application after 5 seconds.

    // If you do not need a running Qt event loop, remove the call
    // to a.exec() or use the Non-Qt Plain C++ Application template.


    RedisData data("localhost");
    QObject o;
    QObject::connect(&data, &RedisData::connected, &o, [&data]{
        QVector<QString> keys;
        keys << "Foo" << "Bar" << "FooBar";
        auto res = data.getValues(keys);
        qDebug() << res;

        QHash<QString, QVariant> val;
        val.insert("Foo", "Bar");
        val.insert("Bar", "Foo");
        val.insert("FooBar", "FooFoo");
        data.setValues(val);

        res = data.getValues(keys);
        qDebug() << res;

    });



    return a.exec();
}
