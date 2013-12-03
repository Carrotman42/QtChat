
#include <QtTest/QtTest>
#include <QTcpSocket>

#include "chatserver.h"
#include <QThread>

class TestServer : public QObject {
    Q_OBJECT;
   
private slots:
    void testLogin();
    void testMsg();
    void testStatus();
    void testErrorBefore();
    void testErrorMalormed();
};

#define PORT 1123

#define MAKESERVER \
    QThread clientThread; \
    clientThread.start(); \
    chatserver server; \
    QVERIFY(server.listen(QHostAddress::Any, PORT)); \
    server.moveToThread(&clientThread);

#define ENDSERVER \
    clientThread.quit(); \
    clientThread.wait();

#define makeClient(name) \
    QTcpSocket name; \
    name.connectToHost("localhost", PORT); \
    QVERIFY(name.waitForConnected(5000));

#define LOGIN_BLOCK(sock) \
    LOGIN(sock); \
    readList(sock, 0);

#define LOGIN(sock) {\
    sock.write("LOGIN "#sock "\n");\
    sock.waitForBytesWritten(); }

#define MSG(sock, msg) {\
    sock.write("MSG " msg "\n"); \
    sock.waitForBytesWritten(); }

#define STATUS(sock, status) {\
    sock.write("STATUS " status "\n");\
    sock.waitForBytesWritten(); }

#define VERIFY_NORECV(sock) { \
    qDebug() << "Ensuring that "#sock " does not recv data"; \
    QVERIFY(!sock.waitForReadyRead(3000)); }

void recv(QTcpSocket& sock, QByteArray&dest) {
    if (!sock.canReadLine()) {
    QVERIFY(sock.waitForReadyRead(5000));
    }
    
    char buf[4096];

    int lenRead;
    do {
        lenRead = sock.readLine(buf, sizeof buf);
    } while (lenRead == 0);
    QVERIFY(lenRead != -1);

    // Take off the newline characters
    lenRead--;
    while (buf[lenRead] == '\n' || buf[lenRead] == '\r') {
        buf[lenRead--] = 0;
    }

    dest = QByteArray(buf, lenRead + 1);
}

#define LIST QMap<QString, QString>

void readList(QTcpSocket& sock, LIST* statuses)
{
    QByteArray list;
    recv(sock, list);
    QCOMPARE(list.left(5), QByteArray("LIST "));
    list = list.mid(5);

    if (!statuses) return;
    LIST& dest = *statuses;
    dest.clear();

    QList<QByteArray> split = list.split('\0');
    for (int i = 0; i < split.size(); i+=2) {
        dest[split[i]] = split[i+1];
    }
}

void readUntilMsg(QTcpSocket&sock, QString&from, QString&msg)
{
    QByteArray line;
    
    do {
        recv(sock, line);
        qDebug() << "Got line:" << line;
    } while (line.left(4) != QByteArray("MSG "));

    line = line.mid(4);
    int sep = line.indexOf('\0');
    from = line.left(sep);
    qDebug() << "Sizeof from=" << from.length() << ":" << from;
    msg = line.mid(sep + 1);
}

void TestServer::testLogin()
{
    MAKESERVER;

    makeClient(s1);

    makeClient(s2);
    LOGIN(s2);
    LIST statuses;
    readList(s2, &statuses);
    QCOMPARE(statuses.size(), 1);
    QVERIFY(statuses.contains("s2"));
    VERIFY_NORECV(s1);

    LOGIN(s1);
    qDebug() << "Logged in s1";
    readList(s2, &statuses);
    QCOMPARE(statuses.size(), 2);
    QVERIFY(statuses.contains("s1"));
    QVERIFY(statuses.contains("s2"));
    readList(s1, &statuses);
    QCOMPARE(statuses.size(), 2);
    QVERIFY(statuses.contains("s1"));
    QVERIFY(statuses.contains("s2"));

    ENDSERVER;
}

void TestServer::testMsg()
{
    MAKESERVER;

    makeClient(s1);
    LOGIN_BLOCK(s1);
    makeClient(s2);

    QString from, msg;
    MSG(s1, "Should be ignored!");
    readUntilMsg(s1, from, msg);
    VERIFY_NORECV(s2);
    
    LOGIN_BLOCK(s2);

    MSG(s1, "From s1!");

    readUntilMsg(s2, from, msg);
    QCOMPARE(from, QString("s1"));
    QCOMPARE(msg, QString("From s1!"));
    
    readUntilMsg(s1, from, msg);
    QCOMPARE(from, QString("s1"));
    QCOMPARE(msg, QString("From s1!"));
    
    MSG(s2, "From s2!");
    readUntilMsg(s2, from, msg);
    QCOMPARE(from, QString("s2"));
    QCOMPARE(msg, QString("From s2!"));
    
    readUntilMsg(s1, from, msg);
    QCOMPARE(from, QString("s2"));
    QCOMPARE(msg, QString("From s2!"));

    ENDSERVER;
}

void TestServer::testStatus()
{
}
void TestServer::testErrorBefore()
{
}
void TestServer::testErrorMalormed()
{
}

QTEST_MAIN(TestServer)
#include "testserver.moc"





