#pragma once

// accounts.json 파일에 대한 CRUD를 담당하는 Repository 클래스.
// unique_ptr<Account>로 다형적 계좌 객체를 소유·관리한다.
// QVector는 unique_ptr의 복사를 시도하므로 std::vector를 사용한다.

#include "account.h"

#include <vector>
#include <memory>

class AccountRepository {
private:
    QString m_filePath;                               // accounts.json 파일 경로
    std::vector<std::unique_ptr<Account>> m_accounts; // 메모리에 올라온 전체 계좌 목록

    void load();        // 파일에서 전체 계좌 데이터를 m_accounts로 읽기
    void save() const;  // m_accounts 전체를 파일에 덮어쓰기

public:
    // 생성자: 파일 경로를 받아 디렉토리를 생성하고 즉시 load() 호출
    explicit AccountRepository(const QString& filePath);

    // 계좌번호로 계좌 포인터 반환; 없으면 nullptr
    Account*              findByNumber(const QString& accountNumber) const;
    // 특정 사용자가 소유한 계좌 포인터 목록 반환
    std::vector<Account*> findByOwnerId(const QString& ownerId) const;
    // 새 계좌를 목록에 추가하고 파일에 저장 (unique_ptr 소유권 이전)
    bool  add(std::unique_ptr<Account> account);
    // 계좌번호로 계좌를 삭제하고 파일에 저장
    bool  remove(const QString& accountNumber);
    // 입출금 후 변경된 내용을 파일에 반영 (save() 래퍼)
    void  saveAll();

    // 중복 없는 계좌번호 생성: "110-XXX-XXXXXX" 형식의 랜덤 번호
    QString generateAccountNumber() const;
};
