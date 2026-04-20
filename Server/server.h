#pragma once

// TCP 서버 클래스. QTcpServer를 상속하여 포트 리슨과 새 연결 수락을 담당한다.
// 새 클라이언트가 접속하면 incomingConnection()이 호출되고,
// 해당 소켓을 ClientHandler에 넘겨 이후 통신을 위임한다.

#include <QTcpServer>

class Bank;

class Server : public QTcpServer {
    Q_OBJECT

public:
    // bank: 모든 ClientHandler가 공유하는 비즈니스 로직 객체
    explicit Server(Bank* bank, QObject* parent = nullptr);
    // 지정 포트에서 리슨 시작; 실패 시 false 반환
    bool startListening(quint16 port);

protected:
    // QTcpServer 가상 함수 오버라이드: 새 연결이 들어올 때마다 자동 호출
    // socketDescriptor로 QTcpSocket을 생성하고 ClientHandler를 만들어 연결 처리를 위임
    void incomingConnection(qintptr socketDescriptor) override;

private:
    Bank* m_bank; // 모든 핸들러가 참조하는 공유 비즈니스 로직 (소유권 없음)
};
