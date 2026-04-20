#include "accountrepository.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>

// 생성자: 데이터 디렉토리가 없으면 생성하고 기존 파일을 로드
AccountRepository::AccountRepository(const QString& filePath) : m_filePath(filePath) {
    QDir().mkpath(QFileInfo(filePath).absolutePath()); // 디렉토리 자동 생성
    load();
}

// 파일에서 전체 계좌 목록을 읽어 m_accounts에 저장
// Account::fromJson()이 Factory Method로 올바른 파생 클래스를 생성
void AccountRepository::load() {
    m_accounts.clear();
    QFile file(m_filePath);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    for (const auto& v : doc.object()["accounts"].toArray())
        m_accounts.push_back(Account::fromJson(v.toObject()));
}

// m_accounts 전체를 JSON으로 직렬화하여 파일에 저장
// 원자적 쓰기: 임시 파일(.tmp)에 먼저 쓴 뒤 rename으로 교체
void AccountRepository::save() const {
    QJsonArray arr;
    for (const auto& a : m_accounts)
        arr.append(a->toJson()); // 다형성: 파생 클래스의 toJson() 호출

    QJsonObject root;
    root["accounts"] = arr;

    QString tmpPath = m_filePath + ".tmp";
    QFile file(tmpPath);
    if (!file.open(QIODevice::WriteOnly)) return;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();

    QFile::remove(m_filePath);           // 기존 파일 삭제
    QFile::rename(tmpPath, m_filePath);  // 임시 파일을 정식 파일로 교체
}

// 계좌번호로 선형 탐색; 없으면 nullptr 반환
Account* AccountRepository::findByNumber(const QString& accountNumber) const {
    for (const auto& a : m_accounts)
        if (a->getAccountNumber() == accountNumber) return a.get();
    return nullptr;
}

// 특정 userId가 소유한 계좌의 원시 포인터 목록 반환 (읽기 전용으로 사용)
std::vector<Account*> AccountRepository::findByOwnerId(const QString& ownerId) const {
    std::vector<Account*> result;
    for (const auto& a : m_accounts)
        if (a->getOwnerId() == ownerId) result.push_back(a.get());
    return result;
}

// 새 계좌를 목록에 추가 (unique_ptr 소유권을 이전받음)
bool AccountRepository::add(std::unique_ptr<Account> account) {
    m_accounts.push_back(std::move(account));
    save();
    return true;
}

// 계좌번호로 계좌를 찾아 벡터에서 삭제하고 파일에 저장
bool AccountRepository::remove(const QString& accountNumber) {
    for (auto it = m_accounts.begin(); it != m_accounts.end(); ++it) {
        if ((*it)->getAccountNumber() == accountNumber) {
            m_accounts.erase(it);
            save();
            return true;
        }
    }
    return false;
}

// 입출금 후 변경된 잔액/거래내역을 파일에 즉시 반영
void AccountRepository::saveAll() {
    save();
}

// 중복 없는 계좌번호를 생성: "110-XXX-XXXXXX" 형식
// 이미 존재하는 번호가 나오면 다시 생성 (충돌 회피)
QString AccountRepository::generateAccountNumber() const {
    QString num;
    do {
        quint32 p1 = QRandomGenerator::global()->bounded(100u, 1000u);   // 3자리
        quint32 p2 = QRandomGenerator::global()->bounded(100000u, 1000000u); // 6자리
        num = QString("110-%1-%2").arg(p1).arg(p2);
    } while (findByNumber(num) != nullptr);
    return num;
}
