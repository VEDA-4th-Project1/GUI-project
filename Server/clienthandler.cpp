#include "clienthandler.h"
#include "bank.h"

#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>

// 생성자: 소켓을 자식 객체로 설정하고 시그널-슬롯 연결
// socket을 setParent(this)로 등록해두면 이 핸들러가 삭제될 때 소켓도 자동 삭제됨
ClientHandler::ClientHandler(QTcpSocket* socket, Bank* bank, QObject* parent)
    : QObject(parent), m_socket(socket), m_bank(bank)
{
    m_socket->setParent(this); // 소켓 소유권 이전 (수명 연동)
    connect(m_socket, &QTcpSocket::readyRead,    this, &ClientHandler::onReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);
}

// 소켓에 데이터가 도착할 때 호출
// TCP 스트림이므로 한 번에 여러 메시지가 오거나 메시지가 쪼개서 올 수 있음
// m_buffer에 데이터를 누적하다가 '\n'이 나올 때마다 한 줄씩 꺼내 파싱
void ClientHandler::onReadyRead() {
    m_buffer += m_socket->readAll(); // 도착한 데이터를 버퍼에 추가

    // '\n'이 있는 한 계속 메시지를 처리 (한 번에 여러 메시지가 왔을 수도 있음)
    while (m_buffer.contains('\n')) {
        int        idx  = m_buffer.indexOf('\n');
        QByteArray line = m_buffer.left(idx).trimmed(); // '\n' 이전까지 한 줄 추출
        m_buffer.remove(0, idx + 1);                    // 처리한 부분을 버퍼에서 제거

        if (line.isEmpty()) continue; // 빈 줄은 건너뜀

        // JSON 파싱: 오류 시 해당 메시지만 무시하고 계속 처리
        QJsonParseError err;
        QJsonDocument   doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError) {
            qWarning() << "[Handler] JSON 파싱 오류:" << err.errorString();
            continue;
        }
        processMessage(doc.object());
    }
}

// 클라이언트 연결 해제 시 호출: deleteLater()로 이벤트 루프 종료 후 안전하게 삭제
void ClientHandler::onDisconnected() {
    qInfo() << "[Handler] 클라이언트 연결 종료";
    deleteLater(); // 소켓(자식)도 함께 삭제됨
}

// JSON 요청의 "type" 필드를 읽어 Bank의 적절한 핸들러로 분기
// 인증이 필요한 요청은 "token" 필드를 Bank에 전달하고, Bank 내부에서 검증
void ClientHandler::processMessage(const QJsonObject& request) {
    QString     type  = request["type"].toString();   // 요청 종류 (register, login 등)
    QString     token = request["token"].toString();  // 세션 토큰 (미인증 요청은 빈 문자열)
    QJsonObject data  = request["data"].toObject();   // 요청 데이터 페이로드

    qInfo() << "[Handler] 요청 수신:" << type;

    QJsonObject response;

    // type 값에 따라 Bank의 해당 핸들러 메서드 호출
    if      (type == "check_id")            response = m_bank->handleCheckId(data);
    else if (type == "register")           response = m_bank->handleRegister(data);
    else if (type == "login")              response = m_bank->handleLogin(data);
    else if (type == "logout")             response = m_bank->handleLogout(token);
    else if (type == "create_account")     response = m_bank->handleCreateAccount(token, data);
    else if (type == "list_accounts")      response = m_bank->handleListAccounts(token);
    else if (type == "get_account_detail") response = m_bank->handleGetAccountDetail(token, data);
    else if (type == "deposit")            response = m_bank->handleDeposit(token, data);
    else if (type == "withdraw")           response = m_bank->handleWithdraw(token, data);
    else if (type == "get_user_info")      response = m_bank->handleGetUserInfo(token);
    else if (type == "change_password")    response = m_bank->handleChangePassword(token, data);
    else if (type == "close_account")      response = m_bank->handleCloseAccount(token, data);
    else {
        // 알 수 없는 type이면 오류 응답 직접 생성
        response["type"]    = "error_response";
        response["status"]  = "error";
        response["message"] = "알 수 없는 요청 타입: " + type;
    }

    sendResponse(response);
}

// JSON 응답을 한 줄(Compact) + '\n'으로 직렬화하여 소켓에 전송
// '\n'이 메시지 구분자이므로 반드시 끝에 붙여야 클라이언트가 경계를 인식할 수 있음
void ClientHandler::sendResponse(const QJsonObject& response) {
    QJsonDocument doc(response);
    m_socket->write(doc.toJson(QJsonDocument::Compact) + "\n");
    m_socket->flush(); // 버퍼에 쌓인 데이터를 즉시 OS로 전송
}
