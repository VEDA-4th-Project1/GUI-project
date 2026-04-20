#pragma once

// 서버-클라이언트 간 JSON 통신에서 사용하는 문자열 상수 모음.
// 요청 type 필드와 응답 status 필드에 쓰이는 값을 한 곳에 모아
// 오타로 인한 버그를 방지하고 양쪽 코드를 일치시킨다.

#include <QtGlobal>

namespace Protocol {

// ── 요청 타입 (Client → Server, JSON "type" 필드 값) ──────────────
inline constexpr const char* REQ_REGISTER         = "register";        // 회원가입
inline constexpr const char* REQ_LOGIN            = "login";           // 로그인
inline constexpr const char* REQ_LOGOUT           = "logout";          // 로그아웃
inline constexpr const char* REQ_CREATE_ACCOUNT   = "create_account";  // 신규 계좌 개설
inline constexpr const char* REQ_LIST_ACCOUNTS    = "list_accounts";   // 내 계좌 목록 조회
inline constexpr const char* REQ_GET_DETAIL       = "get_account_detail"; // 계좌 상세 + 거래내역 조회
inline constexpr const char* REQ_DEPOSIT          = "deposit";         // 입금
inline constexpr const char* REQ_WITHDRAW         = "withdraw";        // 출금
inline constexpr const char* REQ_GET_USER_INFO    = "get_user_info";   // 내 정보 조회
inline constexpr const char* REQ_CHANGE_PASSWORD  = "change_password"; // 비밀번호 변경
inline constexpr const char* REQ_CLOSE_ACCOUNT    = "close_account";   // 계좌 해지

// ── 응답 상태 (Server → Client, JSON "status" 필드 값) ────────────
inline constexpr const char* STATUS_SUCCESS       = "success";       // 정상 처리
inline constexpr const char* STATUS_ERROR         = "error";         // 오류 발생
inline constexpr const char* STATUS_UNAUTHORIZED  = "unauthorized";  // 토큰 없음 / 만료

// ── 기본 포트 ──────────────────────────────────────────────────────
inline constexpr quint16 PORT = 9999;

} // namespace Protocol
