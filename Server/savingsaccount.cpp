#include "savingsaccount.h"

// 생성자: 기반 클래스 Account 초기화(비밀번호 해시화 포함) 후 이자율 설정
SavingsAccount::SavingsAccount(const QString& accNum, const QString& ownerId,
                               double initBalance, const QString& plainPassword,
                               double interestRate)
    : Account(accNum, ownerId, initBalance, plainPassword)
    , m_interestRate(interestRate)
{}

// 출금 구현: 잔액이 부족하면 예외를 던지고 트랜잭션을 기록하지 않음
void SavingsAccount::withdraw(double amount, const QString& description) {
    if (m_balance < amount)
        throw InsufficientFundsException(
            QString("잔액 부족: 현재 잔액 %1원").arg(m_balance));
    m_balance -= amount;
    m_transactions.append({"withdraw", amount, QDateTime::currentDateTime(), description});
}

// 기반 클래스 toJson()에 저축계좌 고유 필드(이자율)를 추가
QJsonObject SavingsAccount::toJson() const {
    QJsonObject obj = Account::toJson(); // 공통 필드 먼저 직렬화
    obj["interestRate"] = m_interestRate;
    return obj;
}
