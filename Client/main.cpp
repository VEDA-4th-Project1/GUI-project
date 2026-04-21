#include "logindialog.h"
#include "networkclient.h"
#include "appstyle.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QFont>
#include <QPalette>
#include <QStyle>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ── 크로스플랫폼 일관성: Fusion 스타일로 Mac/Windows 동일하게 렌더링 ──
    a.setStyle(QStyleFactory::create("Fusion"));

    // ── 강제 라이트 팔레트 (시스템 다크모드 무시) ──
    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window,          QColor("#F2F4F6"));
    lightPalette.setColor(QPalette::WindowText,      QColor("#191F28"));
    lightPalette.setColor(QPalette::Base,            QColor("#FFFFFF"));
    lightPalette.setColor(QPalette::AlternateBase,   QColor("#F9FAFB"));
    lightPalette.setColor(QPalette::Text,            QColor("#191F28"));
    lightPalette.setColor(QPalette::Button,          QColor("#F2F4F6"));
    lightPalette.setColor(QPalette::ButtonText,      QColor("#191F28"));
    lightPalette.setColor(QPalette::Highlight,       QColor("#3182F6"));
    lightPalette.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    lightPalette.setColor(QPalette::PlaceholderText, QColor("#B0B8C1"));
    lightPalette.setColor(QPalette::ToolTipBase,     QColor("#191F28"));
    lightPalette.setColor(QPalette::ToolTipText,     QColor("#FFFFFF"));
    a.setPalette(lightPalette);

    // ── 전역 폰트 (크로스플랫폼 한글 폰트 스택) ──
    QFont appFont;
#if defined(Q_OS_MAC)
    appFont.setFamily("Apple SD Gothic Neo");
#elif defined(Q_OS_WIN)
    appFont.setFamily("Malgun Gothic");
#else
    appFont.setFamily("Segoe UI");
#endif
    appFont.setPointSize(10);
    a.setFont(appFont);

    // ── 전역 QSS (스크롤바, 툴팁 등) ──
    a.setStyleSheet(AppStyle::APP);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BankClient_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    NetworkClient::instance()->connectToServer("127.0.0.1", 9999);

    LoginDialog w;
    w.show();
    return QApplication::exec();
}
