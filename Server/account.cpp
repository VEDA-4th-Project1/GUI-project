#include "account.h"
// fromJson에서 파생 클래스 객체를 생성하기 위해 두 헤더를 여기서만 포함
// (account.h에 포함하면 순환 include 발생)
#include "savingsaccount.h"
#include "checkingaccount.h"

#include <QCryptographicHash>
#include <QJsonArray>

// 생성자: 계좌 개설 시각을 현재 시각으로 기록하고 비밀번호를 해시화하여 저장
Account::Account(const QString& accNum, const QString& ownerId,
                 double initBalance, const QString& plainPassword)
    : m_accountNumber(accNum)
    , m_ownerId(ownerId)
    , m_accountPasswordHash(hashPassword(plainPassword))
    , m_balance(initBalance)
    , m_createdAt(QDateTime::currentDateTime())
{}

// 입력된 평문 비밀번호를 해시화하여 저장된 해시와 비교
bool Account::verifyAccountPassword(const QString& plain) const {
    return m_accountPasswordHash == hashPassword(plain);
}

// SHA-256 해시: User::hashPassword와 동일한 방식
QString Account::hashPassword(const QString& plain) {
    return QString(QCryptographicHash::hash(plain.toUtf8(), QCryptographicHash::Sha256).toHex());
}

// 입금 공통 구현: 잔액에 금액을 더하고 거래 내역을 기록
void Account::deposit(double amount, const QString& description) {
    m_balance += amount;
    m_transactions.append({"deposit", amount, QDateTime::currentDateTime(), description});
}

// 공통 JSON 직렬화: 계좌번호·소유자·잔액·거래내역을 포함
// 파생 클래스에서 super()처럼 호출한 뒤 고유 필드(이자율 등)를 추가한다
QJsonObject Account::toJson() const {
    QJsonArray txns;
    for (const auto& t : m_transactions)
        txns.append(t.toJson());

    QJsonObject obj;
    obj["accountNumber"]      = m_accountNumber;
    obj["ownerId"]            = m_ownerId;
    obj["accountPasswordHash"] = m_accountPasswordHash; // 해시값 그대로 저장
    obj["type"]               = getType();
    obj["balance"]            = m_balance;
    obj["createdAt"]          = m_createdAt.toString(Qt::ISODate);
    obj["transactions"]       = txns;
    return obj;
}

// Factory Method: accounts.json 로드 시 "type" 필드로 올바른 파생 클래스를 생성
// → "savings"이면 SavingsAccount, 그 외이면 CheckingAccount
std::unique_ptr<Account> Account::fromJson(const QJsonObject& obj) {
    QString type = obj["type"].toString();

    std::unique_ptr<Account> account;
    if (type == "savings") {
        // 빈 비밀번호로 생성 후 아래에서 해시를 JSON 값으로 직접 덮어씀
        account = std::make_unique<SavingsAccount>(
            obj["accountNumber"].toString(),
            obj["ownerId"].toString(),
            obj["balance"].toDouble(),
            QString(), // 임시 빈 비밀번호 (해시는 아래에서 복원)
            obj["interestRate"].toDouble(0.03)
        );
    } else {
        account = std::make_unique<CheckingAccount>(
            obj["accountNumber"].toString(),
            obj["ownerId"].toString(),
            obj["balance"].toDouble(),
            QString(), // 임시 빈 비밀번호 (해시는 아래에서 복원)
            obj["overdraftLimit"].toDouble(1000000.0)
        );
    }

    // 개설 시각, 거래 내역, 비밀번호 해시를 파일에서 복원
    // (protected 멤버이므로 같은 클래스의 static 메서드에서 접근 가능)
    // 비밀번호 해시는 이미 해시값이므로 재해시하지 않고 직접 대입
    account->m_accountPasswordHash = obj["accountPasswordHash"].toString();
    account->m_createdAt = QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate);
    for (const auto& v : obj["transactions"].toArray())
        account->m_transactions.append(Transaction::fromJson(v.toObject()));

    return account;
}
