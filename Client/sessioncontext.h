#pragma once

#include <QString>

// 로그인 세션 정보를 앱 전역에서 공유하는 싱글톤.
// 로그인 성공 시 setSession()으로 채우고, 로그아웃 시 clear()로 비운다.
class SessionContext {
public:
    static SessionContext& instance();

    void setSession(const QString& token, const QString& userId, const QString& userName);
    void clear();

    const QString& token()    const { return m_token; }
    const QString& userId()   const { return m_userId; }
    const QString& userName() const { return m_userName; }
    bool           isLoggedIn() const { return !m_token.isEmpty(); }

private:
    SessionContext() = default;

    QString m_token;
    QString m_userId;
    QString m_userName;
};
