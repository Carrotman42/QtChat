
#include <QtGui/QApplication>
#include <QHostAddress>
#include "chatserver.h"

/**
 * Main entry point into the application
 *
 * \param argc The length of the array in argv
 * \param argv Command line parameters to the executable
 *
 * \returns An exit code from QApplication::exec()
 */
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
	 
    chatserver server;
    if (!server.listen(QHostAddress::Any, 1123)) {
        qDebug() << "Could not start server!";
        return 1;
    }

    qDebug() << "Server started on port" << server.serverPort();

    return app.exec();

}
