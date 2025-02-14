#include <QCoreApplication>
#include "fibonaccirpcclient.h"
#include <QRandomGenerator>

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    FibonacciRpcClient client;
    if (!client.connectToServer())
        return EXIT_FAILURE;
    QRandomGenerator generator;
    client.call(generator.global()->bounded(1, 40));
    return app.exec();
}
