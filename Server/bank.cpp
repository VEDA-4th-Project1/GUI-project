#include "bank.h"
#include "savingsaccount.h"
#include "checkingaccount.h"

#include <QJsonArray>

// 생성자: 데이터 디렉토리 경로를 기반으로 두 Repository를 초기화
Bank::Bank(const QString& dataDir)
    : m_userRepo(dataDir + "/users.json")
    , m_accountRepo(dataDir + "/accounts.json")
{}

// ─── 응답 빌더 헬퍼 ────────────────────────────────────────────────────────

// 성공 응답 JSON 생성. data가 비어있지 않으면 "data" 필드도 포함
QJsonObject Bank::successResponse(const QString& type, const QString& message,
                                   const QJsonObject& data) const {
    QJsonObject resp;
    resp["type"]    = type + "_response";
    resp["status"]  = "success";
    resp["message"] = message;
    if (!data.isEmpty()) resp["data"] = data;
    return resp;
}

// 오류 응답 JSON 생성
QJsonObject Bank::errorResponse(const QString& type, const QString& message) const {
    QJsonObject resp;
    resp["type"]    = type + "_response";
    resp["status"]  = "error";
    resp["message"] = message;
    return resp;
}

// 인증 실패(토큰 없음/만료) 응답 JSON 생성
QJsonObject Bank::unauthorizedResponse(const QString& type) const {
    QJsonObject resp;
    resp["type"]    = type + "_response";
    resp["status"]  = "unauthorized";
    resp["message"] = "인증이 필요합니다";
    return resp;
}

// ─── 요청 핸들러 ───────────────────────────────────────────────────────────

// 아이디 중복 확인: 형식 검사 → 중복 여부만 확인하고 저장하지 않음
QJsonObject Bank::handleCheckId(const QJsonObject& data) {
    QString id = data["id"].toString().trimmed();
    if (id.isEmpty() || id.length() < 3)
        return errorResponse("check_id", "아이디는 3자 이상이어야 합니다");
    if (m_userRepo.exists(id))
        return errorResponse("check_id", "이미 사용 중인 아이디입니다");
    return successResponse("check_id", "사용 가능한 아이디입니다");
}

// 회원가입: 입력값 유효성 검사 → 중복 id 확인 → User 객체 생성 후 저장
QJsonObject Bank::handleRegister(const QJsonObject& data) {
    QString id       = data["id"].toString().trimmed();
    QString password = data["password"].toString();
    QString name     = data["name"].toString().trimmed();

    // 입력값 유효성 검사
    if (id.isEmpty() || password.isEmpty() || name.isEmpty())
        return errorResponse("register", "아이디, 비밀번호, 이름을 모두 입력해주세요");
    if (id.length() < 3)
        return errorResponse("register", "아이디는 3자 이상이어야 합니다");
    if (password.length() < 4)
        return errorResponse("register", "비밀번호는 4자 이상이어야 합니다");
    if (m_userRepo.exists(id))
        return errorResponse("register", "이미 사용 중인 아이디입니다");

    // User 생성자에서 비밀번호를 SHA-256으로 해시화하여 저장
    m_userRepo.add(User(id, password, name));
    return successResponse("register", "회원가입이 완료되었습니다");
}

// 로그인: id/비밀번호 검증 → 세션 토큰 발급 → 토큰과 사용자 기본 정보 반환
QJsonObject Bank::handleLogin(const QJsonObject& data) {
    QString id       = data["id"].toString();
    QString password = data["password"].toString();

    auto userOpt = m_userRepo.findById(id);
    // 사용자 없거나 비밀번호 불일치 시 동일한 오류 메시지 (보안: 어느 쪽인지 노출 방지)
    if (!userOpt || !userOpt->verifyPassword(password))
        return errorResponse("login", "아이디 또는 비밀번호가 올바르지 않습니다");

    QString token = m_sessionManager.createSession(id); // UUID 토큰 발급

    QJsonObject userInfo;
    userInfo["id"]   = userOpt->getId();
    userInfo["name"] = userOpt->getName();

    QJsonObject respData;
    respData["token"] = token; // 클라이언트는 이 토큰을 이후 모든 요청에 포함
    respData["user"]  = userInfo;
    return successResponse("login", "로그인 성공", respData);
}

// 로그아웃: 토큰 유효성 확인 → 세션 테이블에서 토큰 삭제
QJsonObject Bank::handleLogout(const QString& token) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("logout");
    m_sessionManager.removeSession(token);
    return successResponse("logout", "로그아웃되었습니다");
}

