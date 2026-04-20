#pragma once

// 비즈니스 로직의 Facade 클래스.
// ClientHandler가 받은 JSON 요청을 이 클래스의 handle* 메서드로 위임한다.
// 각 메서드는 토큰 검증 → 입력 유효성 검사 → 도메인 처리 → JSON 응답 반환 순으로 동작한다.
//
// 내부적으로 세 가지 의존성을 가진다:
//   - UserRepository    : users.json CRUD
//   - AccountRepository : accounts.json CRUD
//   - SessionManager    : 토큰 기반 세션 관리

#include "userrepository.h"
#include "accountrepository.h"
#include "sessionmanager.h"

#include <QJsonObject>

class Bank {
private:
    UserRepository    m_userRepo;       // 사용자 데이터 영속성 담당
    AccountRepository m_accountRepo;    // 계좌 데이터 영속성 담당
    SessionManager    m_sessionManager; // 로그인 세션 관리 담당

    // ── 응답 빌더 헬퍼 ─────────────────────────────────────────────
    // 성공 응답 생성: { type, status:"success", message, data(옵션) }
    QJsonObject successResponse(const QString& type, const QString& message,
                                const QJsonObject& data = {}) const;
    // 오류 응답 생성: { type, status:"error", message }
    QJsonObject errorResponse(const QString& type, const QString& message) const;
    // 인증 실패 응답 생성: { type, status:"unauthorized", message }
    QJsonObject unauthorizedResponse(const QString& type) const;

public:
    // dataDir: users.json과 accounts.json이 저장될 디렉토리 경로
    explicit Bank(const QString& dataDir);

    // ── 요청 핸들러 (ClientHandler에서 호출) ───────────────────────
    QJsonObject handleCheckId(const QJsonObject& data);                            // 아이디 중복 확인
    QJsonObject handleRegister(const QJsonObject& data);                          // 회원가입
    QJsonObject handleLogin(const QJsonObject& data);                             // 로그인
    QJsonObject handleLogout(const QString& token);                               // 로그아웃
    QJsonObject handleCreateAccount(const QString& token, const QJsonObject& data); // 계좌 개설
    QJsonObject handleListAccounts(const QString& token);                         // 계좌 목록 조회
    QJsonObject handleGetAccountDetail(const QString& token, const QJsonObject& data); // 계좌 상세 조회
    QJsonObject handleDeposit(const QString& token, const QJsonObject& data);     // 입금
    QJsonObject handleWithdraw(const QString& token, const QJsonObject& data);    // 출금
    QJsonObject handleGetUserInfo(const QString& token);                          // 내 정보 조회
    QJsonObject handleChangePassword(const QString& token, const QJsonObject& data); // 비밀번호 변경
    QJsonObject handleCloseAccount(const QString& token, const QJsonObject& data);   // 계좌 해지
};
