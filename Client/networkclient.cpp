#include "networkclient.h"

#include <QJsonDocument>
#include <QTimer>

NetworkClient* NetworkClient::s_instance = nullptr;

/** 처음 호출 시 인스턴스를 생성하고, 이후에는 동일한 포인터를 반환한다. */
NetworkClient* NetworkClient::instance() {
    if (!s_instance)
        s_instance = new NetworkClient();
    return s_instance;
}

/** 소켓 시그널을 각 슬롯에 연결한다. */
NetworkClient::NetworkClient(QObject* parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);
    connect(m_socket, SIGNAL(connected()),                                      this, SLOT(onConnected()));
    connect(m_socket, SIGNAL(disconnected()),                                   this, SLOT(onDisconnected()));
    connect(m_socket, SIGNAL(readyRead()),                                      this, SLOT(onReadyRead()));
    connect(m_socket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)),      this, SLOT(onSocketError(QAbstractSocket::SocketError)));
}

/**
 * ConnectingState 중 중복 호출을 방지하기 위해
 * UnconnectedState일 때만 connectToHost를 호출한다.
 */
void NetworkClient::connectToServer(const QString& host, quint16 port) {
    m_host = host;
    m_port = port;
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
        return;
    m_socket->connectToHost(host, port);
}

/**
 * 소켓이 연결된 상태면 즉시 JSON을 '\n'으로 구분하여 전송하고,
 * 아니면 m_pendingRequest에 저장 후 재연결을 시도한다.
 * 재연결 성공(onConnected) 시 자동으로 재전송된다.
 */
void NetworkClient::sendRequest(const QJsonObject& request) {
    m_pendingRequest    = request;
    m_hasPendingRequest = true;

    if (isConnected()) {
        m_socket->write(QJsonDocument(request).toJson(QJsonDocument::Compact) + "\n");
        m_socket->flush();
        m_hasPendingRequest = false;   // 전송 완료 → 재연결 시 재전송 방지
        m_pendingRequest    = QJsonObject();
    } else {
        // 연결 안 돼 있으면 재연결 시도 → onConnected에서 자동 재전송
        connectToServer(m_host.isEmpty() ? "127.0.0.1" : m_host,
                        m_port == 0     ? 9999        : m_port);
    }
}

/** 소켓 상태가 ConnectedState인지 확인한다. */
bool NetworkClient::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

/**
 * 연결 성공 직후 대기 중인 요청이 있으면 즉시 재전송한다.
 * 재전송 완료 후 m_hasPendingRequest를 false로 초기화해 중복 전송을 방지한다.
 */
void NetworkClient::onConnected() {
    emit connectedToServer();
    if (m_hasPendingRequest) {
        m_socket->write(QJsonDocument(m_pendingRequest).toJson(QJsonDocument::Compact) + "\n");
        m_socket->flush();
        m_hasPendingRequest = false;
        m_pendingRequest    = QJsonObject();
    }
}

/**
 * 연결 해제 시 1.5초 후 reconnect()를 자동으로 호출한다.
 * m_host가 비어 있으면 최초 연결 전이므로 재연결을 시도하지 않는다.
 */
void NetworkClient::onDisconnected() {
    emit disconnectedFromServer();
    if (!m_host.isEmpty())
        QTimer::singleShot(1500, this, SLOT(reconnect()));
}

/** QTimer::singleShot에 의해 호출되는 재연결 함수. */
void NetworkClient::reconnect() {
    connectToServer(m_host, m_port);
}

/**
 * 소켓 수신 버퍼에서 '\n' 단위로 JSON 문자열을 추출해 파싱한다.
 * 파싱 성공 시 responseReceived를 발생시키고 대기 요청 플래그를 초기화한다.
 * 불완전한 데이터는 m_buffer에 유지했다가 다음 readyRead에서 이어서 처리한다.
 */
void NetworkClient::onReadyRead() {
    m_buffer += m_socket->readAll();
    while (m_buffer.contains('\n')) {
        int        idx  = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx);
        m_buffer.remove(0, idx + 1);
        if (line.trimmed().isEmpty()) continue;

        QJsonParseError err;
        QJsonDocument   doc = QJsonDocument::fromJson(line, &err);
        if (err.error == QJsonParseError::NoError && doc.isObject()) {
            m_hasPendingRequest = false;
            m_pendingRequest    = QJsonObject();
            emit responseReceived(doc.object());
        }
    }
}

/** 소켓 오류 발생 시 오류 문자열을 errorOccurred 시그널로 전달한다. */
void NetworkClient::onSocketError(QAbstractSocket::SocketError) {
    emit errorOccurred(m_socket->errorString());
}
