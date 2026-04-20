#include "checkingaccount.h"

// 생성자: 기반 클래스 Account 초기화(비밀번호 해시화 포함) 후 마이너스 한도 설정
CheckingAccount::CheckingAccount(const QString& accNum, const QString& ownerId,
                                 double initBalance, const QString& plainPassword,
                                 double overdraftLimit)
    : Account(accNum, ownerId, initBalance, plainPassword)
    , m_overdraftLimit(overdraftLimit)
{}

// 출금 구현: (현재잔액 - 출금액)이 -한도 미만이면 예외, 그 외는 허용
void CheckingAccount::withdraw(double amount, const QString& description) {
    if (m_balance - amount < -m_overdraftLimit)
        throw InsufficientFundsException(
            QString("한도 초과: 출금 가능 한도 %1원").arg(m_balance + m_overdraftLimit));
    m_balance -= amount;
    m_transactions.append({"withdraw", amount, QDateTime::currentDateTime(), description});
}

// 기반 클래스 toJson()에 입출금계좌 고유 필드(마이너스 한도)를 추가
QJsonObject CheckingAccount::toJson() const {
    QJsonObject obj = Account::toJson(); // 공통 필드 먼저 직렬화
    obj["overdraftLimit"] = m_overdraftLimit;
    return obj;
}