// 계좌 개설: 토큰으로 소유자 확인 → 타입에 따라 파생 클래스 생성 → 초기 입금 → 저장
QJsonObject Bank::handleCreateAccount(const QString& token, const QJsonObject& data) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("create_account");

    QString userId      = m_sessionManager.getUserId(token);
    QString type        = data["type"].toString();
    QString accPassword = data["accountPassword"].toString(); // 클라이언트가 설정한 계좌 비밀번호
    double  initBalance = data["initBalance"].toDouble(0.0);

    if (type != "savings" && type != "checking")
        return errorResponse("create_account", "계좌 타입은 'savings' 또는 'checking'이어야 합니다");
    if (accPassword.length() < 4)
        return errorResponse("create_account", "계좌 비밀번호는 4자 이상이어야 합니다");
    if (initBalance < 0)
        return errorResponse("create_account", "초기 입금액은 0 이상이어야 합니다");

    QString accNum = m_accountRepo.generateAccountNumber();
    std::unique_ptr<Account> account;

    if (type == "savings") {
        double rate = data["interestRate"].toDouble(0.03);
        account = std::make_unique<SavingsAccount>(accNum, userId, 0.0, accPassword, rate);
    } else {
        double limit = data["overdraftLimit"].toDouble(1000000.0);
        account = std::make_unique<CheckingAccount>(accNum, userId, 0.0, accPassword, limit);
    }

    // 초기 입금액이 있으면 거래 내역과 함께 잔액에 반영
    if (initBalance > 0)
        account->deposit(initBalance, "초기 입금");

    m_accountRepo.add(std::move(account)); // unique_ptr 소유권 이전

    // User의 accountNumbers 목록에도 새 계좌번호 추가 (양방향 연결)
    auto userOpt = m_userRepo.findById(userId);
    if (userOpt) {
        User user = *userOpt;
        user.addAccount(accNum);
        m_userRepo.update(user);
    }

    QJsonObject respData;
    respData["accountNumber"] = accNum;
    return successResponse("create_account", "계좌가 개설되었습니다", respData);
}

// 계좌 목록 조회: 토큰으로 소유자 확인 → 해당 userId의 계좌를 요약 정보로 반환
QJsonObject Bank::handleListAccounts(const QString& token) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("list_accounts");

    QString userId = m_sessionManager.getUserId(token);

    QJsonArray arr;
    for (auto* acc : m_accountRepo.findByOwnerId(userId)) {
        QJsonObject obj;
        obj["accountNumber"] = acc->getAccountNumber();
        obj["type"]          = acc->getType();
        obj["balance"]       = acc->getBalance();
        obj["createdAt"]     = acc->getCreatedAt().toString(Qt::ISODate);
        arr.append(obj); // 거래 내역은 제외 (상세 조회에서만 포함)
    }

    QJsonObject respData;
    respData["accounts"] = arr;
    return successResponse("list_accounts", "계좌 목록 조회 성공", respData);
}

// 계좌 상세 조회: 소유권 검증 후 거래 내역을 포함한 계좌 전체 정보 반환
QJsonObject Bank::handleGetAccountDetail(const QString& token, const QJsonObject& data) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("get_account_detail");

    QString  userId     = m_sessionManager.getUserId(token);
    QString  accNum     = data["accountNumber"].toString();
    QString  accPassword = data["accountPassword"].toString(); // 계좌 비밀번호 검증
    Account* acc        = m_accountRepo.findByNumber(accNum);

    if (!acc)
        return errorResponse("get_account_detail", "계좌를 찾을 수 없습니다");
    if (acc->getOwnerId() != userId)
        return errorResponse("get_account_detail", "접근 권한이 없습니다");
    if (!acc->verifyAccountPassword(accPassword))
        return errorResponse("get_account_detail", "계좌 비밀번호가 올바르지 않습니다");

    QJsonObject respData;
    respData["account"] = acc->toJson(); // 거래 내역 포함 전체 직렬화
    return successResponse("get_account_detail", "계좌 상세 조회 성공", respData);
}

// 입금: 소유권 검증 → deposit() 호출 → 변경된 잔액을 파일에 저장
QJsonObject Bank::handleDeposit(const QString& token, const QJsonObject& data) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("deposit");

    QString userId      = m_sessionManager.getUserId(token);
    QString accNum      = data["accountNumber"].toString();
    QString accPassword = data["accountPassword"].toString(); // 계좌 비밀번호 검증
    double  amount      = data["amount"].toDouble(0.0);
    QString description = data["description"].toString("입금");

    if (amount <= 0)
        return errorResponse("deposit", "입금액은 0보다 커야 합니다");

    Account* acc = m_accountRepo.findByNumber(accNum);
    if (!acc)
        return errorResponse("deposit", "계좌를 찾을 수 없습니다");
    if (acc->getOwnerId() != userId)
        return errorResponse("deposit", "접근 권한이 없습니다");
    if (!acc->verifyAccountPassword(accPassword))
        return errorResponse("deposit", "계좌 비밀번호가 올바르지 않습니다");

    acc->deposit(amount, description); // 잔액 증가 + 거래 내역 기록
    m_accountRepo.saveAll();           // 변경 내용을 파일에 즉시 반영

    QJsonObject respData;
    respData["balance"] = acc->getBalance(); // 입금 후 최신 잔액 반환
    return successResponse("deposit", QString("입금 완료: %1원").arg(amount), respData);
}

