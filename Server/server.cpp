#include "server.h"
#include "clienthandler.h"
#include "bank.h"
#include "protocol.h"

#include <QTcpSocket>
#include <QDebug>

Server::Server(Bank* bank, QObject* parent)
    : QTcpServer(parent), m_bank(bank)
{}

// 지정 포트에서 모든 네트워크 인터페이스(Any)로 리슨 시작
bool Server::startListening(quint16 port) {
    if (!listen(QHostAddress::Any, port)) {
        qCritical() << "[Server] 포트" << port << "에서 서버 시작 실패:" << errorString();
        return false;
    }
    qInfo() << "[Server] 포트" << port << "에서 대기 중";
    return true;
}

// 새 클라이언트 접속 시 Qt가 자동 호출
// 1. socketDescriptor로 QTcpSocket 생성
// 2. ClientHandler에 소켓과 Bank 포인터를 넘겨 이후 통신을 완전히 위임
// 3. ClientHandler가 소켓을 자식으로 소유하므로 연결 해제 시 자동 정리됨
void Server::incomingConnection(qintptr socketDescriptor) {
    auto* socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);

    auto* handler = new ClientHandler(socket, m_bank); // 소켓 소유권을 핸들러에 이전
    Q_UNUSED(handler)

    qInfo() << "[Server] 새 연결:" << socket->peerAddress().toString();
}
