#pragma once

#include <QObject>
#include <QTcpSocket>
#include <QJsonObject>

// TCP 소켓 통신 싱글톤.
// 서버와의 연결/송신/수신을 담당하며, 응답 수신 시 responseReceived 시그널을 발생시킨다.
class NetworkClient : public QObject {
    Q_OBJECT

public:
    static NetworkClient* instance();

    void connectToServer(const QString& host = "127.0.0.1", quint16 port = 9999);
    void sendRequest(const QJsonObject& request);
    bool isConnected() const;

signals:
    void responseReceived(const QJsonObject& response);
    void connectedToServer();
    void disconnectedFromServer();
    void errorOccurred(const QString& message);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onSocketError(QAbstractSocket::SocketError error);
    void reconnect();

private:
    explicit NetworkClient(QObject* parent = nullptr);

    static NetworkClient* s_instance;
    QTcpSocket*  m_socket;
    QByteArray   m_buffer;
    QString      m_host;
    quint16      m_port = 0;

    // 연결 끊김 시 재연결 후 자동 재전송을 위한 대기 요청 저장
    QJsonObject  m_pendingRequest;
    bool         m_hasPendingRequest = false;
};