// 출금: 소유권 검증 → withdraw() 호출 → 잔액 부족 예외 처리 → 파일 저장
QJsonObject Bank::handleWithdraw(const QString& token, const QJsonObject& data) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("withdraw");

    QString userId      = m_sessionManager.getUserId(token);
    QString accNum      = data["accountNumber"].toString();
    QString accPassword = data["accountPassword"].toString(); // 계좌 비밀번호 검증
    double  amount      = data["amount"].toDouble(0.0);
    QString description = data["description"].toString("출금");

    if (amount <= 0)
        return errorResponse("withdraw", "출금액은 0보다 커야 합니다");

    Account* acc = m_accountRepo.findByNumber(accNum);
    if (!acc)
        return errorResponse("withdraw", "계좌를 찾을 수 없습니다");
    if (acc->getOwnerId() != userId)
        return errorResponse("withdraw", "접근 권한이 없습니다");
    if (!acc->verifyAccountPassword(accPassword))
        return errorResponse("withdraw", "계좌 비밀번호가 올바르지 않습니다");

    try {
        acc->withdraw(amount, description); // 계좌 타입에 따라 예외 조건이 다름
        m_accountRepo.saveAll();
    } catch (const Account::InsufficientFundsException& e) {
        // 잔액 부족 / 한도 초과 시 예외를 잡아 오류 응답으로 변환
        return errorResponse("withdraw", QString::fromStdString(e.what()));
    }

    QJsonObject respData;
    respData["balance"] = acc->getBalance(); // 출금 후 최신 잔액 반환
    return successResponse("withdraw", QString("출금 완료: %1원").arg(amount), respData);
}

// 내 정보 조회: 토큰으로 userId 확인 → id·이름·가입일 반환
QJsonObject Bank::handleGetUserInfo(const QString& token) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("get_user_info");

    QString userId  = m_sessionManager.getUserId(token);
    auto    userOpt = m_userRepo.findById(userId);
    if (!userOpt)
        return errorResponse("get_user_info", "사용자를 찾을 수 없습니다");

    QJsonObject userInfo;
    userInfo["id"]        = userOpt->getId();
    userInfo["name"]      = userOpt->getName();
    userInfo["createdAt"] = userOpt->getCreatedAt().toString(Qt::ISODate);

    QJsonObject respData;
    respData["user"] = userInfo;
    return successResponse("get_user_info", "사용자 정보 조회 성공", respData);
}

// 비밀번호 변경: 현재 비밀번호 검증 → 새 비밀번호 해시화 후 저장
QJsonObject Bank::handleChangePassword(const QString& token, const QJsonObject& data) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("change_password");

    QString userId    = m_sessionManager.getUserId(token);
    QString currentPw = data["currentPassword"].toString();
    QString newPw     = data["newPassword"].toString();

    if (newPw.length() < 4)
        return errorResponse("change_password", "새 비밀번호는 4자 이상이어야 합니다");

    auto userOpt = m_userRepo.findById(userId);
    if (!userOpt)
        return errorResponse("change_password", "사용자를 찾을 수 없습니다");
    if (!userOpt->verifyPassword(currentPw)) // 현재 비밀번호 검증
        return errorResponse("change_password", "현재 비밀번호가 올바르지 않습니다");

    User user = *userOpt;
    user.changePassword(newPw); // 내부에서 새 비밀번호를 SHA-256으로 해시화
    m_userRepo.update(user);
    return successResponse("change_password", "비밀번호가 변경되었습니다");
}

// 계좌 해지: 소유권 검증 → 계좌 삭제 → User의 accountNumbers 목록에서도 제거
QJsonObject Bank::handleCloseAccount(const QString& token, const QJsonObject& data) {
    if (!m_sessionManager.isValid(token))
        return unauthorizedResponse("close_account");

    QString  userId      = m_sessionManager.getUserId(token);
    QString  accNum      = data["accountNumber"].toString();
    QString  accPassword = data["accountPassword"].toString(); // 계좌 비밀번호 검증
    Account* acc         = m_accountRepo.findByNumber(accNum);

    if (!acc)
        return errorResponse("close_account", "계좌를 찾을 수 없습니다");
    if (acc->getOwnerId() != userId)
        return errorResponse("close_account", "접근 권한이 없습니다");
    if (!acc->verifyAccountPassword(accPassword))
        return errorResponse("close_account", "계좌 비밀번호가 올바르지 않습니다");

    m_accountRepo.remove(accNum); // accounts.json에서 삭제

    // User의 소유 계좌 목록에서도 제거 (양방향 일관성 유지)
    auto userOpt = m_userRepo.findById(userId);
    if (userOpt) {
        User user = *userOpt;
        user.removeAccount(accNum);
        m_userRepo.update(user);
    }

    return successResponse("close_account", "계좌가 해지되었습니다");
}
