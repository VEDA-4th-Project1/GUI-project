#pragma once

// 로그인 세션을 UUID 토큰으로 관리하는 클래스.
// 로그인 성공 시 UUID를 발급하고 이후 모든 인증 필요 요청에서 토큰을 검증한다.
// 토큰-userId 매핑을 메모리(QHash)에만 유지하므로 서버 재시작 시 세션은 초기화된다.

#include <QString>
#include <QHash>

class SessionManager {
private:
    QHash<QString, QString> m_tokenToUserId; // 토큰(key) → userId(value) 매핑 테이블

public:
    // 로그인 성공 시 UUID 토큰을 발급하고 userId와 연결 후 반환
    QString createSession(const QString& userId);
    // 토큰으로 로그인 중인 userId를 조회 (없으면 빈 문자열 반환)
    QString getUserId(const QString& token) const;
    // 로그아웃 시 해당 토큰을 테이블에서 삭제
    void    removeSession(const QString& token);
    // 요청 처리 전 토큰 유효성 확인 (테이블에 존재하는지)
    bool    isValid(const QString& token) const;
};
