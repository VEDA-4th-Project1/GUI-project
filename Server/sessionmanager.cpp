#include "sessionmanager.h"

#include <QUuid>

// 새 세션 발급: QUuid로 고유 토큰 생성 후 userId와 매핑하여 저장
QString SessionManager::createSession(const QString& userId) {
    QString token = QUuid::createUuid().toString(QUuid::WithoutBraces); // 브레이스 없는 UUID 문자열
    m_tokenToUserId[token] = userId;
    return token;
}

// 토큰으로 userId 조회: 존재하지 않으면 빈 문자열 반환
QString SessionManager::getUserId(const QString& token) const {
    return m_tokenToUserId.value(token, QString());
}

// 로그아웃 처리: 해당 토큰을 테이블에서 제거하여 이후 요청에서 unauthorized 반환
void SessionManager::removeSession(const QString& token) {
    m_tokenToUserId.remove(token);
}

// 토큰이 현재 유효한지 확인 (테이블에 키가 존재하면 유효)
bool SessionManager::isValid(const QString& token) const {
    return m_tokenToUserId.contains(token);
}
