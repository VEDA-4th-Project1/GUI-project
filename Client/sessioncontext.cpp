#include "sessioncontext.h"

/** 함수-로컬 static을 사용해 처음 호출 시 단 한 번만 생성된다. */
SessionContext& SessionContext::instance() {
    static SessionContext ctx;
    return ctx;
}

/** 세 필드를 한꺼번에 설정한다. 로그인 성공 콜백에서 호출된다. */
void SessionContext::setSession(const QString& token, const QString& userId, const QString& userName) {
    m_token    = token;
    m_userId   = userId;
    m_userName = userName;
}

/** 모든 세션 필드를 빈 문자열로 초기화한다. 로그아웃 시 호출된다. */
void SessionContext::clear() {
    m_token.clear();
    m_userId.clear();
    m_userName.clear();
}
