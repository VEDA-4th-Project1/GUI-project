#pragma once

// 계좌 도메인의 추상 기반 클래스.
// SavingsAccount(저축계좌)와 CheckingAccount(입출금계좌)의 공통 인터페이스를 정의한다.
//
// OOP 핵심 포인트:
//   - withdraw()와 getType()은 순수 가상 함수 → 파생 클래스가 반드시 구현
//   - virtual ~Account() = default → unique_ptr로 관리할 때 파생 소멸자 보장
//   - fromJson()은 Factory Method 패턴: JSON의 "type" 필드를 보고 올바른 파생 클래스를 생성

#include "transaction.h"

#include <QString>
#include <QDateTime>
#include <QVector>
#include <QJsonObject>
#include <memory>
#include <stdexcept>

class Account {
protected:
    QString              m_accountNumber;      // 계좌번호 (예: "110-123-456789")
    QString              m_ownerId;            // 소유자의 User id
    QString              m_accountPasswordHash; // 계좌 비밀번호 SHA-256 해시 (클라이언트가 설정)
    double               m_balance;            // 현재 잔액 (원)
    QDateTime            m_createdAt;          // 계좌 개설 일시
    QVector<Transaction> m_transactions;       // 거래 내역 목록

public:
    // plainPassword: 클라이언트가 계좌 개설 시 설정한 비밀번호 (내부에서 SHA-256 해시화)
    Account(const QString& accNum, const QString& ownerId,
            double initBalance, const QString& plainPassword);
    virtual ~Account() = default; // 가상 소멸자: unique_ptr<Account>로 파생 객체 삭제 시 필수

    // 입력된 평문 비밀번호가 저장된 해시와 일치하는지 검증
    bool verifyAccountPassword(const QString& plain) const;
    // SHA-256 해시 유틸 (User::hashPassword와 동일한 방식)
    static QString hashPassword(const QString& plain);

    // ── 순수 가상 함수 (파생 클래스에서 반드시 구현) ─────────────
    // 출금 로직: 저축계좌는 잔액 부족 시 예외, 입출금계좌는 한도 내 마이너스 허용
    virtual void    withdraw(double amount, const QString& description = "출금") = 0;
    // 계좌 타입 문자열 반환: "savings" 또는 "checking"
    virtual QString getType() const = 0;
    // JSON 직렬화 (파생 클래스에서 오버라이드해 고유 필드 추가)
    virtual QJsonObject toJson() const;

    // ── 공통 구현 ─────────────────────────────────────────────────
    // 입금: 잔액 증가 후 거래 내역 기록
    virtual void deposit(double amount, const QString& description = "입금");

    // Getter
    QString                     getAccountNumber() const { return m_accountNumber; }
    QString                     getOwnerId()       const { return m_ownerId; }
    double                      getBalance()       const { return m_balance; }
    QDateTime                   getCreatedAt()     const { return m_createdAt; }
    const QVector<Transaction>& getTransactions()  const { return m_transactions; }

    // Factory Method: JSON의 "type" 필드를 읽어 SavingsAccount 또는 CheckingAccount 생성
    static std::unique_ptr<Account> fromJson(const QJsonObject& obj);

    // 잔액 부족 / 한도 초과 시 throw 되는 예외 클래스
    class InsufficientFundsException : public std::runtime_error {
    public:
        explicit InsufficientFundsException(const QString& msg)
            : std::runtime_error(msg.toStdString()) {}
    };
};
