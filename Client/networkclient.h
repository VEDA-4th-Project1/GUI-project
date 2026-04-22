#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

/**
 * @class NetworkClient
 * @brief 서버와의 TCP 소켓 통신을 담당하는 싱글톤.
 *
 * 주요 동작:
 * - sendRequest()로 JSON 요청을 전송한다.
 * - 소켓이 끊겨 있으면 자동으로 재연결하고, 연결 완료 후 대기 중인 요청을 재전송한다.
 * - 서버 응답('\n' 구분)이 수신되면 responseReceived 시그널을 발생시킨다.
 * - 연결이 끊기면 1.5초 후 자동으로 reconnect()를 시도한다.
 */
class NetworkClient : public QObject {
    Q_OBJECT

public:
    /** 유일한 인스턴스를 반환한다. */
    static NetworkClient* instance();

    /**
     * @brief 서버에 TCP 연결을 시도한다.
     * @param host 서버 호스트 (기본값 "127.0.0.1")
     * @param port 서버 포트 (기본값 9999)
     */
    void connectToServer(const QString& host = "127.0.0.1", quint16 port = 9999);

    /**
     * @brief JSON 요청을 서버로 전송한다.
     *
     * 소켓이 연결된 상태면 즉시 전송하고,
     * 연결이 끊겨 있으면 m_pendingRequest에 저장 후 재연결을 시도한다.
     * 재연결 성공(onConnected) 시 자동으로 재전송된다.
     * @param request 전송할 JSON 객체
     */
    void sendRequest(const QJsonObject& request);

    /** 현재 소켓이 ConnectedState이면 true를 반환한다. */
    bool isConnected() const;

signals:
    /** 서버로부터 완전한 JSON 응답 1건을 수신했을 때 발생. */
    void responseReceived(const QJsonObject& response);

    /** TCP 연결이 성공했을 때 발생. */
    void connectedToServer();

    /** TCP 연결이 끊겼을 때 발생. */
    void disconnectedFromServer();

    /** 소켓 오류가 발생했을 때 발생 (오류 메시지 포함). */
    void errorOccurred(const QString& message);

private slots:
    /** 소켓에 수신 데이터가 준비됐을 때 호출된다. '\n' 단위로 JSON을 파싱한다. */
    void onReadyRead();

    /** 연결 성공 시 호출된다. 대기 중인 요청이 있으면 즉시 전송한다. */
    void onConnected();

    /** 연결 해제 시 호출된다. 1.5초 후 자동 재연결을 예약한다. */
    void onDisconnected();

    /** 소켓 오류 발생 시 호출된다. errorOccurred 시그널을 발생시킨다. */
    void onSocketError(QAbstractSocket::SocketError error);

    /** onDisconnected에서 QTimer::singleShot으로 예약되는 재연결 슬롯. */
    void reconnect();

private:
    explicit NetworkClient(QObject* parent = nullptr);

    static NetworkClient* s_instance;  ///< 싱글톤 인스턴스 포인터

    QTcpSocket*  m_socket;             ///< 실제 TCP 소켓
    QByteArray   m_buffer;             ///< 수신 데이터 누적 버퍼 ('\n' 전까지 유지)
    QString      m_host;               ///< 마지막으로 연결한 호스트 주소
    quint16      m_port = 0;           ///< 마지막으로 연결한 포트 번호

    QJsonObject  m_pendingRequest;         ///< 연결 끊김 시 보관해두는 대기 요청
    bool         m_hasPendingRequest = false;  ///< 대기 요청 존재 여부
};
