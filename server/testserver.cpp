
#include <QtTest/QtTest>
#include <QTcpSocket>

#include "chatserver.h"
#include <QThread>

class TestServer : public QObject {
    Q_OBJECT;
   
private slots:
    /**
     * Tests the LOGIN command
     */
    void testLogin();
    /**
     * Tests the MSG command
     */
    void testMsg();
    /**
     * Tests the STATUS command
     */
    void testStatus();
    /**
     * Tests to make sure an ERROR code is sent if you try to do things before loggin in.
     */
    void testErrorBefore();
    /**
     * Tests to make sure ERROR codes are sent when bad commands are sent.
     */
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

#define SEND(sock, msg) {\
    sock.write(msg "\n"); \
    sock.waitForBytesWritten();}

#define LOGIN(sock) SEND(sock, "LOGIN "#sock)
#define MSG(sock, msg) SEND(sock, "MSG " msg)
#define STATUS(sock, status) SEND(sock, "STATUS " status);

#define VERIFY_NORECV(sock) { \
    qDebug() << "Ensuring that "#sock " does not recv data"; \
    QVERIFY(!sock.waitForReadyRead(300)); }

void recv(QTcpSocket& sock, QByteArray&dest) {
    int tries = 0;
    while (sock.bytesAvailable() == 0) {
        sock.waitForReadyRead(1000);
        tries++;
        QVERIFY(tries < 5);
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
    MAKESERVER;

    makeClient(s1);
    LOGIN_BLOCK(s1);
    makeClient(s2);

    LIST st;
    STATUS(s1, "First status");
    readList(s1, &st);
    VERIFY_NORECV(s2);
    
    LOGIN(s2);
    readList(s1, 0);
    readList(s2, &st);
    QCOMPARE(st.size(), 2);
    QVERIFY(st.contains("s1"));
    QVERIFY(st.contains("s2"));
    QCOMPARE(st["s1"], QString("First status"));
    
    STATUS(s1, "New status!");
    readList(s2, &st);
    QCOMPARE(st.size(), 2);
    QVERIFY(st.contains("s1"));
    QVERIFY(st.contains("s2"));
    QCOMPARE(st["s1"], QString("New status!"));
    readList(s1, &st);
    QCOMPARE(st.size(), 2);
    QVERIFY(st.contains("s1"));
    QVERIFY(st.contains("s2"));
    QCOMPARE(st["s1"], QString("New status!"));

    ENDSERVER;
}


#define VERIFY_ERROR(sock) {\
        QByteArray list; \
        recv(sock, list); \
        QCOMPARE(list.left(6), QByteArray("ERROR ")); \
    }

void TestServer::testErrorBefore()
{
    MAKESERVER;
    
    makeClient(s1);
    MSG(s1, "Hello!");
    VERIFY_ERROR(s1);
    STATUS(s1, "Try again!");
    VERIFY_ERROR(s1);

    ENDSERVER;
}
void TestServer::testErrorMalormed()
{
    MAKESERVER;
    
    makeClient(s1);
    SEND(s1, "LOGIN");
    VERIFY_ERROR(s1);

    LOGIN_BLOCK(s1);

    SEND(s1, "supdawg");
    VERIFY_ERROR(s1);
    SEND(s1, "MSG");
    VERIFY_ERROR(s1);
    SEND(s1, "STATUS");
    VERIFY_ERROR(s1);

    ENDSERVER;
}

QTEST_MAIN(TestServer)
#include "testserver.moc"





