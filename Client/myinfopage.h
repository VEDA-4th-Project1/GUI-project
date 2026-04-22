#ifndef MYINFOPAGE_H
#define MYINFOPAGE_H

#include <QWidget>
#include <QLabel>
#include <QJsonObject>

class QDialog;
class QLineEdit;
class QComboBox;
class QPushButton;

/**
 * @class MyInfoPage
 * @brief 사용자 기본 정보·계좌 목록·기능 버튼을 표시하는 내 정보 페이지.
 *
 * 구성:
 * - 프로필 헤더 카드: 아바타(이름 첫 글자)·이름·소개 문구 (그라디언트 배경)
 * - 통계 카드 3개: 보유 계좌 수 / 총 자산 / 가입 경과일
 * - 기본 정보 카드: 아이디·이름·가입일 + 비밀번호 변경·계좌 해지 버튼
 * - 내 계좌 카드: 계좌별 타입 배지·번호·잔액 행 목록
 *
 * MainWindow에서 페이지 전환 시 loadUserInfo()를 명시적으로 호출해
 * 최신 데이터를 로드한다.
 */
class MyInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyInfoPage(QWidget *parent = nullptr);

    /**
     * @brief 서버에 "get_user_info"와 "list_accounts" 요청을 연속으로 전송한다.
     *
     * 두 요청은 각각의 응답이 수신되면 onNetworkResponse에서 처리된다.
     */
    void loadUserInfo();

private slots:
    /**
     * @brief NetworkClient의 responseReceived 시그널에 연결되는 슬롯.
     *
     * 처리 타입:
     * - get_user_info_response: 아이디·이름·가입일·경과일 갱신
     * - list_accounts_response: 계좌 목록·총 자산·보유 계좌 수 갱신
     * - change_password_response: 성공/실패 메시지 표시
     * - close_account_response: 성공 시 다이얼로그 닫기 + 정보 재로드
     */
    void onNetworkResponse(const QJsonObject &resp);

    /**
     * @brief 비밀번호 변경 다이얼로그를 생성하고 모달로 실행한다.
     *
     * 현재 비밀번호·새 비밀번호·새 비밀번호 확인 필드를 포함한다.
     */
    void openChangePasswordDialog();

    /**
     * @brief 계좌 해지 다이얼로그를 생성하고 모달로 실행한다.
     *
     * 보유 계좌가 없으면 알림 메시지만 표시한다.
     * 다이얼로그 내부에서 해지 요청이 전송되고
     * 응답은 onNetworkResponse의 "close_account_response"에서 처리된다.
     */
    void openCloseAccountDialog();

    /**
     * @brief 비밀번호 변경 다이얼로그의 확인 버튼 클릭 슬롯.
     *
     * 빈 값·6자 미만·불일치·현재=새 비밀번호 등을 검증하고
     * 통과 시 서버에 "change_password" 요청을 전송한다.
     */
    void onChangePwOkClicked();

    /**
     * @brief 계좌 해지 다이얼로그의 해지 버튼 클릭 슬롯.
     *
     * 비밀번호 공백 확인·확인 질문 후 서버에 "close_account" 요청을 전송한다.
     * 응답을 기다리는 동안 해지 버튼을 비활성화한다.
     */
    void onCloseAccOkClicked();

private:
    /** 스크롤 영역 안에 헤더·통계·기본 정보·계좌 목록 카드를 배치한다. */
    void setupUI();

    // ── 기본 정보 카드 라벨 ────────────────────────────────────────────────
    QLabel *m_idLabel;          ///< 아이디 값 표시 라벨
    QLabel *m_nameLabel;        ///< 이름 값 표시 라벨
    QLabel *m_createdAtLabel;   ///< 가입일 값 표시 라벨

    // ── 프로필 헤더 위젯 ───────────────────────────────────────────────────
    QLabel *m_avatarLabel      = nullptr;  ///< 이름 첫 글자를 표시하는 원형 아바타 라벨
    QLabel *m_headerNameLabel  = nullptr;  ///< 헤더에 표시되는 "OOO님" 라벨

    // ── 통계 카드 값 라벨 ──────────────────────────────────────────────────
    QLabel *m_statAccountsVal  = nullptr;  ///< "보유 계좌" 수 값 라벨
    QLabel *m_statTotalVal     = nullptr;  ///< "총 자산" 값 라벨
    QLabel *m_statDaysVal      = nullptr;  ///< "가입 경과일" 값 라벨

    // ── 계좌 목록 컨테이너 ─────────────────────────────────────────────────
    QWidget *m_accountsListBox = nullptr;  ///< 계좌별 행 위젯을 담는 컨테이너 (동적 재구성)

    QStringList m_accountList;     ///< 계좌 해지 콤보박스에 사용되는 계좌 목록 ("번호 (타입)" 형식)
    double      m_totalBalance = 0.0;  ///< 모든 계좌의 잔액 합산값

    // ── 비밀번호 변경 다이얼로그 멤버 ─────────────────────────────────────
    QDialog   *m_changePwDlg   = nullptr;  ///< 비밀번호 변경 다이얼로그 (열려 있을 때만 유효)
    QLineEdit *m_currentPwEdit = nullptr;  ///< 현재 비밀번호 입력 필드
    QLineEdit *m_newPwEdit     = nullptr;  ///< 새 비밀번호 입력 필드
    QLineEdit *m_confirmPwEdit = nullptr;  ///< 새 비밀번호 확인 입력 필드

    // ── 계좌 해지 다이얼로그 멤버 ─────────────────────────────────────────
    QDialog     *m_closeAccDlg    = nullptr;  ///< 계좌 해지 다이얼로그 (열려 있을 때만 유효)
    QComboBox   *m_closeAccCombo  = nullptr;  ///< 해지할 계좌 선택 콤보박스
    QLineEdit   *m_closeAccPwEdit = nullptr;  ///< 계좌 비밀번호 입력 필드
    QPushButton *m_closeAccOkBtn  = nullptr;  ///< 해지 확인 버튼 — 요청 중 비활성화
};

#endif // MYINFOPAGE_H
