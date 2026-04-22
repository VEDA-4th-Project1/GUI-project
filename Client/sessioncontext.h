#pragma once

#include <QString>

/**
 * @class SessionContext
 * @brief 로그인 세션 정보를 앱 전역에서 공유하는 싱글톤.
 *
 * 로그인 성공 시 setSession()으로 토큰·사용자 정보를 저장하고,
 * 로그아웃 시 clear()로 초기화한다.
 * 모든 페이지/다이얼로그는 instance()를 통해 동일한 인스턴스를 참조한다.
 */
class SessionContext {
public:
    /** 유일한 인스턴스를 반환한다 (함수-로컬 static으로 스레드 안전). */
    static SessionContext& instance();

    /**
     * @brief 로그인 성공 후 세션 정보를 설정한다.
     * @param token    서버가 발급한 인증 토큰
     * @param userId   사용자 아이디 (로그인 ID)
     * @param userName 사용자 이름 (표시용)
     */
    void setSession(const QString& token, const QString& userId, const QString& userName);

    /** 세션 정보를 모두 초기화한다 (로그아웃 시 호출). */
    void clear();

    /** 현재 세션 토큰을 반환한다. */
    const QString& token()    const { return m_token; }

    /** 현재 로그인한 사용자의 아이디를 반환한다. */
    const QString& userId()   const { return m_userId; }

    /** 현재 로그인한 사용자의 이름을 반환한다. */
    const QString& userName() const { return m_userName; }

    /** 토큰이 존재하면 true (로그인 상태 확인). */
    bool           isLoggedIn() const { return !m_token.isEmpty(); }

private:
    SessionContext() = default;  // 외부 생성 금지 (싱글톤)

    QString m_token;      ///< 서버 인증 토큰
    QString m_userId;     ///< 사용자 아이디
    QString m_userName;   ///< 사용자 이름 (화면 표시용)
};
