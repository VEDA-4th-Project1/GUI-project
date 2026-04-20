#pragma once

// 입출금계좌 (Account의 파생 클래스).
// overdraftLimit(마이너스 한도) 범위 내에서 잔액이 마이너스가 되어도 출금을 허용한다.
// 한도를 초과하면 InsufficientFundsException을 던진다.

#include "account.h"

class CheckingAccount : public Account {
private:
    double m_overdraftLimit; // 마이너스 허용 한도 (예: 1000000 = 100만원)

public:
    // plainPassword: 클라이언트가 설정한 계좌 비밀번호, overdraftLimit 기본값 100만원
    CheckingAccount(const QString& accNum, const QString& ownerId,
                    double initBalance, const QString& plainPassword,
                    double overdraftLimit = 1000000.0);

    // (잔액 - 출금액) < -한도 이면 예외, 그 외는 마이너스 잔액 허용
    void    withdraw(double amount, const QString& description = "출금") override;
    // 계좌 타입 식별자 반환
    QString getType() const override { return QStringLiteral("checking"); }
    // 기본 toJson()에 overdraftLimit 필드를 추가하여 반환
    QJsonObject toJson() const override;

    double getOverdraftLimit() const { return m_overdraftLimit; }
};
