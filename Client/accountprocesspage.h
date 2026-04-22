#ifndef ACCOUNTPROCESSPAGE_H
#define ACCOUNTPROCESSPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

class QLabel;
class QPushButton;
class QFrame;
class QLineEdit;
class QVBoxLayout;
class QScrollArea;
class QComboBox;
class QRadioButton;
class QDoubleSpinBox;
class QShowEvent;
class QListWidget;
class QListWidgetItem;

/**
 * @class AccountProcessPage
 * @brief 계좌 조회·입출금 처리 페이지.
 *
 * 화면은 좌우 두 패널로 구성된다.
 *
 * [좌측 패널 — 계좌 조회]
 * - 계좌번호 검색 + 드롭다운 결과 목록
 * - 선택된 계좌의 번호·잔액 표시
 * - 해당 계좌의 거래 내역 스크롤 목록 + 총 건수
 *
 * [우측 패널 — 계좌 상세 / 입출금]
 * - 콤보박스로 내 계좌 선택
 * - 계좌번호·타입·잔액 표시
 * - 비밀번호 입력 → 거래 내역 조회 연동
 * - 입금/출금 라디오 + 금액 + 설명 → 실행 버튼
 *
 * 저축계좌(savings)는 출금 라디오 버튼이 비활성화된다.
 * 계좌 상세 조회는 세션 토큰만으로 수행되며, 입금·출금 시에만 계좌 비밀번호가 필요하다.
 */
class AccountProcessPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountProcessPage(QWidget *parent = nullptr);

public slots:
    /** 서버에 "list_accounts" 요청을 전송해 계좌 목록을 갱신한다. */
    void loadAccounts();

protected:
    /** 페이지가 표시될 때마다 loadAccounts()를 호출해 최신 계좌 목록을 로드한다. */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief 우측 계좌 콤보박스 선택 변경 시 호출된다.
     * @param index 선택된 콤보박스 인덱스
     *
     * 선택된 계좌 정보를 updateRightSummary()로 갱신하고,
     * 비밀번호가 입력되어 있으면 계좌 상세(거래 내역)도 함께 조회한다.
     */
    void onDetailAccountChanged(int index);

    /**
     * @brief 실행 버튼 클릭 시 입금 또는 출금 요청을 서버에 전송한다.
     *
     * 유효성 검사: 계좌 미선택·비밀번호 미입력·금액 0 이하·저축계좌 출금 시도.
     */
    void onExecuteClicked();

    /**
     * @brief NetworkClient의 responseReceived 시그널에 연결되는 슬롯.
     *
     * 처리 타입:
     * - list_accounts_response: 계좌 목록 갱신, 콤보박스·좌측·우측 요약 업데이트
     * - get_account_detail_response: 비밀번호 캐시 저장, 잔액·거래 내역 업데이트
     * - deposit_response / withdraw_response: 잔액 갱신, 성공 메시지, 화면 리프레시
     */
    void onResponseReceived(const QJsonObject &response);

    /**
     * @brief 출금 라디오 버튼 클릭 시 저축계좌 여부를 확인한다.
     *
     * 저축계좌면 출금 불가 경고를 표시하고 입금 라디오로 되돌린다.
     */
    void onWithdrawRadioClicked(bool checked);

    /**
     * @brief 좌측 검색 필드 텍스트 변경 시 fillSearchResults()를 호출한다.
     * @param text 현재 입력된 검색 텍스트
     */
    void onSearchTextChanged(const QString &text);

    /**
     * @brief 검색 결과 목록에서 항목 클릭 시 좌측 계좌를 선택한다.
     *
     * 선택된 계좌 정보로 updateLeftSummary()를 호출하고
     * 캐시된 비밀번호가 있으면 거래 내역도 즉시 조회한다.
     */
    void onSearchResultClicked(QListWidgetItem *item);

