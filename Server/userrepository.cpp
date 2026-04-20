#include "userrepository.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// 생성자: 데이터 디렉토리가 없으면 생성하고 기존 파일을 로드
UserRepository::UserRepository(const QString& filePath) : m_filePath(filePath) {
    QDir().mkpath(QFileInfo(filePath).absolutePath()); // 디렉토리 자동 생성
    load();
}

// 파일에서 전체 사용자 목록을 읽어 m_users에 저장
// 파일이 없으면 빈 목록으로 시작 (최초 실행 시)
void UserRepository::load() {
    m_users.clear();
    QFile file(m_filePath);
    if (!file.exists()) return;
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    for (const auto& v : doc.object()["users"].toArray())
        m_users.append(User::fromJson(v.toObject()));
}

// m_users 전체를 JSON으로 직렬화하여 파일에 저장
// 원자적 쓰기: 임시 파일(.tmp)에 먼저 쓴 뒤 rename으로 교체
// → 쓰기 도중 프로세스가 죽어도 기존 파일이 손상되지 않음
void UserRepository::save() const {
    QJsonArray arr;
    for (const auto& u : m_users)
        arr.append(u.toJson());

    QJsonObject root;
    root["users"] = arr;

    QString tmpPath = m_filePath + ".tmp";
    QFile file(tmpPath);
    if (!file.open(QIODevice::WriteOnly)) return;
    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();

    QFile::remove(m_filePath);           // 기존 파일 삭제
    QFile::rename(tmpPath, m_filePath);  // 임시 파일을 정식 파일로 교체
}

// id로 사용자 선형 탐색; 없으면 std::nullopt 반환
std::optional<User> UserRepository::findById(const QString& id) const {
    for (const auto& u : m_users)
        if (u.getId() == id) return u;
    return std::nullopt;
}

// 새 사용자를 목록에 추가하고 파일에 저장 (중복 id면 실패)
bool UserRepository::add(const User& user) {
    if (exists(user.getId())) return false;
    m_users.append(user);
    save();
    return true;
}

// 동일 id를 가진 기존 항목을 새 데이터로 교체하고 파일에 저장
bool UserRepository::update(const User& user) {
    for (auto& u : m_users) {
        if (u.getId() == user.getId()) {
            u = user;
            save();
            return true;
        }
    }
    return false;
}

// 해당 id의 사용자를 목록에서 제거하고 파일에 저장
bool UserRepository::remove(const QString& id) {
    for (int i = 0; i < m_users.size(); ++i) {
        if (m_users[i].getId() == id) {
            m_users.removeAt(i);
            save();
            return true;
        }
    }
    return false;
}

// findById()로 존재 여부만 확인 (중복 가입 방지용)
bool UserRepository::exists(const QString& id) const {
    return findById(id).has_value();
}
