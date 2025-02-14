#include <QCoreApplication>
#include <QTimer>
#include <QDebug>

#include "qamqpclient.h"
#include "qamqpexchange.h"
#include "qamqpqueue.h"

class LogReceiver : public QObject
{
    Q_OBJECT
public:
    LogReceiver(QObject *parent = 0) : QObject(parent) {
        m_client.setUsername("admin");
        m_client.setPassword("11111111");
        m_client.setAutoReconnect(true, 5000);
    }

public Q_SLOTS:
    void start() {
        connect(&m_client, SIGNAL(connected()), this, SLOT(clientConnected()));
        connect(&m_client, SIGNAL(socketErrorOccurred(QAbstractSocket::SocketError)), this, SLOT(errorReceived(QAbstractSocket::SocketError)));
        m_client.connectToHost();
    }

private Q_SLOTS:
    void clientConnected() {
        QAmqpExchange *exchange = m_client.createExchange("logs");
        connect(exchange, SIGNAL(declared()), this, SLOT(exchangeDeclared()));
        exchange->declare(QAmqpExchange::FanOut);
    }

    void exchangeDeclared() {
        QAmqpQueue *temporaryQueue = m_client.createQueue();
        connect(temporaryQueue, SIGNAL(declared()), this, SLOT(queueDeclared()));
        connect(temporaryQueue, SIGNAL(bound()), this, SLOT(queueBound()));
        connect(temporaryQueue, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
        temporaryQueue->declare(QAmqpQueue::Exclusive);
    }

    void queueDeclared() {
        QAmqpQueue *temporaryQueue = qobject_cast<QAmqpQueue*>(sender());
        if (!temporaryQueue)
            return;

        temporaryQueue->bind("logs", temporaryQueue->name());
    }

    void queueBound() {
        QAmqpQueue *temporaryQueue = qobject_cast<QAmqpQueue*>(sender());
        if (!temporaryQueue)
            return;

        qDebug() << " [*] Waiting for logs. To exit press CTRL+C";
        temporaryQueue->consume(QAmqpQueue::coNoAck);
    }

    void messageReceived() {
        QAmqpQueue *temporaryQueue = qobject_cast<QAmqpQueue*>(sender());
        if (!temporaryQueue)
            return;

        QAmqpMessage message = temporaryQueue->dequeue();
        qDebug() << " [x] " << message.payload();
    }

    void errorReceived(QAbstractSocket::SocketError error) {
        QString errorString;
        switch (error) {
        case QAbstractSocket::SocketError::HostNotFoundError:
            errorString = "HostNotFoundError";
            break;
        case QAbstractSocket::SocketError::SocketAccessError:
            errorString = "SocketAccessError";
            break;
        case QAbstractSocket::SocketError::SocketTimeoutError:
            errorString = "SocketTimeoutError";
            break;
        case QAbstractSocket::SocketError::ConnectionRefusedError:
            errorString = "ConnectionRefusedError";
            break;
        case QAbstractSocket::SocketError::RemoteHostClosedError:
            errorString = "RemoteHostClosedError";
            break;
        case QAbstractSocket::SocketError::SocketResourceError:
            errorString = "SocketResourceError";
            break;
        case QAbstractSocket::SocketError::DatagramTooLargeError:
            errorString = "DatagramTooLargeError";
            break;
        case QAbstractSocket::SocketError::NetworkError:
            errorString = "NetworkError";
            break;
        case QAbstractSocket::SocketError::AddressInUseError:
            errorString = "AddressInUseError";
            break;
        case QAbstractSocket::SocketError::SocketAddressNotAvailableError:
            errorString = "SocketAddressNotAvailableError";
            break;
        case QAbstractSocket::SocketError::UnsupportedSocketOperationError:
            errorString = "UnsupportedSocketOperationError";
            break;
        case QAbstractSocket::SocketError::UnfinishedSocketOperationError:
            errorString = "UnfinishedSocketOperationError";
            break;
        case QAbstractSocket::SocketError::ProxyAuthenticationRequiredError:
            errorString = "ProxyAuthenticationRequiredError";
            break;
        case QAbstractSocket::SocketError::SslHandshakeFailedError:
            errorString = "SslHandshakeFailedError";
            break;
        case QAbstractSocket::SocketError::ProxyConnectionRefusedError:
            errorString = "ProxyConnectionRefusedError";
            break;
        case QAbstractSocket::SocketError::ProxyConnectionClosedError:
            errorString = "ProxyConnectionClosedError";
            break;
        case QAbstractSocket::SocketError::ProxyConnectionTimeoutError:
            errorString = "ProxyConnectionTimeoutError";
            break;
        case QAbstractSocket::SocketError::ProxyNotFoundError:
            errorString = "ProxyNotFoundError";
            break;
        case QAbstractSocket::SocketError::ProxyProtocolError:
            errorString = "ProxyProtocolError";
            break;
        case QAbstractSocket::SocketError::OperationError:
            errorString = "OperationError";
            break;
        case QAbstractSocket::SocketError::SslInternalError:
            errorString = "SslInternalError";
            break;
        case QAbstractSocket::SocketError::SslInvalidUserDataError:
            errorString = "SslInvalidUserDataError";
            break;
        case QAbstractSocket::SocketError::TemporaryError:
            errorString = "TemporaryError";
            break;
        case QAbstractSocket::SocketError::UnknownSocketError:
            errorString = "UnknownSocketError";
            break;
        }
        qDebug() << "Error occured: " + errorString;
    }

private:
    QAmqpClient m_client;

};

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    LogReceiver logReceiver;
    logReceiver.start();
    return app.exec();
}

#include "main.moc"
