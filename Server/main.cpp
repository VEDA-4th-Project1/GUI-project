#include "server.h"
#include "bank.h"
#include "protocol.h"

#include <QCoreApplication>
#include <QDir>
#include <QDebug>

// 진입점: QCoreApplication으로 이벤트 루프를 시작하는 콘솔 서버
int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv); // GUI 없는 Qt 앱 (콘솔 서버)

    // 실행파일 위치 기준 data/ 폴더를 데이터 저장 경로로 사용
    QString dataDir = QCoreApplication::applicationDirPath() + "/data";
    QDir().mkpath(dataDir); // 폴더 없으면 자동 생성

    Bank   bank(dataDir);   // 비즈니스 로직 + Repository 초기화 (JSON 파일 로드)
    Server server(&bank);   // TCP 서버 (Bank 포인터를 모든 핸들러와 공유)

    // 포트: 커맨드라인 인자로 지정 가능, 없으면 기본값(9999) 사용
    // 실행 예: ./BankServer 8080
    quint16 port = Protocol::PORT;
    if (argc >= 2) {
        bool ok = false;
        quint16 p = static_cast<quint16>(QString(argv[1]).toUInt(&ok));
        if (ok) port = p;
    }

    if (!server.startListening(port))
        return 1; // 포트 바인딩 실패 시 종료

    qInfo() << "[Main] Bank 서버 시작. 데이터 디렉토리:" << dataDir;
    return app.exec(); // 이벤트 루프 진입 (Ctrl+C로 종료)
}
