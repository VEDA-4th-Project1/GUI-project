#pragma once

// users.json 파일에 대한 CRUD를 담당하는 Repository 클래스.
// 서버 시작 시 파일을 메모리에 로드하고, 변경이 생길 때마다 파일 전체를 다시 쓴다.
// 원자적 쓰기(임시 파일 → rename)로 파일 손상을 방지한다.

#include "user.h"

#include <QVector>
#include <optional>

class UserRepository {
private:
    QString       m_filePath; // users.json 파일 경로
    QVector<User> m_users;    // 메모리에 올라온 전체 사용자 목록

    void load();        // 파일에서 전체 사용자 데이터를 m_users로 읽기
    void save() const;  // m_users 전체를 파일에 덮어쓰기

public:
    // 생성자: 파일 경로를 받아 디렉토리를 생성하고 즉시 load() 호출
    explicit UserRepository(const QString& filePath);

    // id로 사용자 조회; 없으면 std::nullopt 반환
    std::optional<User> findById(const QString& id) const;
    // 새 사용자 추가 (이미 존재하면 false 반환)
    bool add(const User& user);
    // 기존 사용자 정보 갱신 (비밀번호 변경, 계좌 목록 업데이트 등에 사용)
    bool update(const User& user);
    // id로 사용자 삭제
    bool remove(const QString& id);
    // 해당 id의 사용자가 존재하는지 확인 (중복 가입 방지에 사용)
    bool exists(const QString& id) const;
};
