#include "networkclient.h"

#include <QJsonDocument>
#include <QTimer>

NetworkClient* NetworkClient::s_instance = nullptr;

NetworkClient* NetworkClient::instance() {
    if (!s_instance)
        s_instance = new NetworkClient();
    return s_instance;
}

NetworkClient::NetworkClient(QObject* parent) : QObject(parent) {
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected,          this, &NetworkClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected,       this, &NetworkClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead,          this, &NetworkClient::onReadyRead);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &NetworkClient::onSocketError);
}

void NetworkClient::connectToServer(const QString& host, quint16 port) {
    m_host = host;
    m_port = port;
    // ConnectingState 중에 중복 호출 방지
    if (m_socket->state() != QAbstractSocket::UnconnectedState)
        return;
    m_socket->connectToHost(host, port);
}

// 요청을 저장해두고, 연결 중이면 즉시 전송 / 아니면 연결 후 자동 재전송
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

bool NetworkClient::isConnected() const {
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

// 연결 성공 시: 대기 중인 요청이 있으면 즉시 전송
void NetworkClient::onConnected() {
    emit connectedToServer();
    if (m_hasPendingRequest) {
        m_socket->write(QJsonDocument(m_pendingRequest).toJson(QJsonDocument::Compact) + "\n");
        m_socket->flush();
        m_hasPendingRequest = false;   // 재전송 완료 → 중복 방지
        m_pendingRequest    = QJsonObject();
    }
}

// 연결 끊김 시: 1.5초 후 자동 재연결 (대기 요청 있으면 재연결 후 자동 재전송)
void NetworkClient::onDisconnected() {
    emit disconnectedFromServer();
    if (!m_host.isEmpty())
        QTimer::singleShot(1500, this, [this]{ connectToServer(m_host, m_port); });
}

// TCP 스트림 수신: '\n' 구분자로 메시지 분리 후 responseReceived 발생
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
            // 응답 수신 완료 → 대기 요청 클리어
            m_hasPendingRequest = false;
            m_pendingRequest    = QJsonObject();
            emit responseReceived(doc.object());
        }
    }
}

void NetworkClient::onSocketError(QAbstractSocket::SocketError) {
    emit errorOccurred(m_socket->errorString());
}
