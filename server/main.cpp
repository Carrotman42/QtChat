
#include <QtGui/QApplication>
#include "dialog.h"

#include "gatedb.h"
#include <QHostAddress>

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
	 
    gateserver server;
    if (!server.listen(QHostAddress::Any, 1123)) {
        lbl->setText("Could not start server!");
        qDebug() << "Could not start server!";
        return;
    }

    qDebug() << "Server started on port" << server.serverPort();

    return app.exec();

}
