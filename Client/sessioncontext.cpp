#include "sessioncontext.h"

SessionContext& SessionContext::instance() {
    static SessionContext ctx;
    return ctx;
}

void SessionContext::setSession(const QString& token, const QString& userId, const QString& userName) {
    m_token    = token;
    m_userId   = userId;
    m_userName = userName;
}

void SessionContext::clear() {
    m_token.clear();
    m_userId.clear();
    m_userName.clear();
}
