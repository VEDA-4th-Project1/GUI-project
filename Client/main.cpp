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

/**
 * 애플리케이션 진입점.
 *
 * 초기화 순서:
 * 1. Fusion 스타일 적용 — Mac/Windows에서 렌더링을 통일한다.
 * 2. 라이트 팔레트 강제 적용 — 시스템 다크모드를 무시한다.
 * 3. 크로스플랫폼 한글 폰트 설정 (Mac: Apple SD Gothic Neo, Windows: Malgun Gothic).
 * 4. 전역 QSS(AppStyle::APP) 적용 — 스크롤바·툴팁 등 공통 스타일.
 * 5. 언어 설정에 맞는 번역 파일 로드.
 * 6. NetworkClient로 서버(127.0.0.1:9999)에 연결.
 * 7. LoginDialog 표시 후 이벤트 루프 시작.
 */
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

    // ── 전역 폰트 (플랫폼별 한글 폰트 우선 적용) ──
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

    // ── 전역 QSS (스크롤바, 툴팁 등 공통 스타일) ──
    a.setStyleSheet(AppStyle::APP);

    // ── 번역 파일 로드 (시스템 언어 기준, 없으면 기본 언어 사용) ──
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "BankClient_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    // ── 서버 연결 (비동기 — 연결 실패 시 NetworkClient가 자동 재시도) ──
    NetworkClient::instance()->connectToServer("127.0.0.1", 9999);

    // ── 로그인 다이얼로그 표시 및 이벤트 루프 시작 ──
    LoginDialog w;
    w.show();
    return QApplication::exec();
}
