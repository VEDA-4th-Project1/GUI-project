#pragma once

// 개별 클라이언트 연결을 처리하는 클래스.
// Server가 새 연결을 수락할 때마다 인스턴스가 하나씩 생성된다.
// QTcpSocket의 readyRead 시그널을 받아 데이터를 읽고,
// JSON으로 파싱한 뒤 Bank의 핸들러로 위임하고 응답을 소켓에 쓴다.
//
// TCP 스트림 경계 처리:
//   TCP는 데이터를 연속 스트림으로 전달하므로 메시지 경계가 없다.
//   이 클래스는 '\n' 구분자 기준으로 한 줄씩 잘라 JSON 메시지를 분리한다.
//   m_buffer에 수신 데이터를 누적하다가 '\n'이 나오면 파싱한다.

#include <QObject>
#include <QByteArray>
#include <QJsonObject>

class QTcpSocket;
class Bank;

class ClientHandler : public QObject {
    Q_OBJECT

public:
    // socket: 이 핸들러가 담당할 클라이언트 소켓 (소유권 이전, setParent(this) 호출)
    // bank: 비즈니스 로직 참조 (소유권 없음)
    explicit ClientHandler(QTcpSocket* socket, Bank* bank, QObject* parent = nullptr);

private slots:
    // 소켓에 읽을 데이터가 생겼을 때 Qt가 자동 호출
    void onReadyRead();
    // 클라이언트가 연결을 끊었을 때 호출: deleteLater()로 자신을 정리
    void onDisconnected();

private:
    // 파싱된 JSON 요청을 type 필드에 따라 Bank의 적절한 핸들러로 분기
    void processMessage(const QJsonObject& request);
    // Bank에서 받은 JSON 응답을 Compact JSON + '\n'으로 소켓에 전송
    void sendResponse(const QJsonObject& response);

    QTcpSocket* m_socket; // 담당 클라이언트 소켓 (자식 객체)
    Bank*       m_bank;   // 비즈니스 로직 참조
    QByteArray  m_buffer; // TCP 스트림 수신 버퍼 (줄바꿈 기준으로 메시지 분리)
};
