#ifndef NEWACCOUNTPAGE_H
#define NEWACCOUNTPAGE_H

#include <QWidget>
#include <QJsonObject>

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;

/**
 * @class NewAccountPage
 * @brief 신규 계좌 개설 페이지.
 *
 * 계좌 종류(저축/입출금)·비밀번호·초기 입금액을 입력받아
 * 서버에 "create_account" 요청을 전송한다.
 *
 * - 저축(savings): 이자율 연 3.0% 고정 표시, 마이너스 한도 행 숨김.
 * - 입출금(checking): 마이너스 한도 0원 고정 표시, 이자율 행 숨김.
 * 계좌 종류 변경 시 onTypeChanged()가 두 위젯의 가시성을 전환한다.
 */
class NewAccountPage : public QWidget {
    Q_OBJECT

public:
    explicit NewAccountPage(QWidget* parent = nullptr);

private slots:
    /**
     * @brief 계좌 개설 버튼 클릭 시 호출된다.
     *
     * 비밀번호 4자 미만이면 경고 후 중단한다.
     * 유효성 통과 시 서버에 "create_account" 요청을 전송한다.
     */
    void onCreateClicked();

    /**
     * @brief 계좌 종류 콤보박스 변경 시 호출된다.
     * @param index 선택된 인덱스 (0: savings, 1: checking)
     *
     * m_rateWidget와 m_limitWidget의 가시성을 계좌 종류에 따라 전환한다.
     */
    void onTypeChanged(int index);

    /**
     * @brief NetworkClient의 responseReceived 시그널에 연결되는 슬롯.
     *
     * "create_account_response" 타입만 처리한다.
     * 성공 시 발급된 계좌번호를 안내 메시지로 표시하고 입력 필드를 초기화한다.
     */
    void onNetworkResponse(const QJsonObject& response);

private:
    /** 페이지 레이아웃 및 모든 위젯을 생성하고 배치한다. */
    void setupUI();

    QComboBox*      m_typeCombo;        ///< 계좌 종류 선택 콤보박스 (savings/checking)
    QLineEdit*      m_accPasswordEdit;  ///< 계좌 비밀번호 입력 필드 (4자 이상)
    QDoubleSpinBox* m_initBalanceSpin;  ///< 초기 입금액 스핀박스 (0원 ~ 999,999,999원)
    QWidget*        m_rateWidget;       ///< 이자율 표시 행 — 저축계좌 선택 시에만 표시
    QWidget*        m_limitWidget;      ///< 마이너스 한도 표시 행 — 입출금계좌 선택 시에만 표시
    QPushButton*    m_createBtn;        ///< 계좌 개설 버튼 — 요청 중 비활성화
};

#endif // NEWACCOUNTPAGE_H
