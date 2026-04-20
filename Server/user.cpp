#include "user.h"

#include <QCryptographicHash>
#include <QJsonArray>

// 생성자: 가입 시각을 현재 시각으로 기록하고 비밀번호를 즉시 해시화
User::User(const QString& id, const QString& plainPassword, const QString& name)
    : m_id(id)
    , m_passwordHash(hashPassword(plainPassword))
    , m_name(name)
    , m_createdAt(QDateTime::currentDateTime())
{}

// 로그인 시 비밀번호 검증: 입력값을 해시화해서 저장된 해시와 비교
bool User::verifyPassword(const QString& plain) const {
    return m_passwordHash == hashPassword(plain);
}

// 비밀번호 변경: 새 평문을 해시화하여 기존 해시를 덮어씀
void User::changePassword(const QString& newPlain) {
    m_passwordHash = hashPassword(newPlain);
}

// 계좌 개설 시 호출: 중복 없이 계좌번호를 소유 목록에 추가
void User::addAccount(const QString& accountNumber) {
    if (!m_accountNumbers.contains(accountNumber))
        m_accountNumbers.append(accountNumber);
}

// 계좌 해지 시 호출: 소유 목록에서 해당 계좌번호를 제거
void User::removeAccount(const QString& accountNumber) {
    m_accountNumbers.removeAll(accountNumber);
}

// users.json에 저장할 수 있도록 QJsonObject로 직렬화
QJsonObject User::toJson() const {
    QJsonArray accs;
    for (const auto& n : m_accountNumbers)
        accs.append(n);

    QJsonObject obj;
    obj["id"]             = m_id;
    obj["passwordHash"]   = m_passwordHash;
    obj["name"]           = m_name;
    obj["createdAt"]      = m_createdAt.toString(Qt::ISODate);
    obj["accountNumbers"] = accs;
    return obj;
}

// users.json에서 읽은 QJsonObject를 User 객체로 역직렬화
User User::fromJson(const QJsonObject& obj) {
    User u;
    u.m_id           = obj["id"].toString();
    u.m_passwordHash = obj["passwordHash"].toString(); // 이미 해시값이므로 재해시하지 않음
    u.m_name         = obj["name"].toString();
    u.m_createdAt    = QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate);
    for (const auto& v : obj["accountNumbers"].toArray())
        u.m_accountNumbers.append(v.toString());
    return u;
}

// SHA-256 해시 유틸: 평문 문자열 → hex 인코딩된 해시 문자열 반환
QString User::hashPassword(const QString& plain) {
    return QString(QCryptographicHash::hash(plain.toUtf8(), QCryptographicHash::Sha256).toHex());
}
