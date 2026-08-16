# Easy Bank — Qt6 은행 클라이언트/서버

Qt6 기반의 TCP 클라이언트–서버 은행 애플리케이션입니다.
콘솔 서버(`BankServer`)가 계좌·회원 데이터를 JSON 파일로 관리하고,
GUI 클라이언트(`BankClient`)가 줄바꿈(`\n`)으로 구분된 JSON 메시지로 서버와 통신합니다.

| | |
|---|---|
| 언어 / 프레임워크 | C++17, Qt 6.5+ (Core, Widgets, Network) |
| 빌드 | CMake 3.19+ (Client) / 3.16+ (Server) |
| 통신 | TCP, 기본 포트 `9999`, 줄 단위 JSON |
| 저장소 | 서버 실행 파일 옆 `data/users.json`, `data/accounts.json` |

---

## 디렉토리 구조

```
GUI-project/
├── Server/          # 콘솔 TCP 서버 (BankServer)
│   ├── main.cpp             # 진입점, data 디렉토리 준비 + 포트 파싱
│   ├── server.*             # QTcpServer 상속, 연결 수락
│   ├── clienthandler.*      # 소켓 1개 담당, '\n' 단위 JSON 파싱 → Bank 위임
│   ├── bank.*               # 비즈니스 로직 Facade (handle* 메서드)
│   ├── protocol.h           # 요청 type / 응답 status / 기본 포트 상수
│   ├── user.*               # 회원 도메인 (SHA-256 비밀번호 해시)
│   ├── account.*            # 계좌 추상 기반 클래스 + Factory Method(fromJson)
│   ├── savingsaccount.*     # 저축계좌 — 잔액 부족 시 예외
│   ├── checkingaccount.*    # 입출금계좌 — 마이너스 한도 내 출금 허용
│   ├── transaction.h        # 거래 내역 구조체
│   ├── userrepository.*     # users.json CRUD (원자적 쓰기)
│   ├── accountrepository.*  # accounts.json CRUD (unique_ptr 다형성 보관)
│   └── sessionmanager.*     # UUID 토큰 ↔ userId 매핑 (메모리)
│
└── Client/          # Qt Widgets GUI 클라이언트 (BankClient)
    ├── main.cpp             # Fusion 스타일·라이트 팔레트·폰트·QSS·서버 연결
    ├── logindialog.*        # 로그인
    ├── registerdialog.*     # 회원가입 (아이디 중복 확인 포함)
    ├── mainwindow.*         # 사이드바 + QStackedWidget 페이지 전환
    ├── homepage.*           # 총 자산 + 계좌별 분포 도넛 차트
    ├── newaccountpage.*     # 신규 계좌 개설
    ├── myinfopage.*         # 내 정보·계좌 목록·비밀번호 변경·계좌 해지
    ├── accountprocesspage.* # 계좌 조회 / 입금·출금
    ├── balancepiechart.*    # 커스텀 도넛 파이차트 위젯 (호버·애니메이션)
    ├── networkclient.*      # TCP 싱글톤 (자동 재연결 + 대기 요청 재전송)
    ├── sessioncontext.*     # 토큰·사용자 정보 전역 싱글톤
    ├── appstyle.h           # 색상 토큰 + QSS 디자인 시스템
    └── resources/           # 사이드바 아이콘 (qrc 리소스)
```

---

## 빌드 및 실행

### 요구 사항
- Qt 6.5 이상 (Core, Widgets, Network, LinguistTools)
- CMake 3.19 이상, C++17 지원 컴파일러

### 서버

```bash
cmake -S Server -B Server/build -DCMAKE_PREFIX_PATH=<Qt6 설치 경로>
```

```bash
cmake --build Server/build
```

```bash
./Server/build/BankServer
```

포트를 바꾸려면 첫 번째 인자로 전달합니다 (기본값 `9999`).

```bash
./Server/build/BankServer 8080
```

실행 파일 위치에 `data/` 폴더가 자동 생성되고 `users.json`, `accounts.json`이 여기에 저장됩니다.

### 클라이언트

```bash
cmake -S Client -B Client/build -DCMAKE_PREFIX_PATH=<Qt6 설치 경로>
```

```bash
cmake --build Client/build
```

```bash
./Client/build/BankClient
```

클라이언트는 시작 시 `127.0.0.1:9999`로 접속합니다 (`Client/main.cpp`에 하드코딩).
다른 주소·포트를 쓰려면 해당 호출을 수정해 다시 빌드해야 합니다.
서버가 꺼져 있어도 클라이언트는 종료되지 않고 1.5초 간격으로 재연결을 시도합니다.

Qt Creator에서는 `Server/CMakeLists.txt`와 `Client/CMakeLists.txt`를 각각 별도 프로젝트로 열어 사용합니다.

---

## 주요 기능