private:
    /** setupLeftPanel()·setupRightPanel()을 호출해 전체 UI를 구성한다. */
    void setupUI();

    /** 좌측 패널(계좌 검색·잔액·거래 내역)을 구성한다. */
    void setupLeftPanel();

    /** 우측 패널(계좌 콤보·입출금 폼)을 구성한다. */
    void setupRightPanel();

    /**
     * @brief 서버에 "get_account_detail" 요청을 전송한다. (비밀번호 불필요)
     * @param accountNumber 조회할 계좌번호
     */
    void loadAccountDetail(const QString &accountNumber);

    /** m_historyLayout에서 모든 거래 내역 위젯을 제거하고 m_historyCount를 0으로 초기화한다. */
    void clearHistoryLabels();

    /**
     * @brief 거래 내역 한 건을 m_historyLayout에 라벨로 추가한다.
     * @param type          거래 종류 ("deposit" / "withdraw")
     * @param accountNumber 계좌번호
     * @param text          "금액원 / 설명" 형식 텍스트
     */
    void addHistoryLabel(const QString &type, const QString &accountNumber, const QString &text);

    /** m_historyCount를 기반으로 총 거래 건수 라벨을 갱신한다. */
    void updateTotalLabel();

    /**
     * @brief m_currentTransactions 배열로 거래 내역 라벨을 전부 재구성한다.
     * @param transactions 서버에서 수신한 거래 내역 JSON 배열
     */
    void renderHistoryLabels(const QJsonArray &transactions);

    /** renderHistoryLabels(m_currentTransactions)를 호출해 현재 필터를 재적용한다. */
    void applyHistoryFilter();

    /**
     * @brief keyword를 포함하는 계좌번호를 m_allAccounts에서 검색해 m_searchResultList를 채운다.
     * @param keyword 검색 키워드 (대소문자 무시)
     *
     * 키워드가 비어 있으면 목록을 숨긴다.
     */
    void fillSearchResults(const QString &keyword);

    /**
     * @brief 좌측 패널의 계좌번호·잔액 라벨을 acc JSON으로 갱신한다.
     * @param acc 계좌 JSON 객체 (accountNumber, balance 포함)
     */
    void updateLeftSummary(const QJsonObject &acc);

    /**
     * @brief 우측 패널의 계좌번호·타입·잔액 라벨과 출금 버튼 가용성을 acc JSON으로 갱신한다.
     * @param acc 계좌 JSON 객체 (accountNumber, type, balance 포함)
     *
     * 저축계좌(savings)면 m_withdrawRadio를 비활성화한다.
     */
    void updateRightSummary(const QJsonObject &acc);

private:
    QFrame *m_leftCard;   ///< 좌측 패널 카드 프레임
    QFrame *m_rightCard;  ///< 우측 패널 카드 프레임

    // ── 좌측 패널 위젯 ─────────────────────────────────────────────────────
    QLabel      *m_titleLabel;           ///< "계좌 조회" 제목 라벨
    QLineEdit   *m_searchEdit;           ///< 계좌번호 검색 입력 필드
    QListWidget *m_searchResultList;     ///< 검색 결과 드롭다운 목록 (평소 숨김)
    QLabel      *m_accountNumberLabel;   ///< 선택된 계좌번호 표시 라벨
    QLabel      *m_balanceLabel;         ///< 선택된 계좌 잔액 표시 라벨

    QWidget     *m_historyContainer;     ///< 거래 내역 위젯들을 담는 컨테이너
    QVBoxLayout *m_historyLayout;        ///< 거래 내역 컨테이너의 레이아웃
    QLabel      *m_totalLabel;           ///< 총 거래 건수 표시 라벨

    // ── 우측 패널 위젯 ─────────────────────────────────────────────────────
    QLabel        *m_detailTitleLabel;      ///< "계좌 상세 / 입출금" 제목 라벨
    QComboBox     *m_detailAccountCombo;    ///< 내 계좌 선택 콤보박스
    QLabel        *m_selectedAccountLabel;  ///< 선택 계좌번호 표시 라벨
    QLabel        *m_selectedTypeLabel;     ///< 선택 계좌 타입 표시 라벨
    QLabel        *m_selectedBalanceLabel;  ///< 선택 계좌 잔액 표시 라벨

    QLineEdit     *m_accountPasswordEdit;   ///< 계좌 비밀번호 입력 필드
    QRadioButton  *m_depositRadio;          ///< 입금 선택 라디오 버튼
    QRadioButton  *m_withdrawRadio;         ///< 출금 선택 라디오 버튼 (저축계좌 시 비활성)
    QDoubleSpinBox *m_amountSpin;           ///< 거래 금액 입력 스핀박스
    QLineEdit     *m_descriptionEdit;       ///< 거래 설명 입력 필드
    QPushButton   *m_executeBtn;            ///< 입출금 실행 버튼

    // ── 상태 변수 ───────────────────────────────────────────────────────────
    QString m_leftAccountNumber;  ///< 현재 좌측 패널에 선택된 계좌번호
    int     m_leftBalance;        ///< 현재 좌측 패널 계좌의 잔액

    QString m_rightAccountNumber; ///< 현재 우측 패널에 선택된 계좌번호
    QString m_rightAccountType;   ///< 현재 우측 패널 계좌의 타입 ("savings"/"checking")
    int     m_rightBalance;       ///< 현재 우측 패널 계좌의 잔액

    int        m_historyCount;         ///< 현재 표시 중인 거래 내역 건수
    QJsonArray m_allAccounts;          ///< 서버에서 수신한 전체 계좌 목록
    QJsonArray m_currentTransactions;  ///< 좌측 선택 계좌의 현재 거래 내역
};

#endif // ACCOUNTPROCESSPAGE_H
