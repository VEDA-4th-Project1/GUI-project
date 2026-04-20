#pragma once

// 저축계좌 (Account의 파생 클래스).
// 잔액이 출금액보다 적으면 InsufficientFundsException을 던진다.
// 이자율(interestRate)을 추가 멤버로 가지며 JSON 직렬화 시 함께 저장된다.

#include "account.h"

class SavingsAccount : public Account {
private:
    double m_interestRate; // 연 이자율 (예: 0.03 = 3%)

public:
    // plainPassword: 클라이언트가 설정한 계좌 비밀번호, interestRate 기본값 3%
    SavingsAccount(const QString& accNum, const QString& ownerId,
                   double initBalance, const QString& plainPassword,
                   double interestRate = 0.03);

    // 잔액 부족 시 InsufficientFundsException 예외 발생
    void    withdraw(double amount, const QString& description = "출금") override;
    // 계좌 타입 식별자 반환
    QString getType() const override { return QStringLiteral("savings"); }
    // 기본 toJson()에 interestRate 필드를 추가하여 반환
    QJsonObject toJson() const override;

    double getInterestRate() const { return m_interestRate; }
};