- **회원**: 회원가입(아이디 중복 확인), 로그인/로그아웃, 내 정보 조회, 비밀번호 변경
- **계좌**: 저축·입출금 계좌 개설, 계좌 목록 조회, 상세 조회(거래 내역 포함), 계좌 해지
- **거래**: 입금, 출금(계좌 타입별 규칙 적용), 거래 내역 기록·조회
- **시각화**: 홈 화면의 계좌별 자산 분포 도넛 차트, 내 정보 페이지의 통계 카드

### 계좌 타입별 출금 규칙

| 타입 | 식별자 | 출금 조건 | 추가 필드 |
|---|---|---|---|
| 저축계좌 | `savings` | 잔액 부족 시 `InsufficientFundsException` | `interestRate` (기본 0.03) |
| 입출금계좌 | `checking` | `잔액 - 출금액 >= -한도`이면 허용 | `overdraftLimit` (기본 1,000,000) |

계좌번호는 `110-XXX-XXXXXX` 형식으로 중복 없이 무작위 생성됩니다.

---

## 통신 프로토콜

TCP 스트림 위에서 **Compact JSON 1건 + `\n`**을 하나의 메시지로 취급합니다.
양쪽 모두 수신 버퍼에 데이터를 누적하다가 `\n`을 만날 때마다 한 건씩 파싱합니다.

### 요청 (Client → Server)

```json
{ "type": "deposit", "token": "<UUID>", "data": { "accountNumber": "110-123-456789", "accountPassword": "1234", "amount": 50000, "description": "월급" } }
```

- `token`은 로그인 이후 발급되는 세션 토큰입니다. `check_id`, `register`, `login`을 제외한 모든 요청에 필요합니다.

### 응답 (Server → Client)

```json
{ "type": "deposit_response", "status": "success", "message": "입금 완료: 50000원", "data": { "balance": 150000 } }
```

- 응답 `type`은 항상 요청 `type + "_response"` 입니다.
- `status`는 `success` / `error` / `unauthorized` 중 하나입니다 (`Server/protocol.h`).

### 요청 타입 목록

| type | 인증 | data 주요 필드 | 설명 |
|---|---|---|---|
| `check_id` | – | `id` | 아이디 중복 확인 |
| `register` | – | `id`, `password`, `name` | 회원가입 (id 3자↑, pw 4자↑) |
| `login` | – | `id`, `password` | 로그인 → `token`, `user` 반환 |
| `logout` | ✔ | – | 세션 토큰 폐기 |
| `create_account` | ✔ | `type`, `accountPassword`, `initBalance`, `interestRate`/`overdraftLimit` | 계좌 개설 |
| `list_accounts` | ✔ | – | 내 계좌 요약 목록 (거래 내역 제외) |
| `get_account_detail` | ✔ | `accountNumber` | 거래 내역 포함 상세 조회 |
| `deposit` | ✔ | `accountNumber`, `accountPassword`, `amount`, `description` | 입금 |
| `withdraw` | ✔ | `accountNumber`, `accountPassword`, `amount`, `description` | 출금 |
| `get_user_info` | ✔ | – | 아이디·이름·가입일 조회 |
| `change_password` | ✔ | `currentPassword`, `newPassword` | 로그인 비밀번호 변경 |
| `close_account` | ✔ | `accountNumber`, `accountPassword` | 계좌 해지 |

---

## 설계 메모

- **계층 분리**: `Server` → `ClientHandler`(전송·파싱) → `Bank`(비즈니스 로직 Facade) → `Repository`(영속성) / `SessionManager`(세션).
- **다형성**: `Account`가 추상 기반 클래스이고 `withdraw()`·`getType()`이 순수 가상 함수입니다. `Account::fromJson()`이 `type` 필드를 보고 파생 클래스를 생성하는 Factory Method 역할을 합니다.
- **비밀번호**: 로그인 비밀번호와 계좌 비밀번호 모두 SHA-256 해시로만 저장하며 평문은 보관하지 않습니다.
- **인증 범위**: 조회(`get_account_detail`)는 세션 토큰 + 소유권 확인만 요구하고, 자산이 실제로 움직이는 `deposit`·`withdraw`·`close_account`에서만 계좌 비밀번호를 추가로 검증합니다.
- **영속성**: 변경이 생길 때마다 파일 전체를 다시 쓰며, 임시 파일 → rename 방식의 원자적 쓰기로 손상을 방지합니다.
- **세션**: 토큰–userId 매핑을 메모리(`QHash`)에만 유지하므로 서버를 재시작하면 모든 세션이 만료됩니다.
- **재연결**: 클라이언트는 연결이 끊기면 요청을 보관해두고, 자동 재연결 성공 시 대기 중인 요청을 재전송합니다.

### 알려진 제약

- 통신은 평문 TCP이며 TLS가 적용되어 있지 않습니다. 로컬/학습용 구성입니다.
- 서버 주소와 포트가 클라이언트에 하드코딩되어 있습니다.
- `Server/protocol.h`에 `check_id` 상수가 없고, 클라이언트는 요청 타입을 문자열 리터럴로 직접 사용합니다.
- 자동화된 테스트가 없습니다.

---

## 저장소

- 원격: <https://github.com/VEDA-4th-Project1/GUI-project>
