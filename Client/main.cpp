#include "logindialog.h"
#include "networkclient.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BankClient_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // 앱 시작 시 서버에 TCP 연결
    NetworkClient::instance()->connectToServer("127.0.0.1", 9999);

    LoginDialog w;
    w.show();
    return QApplication::exec();
}
