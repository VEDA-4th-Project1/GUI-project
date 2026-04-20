#pragma once

// 회원 정보를 담는 도메인 클래스.
// 비밀번호는 SHA-256 해시로만 저장하며 평문은 절대 보관하지 않는다.
// users.json에 직렬화/역직렬화되어 영구 저장된다.

#include <QString>
#include <QDateTime>
#include <QStringList>
#include <QJsonObject>

class User {
private:
    QString     m_id;             // 로그인 아이디 (고유값)
    QString     m_passwordHash;   // SHA-256 해시된 비밀번호
    QString     m_name;           // 사용자 이름
    QDateTime   m_createdAt;      // 가입 일시
    QStringList m_accountNumbers; // 이 사용자가 소유한 계좌번호 목록

public:
    User() = default;
    // 생성자: 평문 비밀번호를 받아 내부적으로 해시화하여 저장
    User(const QString& id, const QString& plainPassword, const QString& name);

    // 입력된 평문 비밀번호가 저장된 해시와 일치하는지 검증
    bool verifyPassword(const QString& plain) const;
    // 새 비밀번호로 해시를 갱신 (비밀번호 변경)
    void changePassword(const QString& newPlain);

    // 소유 계좌번호 목록에 추가 (중복 방지 포함)
    void addAccount(const QString& accountNumber);
    // 소유 계좌번호 목록에서 제거 (계좌 해지 시 호출)
    void removeAccount(const QString& accountNumber);

    // 이 객체를 QJsonObject로 변환 (users.json 저장용)
    QJsonObject toJson() const;
    // QJsonObject에서 User 객체를 복원 (users.json 로드용)
    static User fromJson(const QJsonObject& obj);

    // Getter
    QString            getId()             const { return m_id; }
    QString            getName()           const { return m_name; }
    QDateTime          getCreatedAt()      const { return m_createdAt; }
    const QStringList& getAccountNumbers() const { return m_accountNumbers; }

    // 평문 문자열을 SHA-256 hex 문자열로 변환하는 유틸
    static QString hashPassword(const QString& plain);
};
