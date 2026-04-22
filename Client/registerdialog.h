#ifndef RESISTERDIALOG_H
#define RESISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QDateEdit>
#include <QJsonObject>

/**
 * @class RegisterDialog
 * @brief 회원가입 다이얼로그.
 *
 * 이름·생년월일·아이디(중복 확인 포함)·비밀번호·비밀번호 확인을 입력받아
 * 서버에 "register" 요청을 전송한다.
 * 아이디 중복 확인 전에 가입 완료를 시도하면 경고를 표시한다.
 */
class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);

private slots:
    /**
     * @brief 중복 확인 버튼 클릭 시 서버에 "check_id" 요청을 전송한다.
     *
     * 아이디가 비어있거나 3자 미만이면 경고를 표시하고 전송하지 않는다.
     */
    void onDuplicateCheckClicked();

    /**
     * @brief 가입 완료 버튼 클릭 시 입력값을 검증하고 "register" 요청을 전송한다.
     *
     * validateInputs()가 실패하면 전송하지 않는다.
     */
    void onConfirmClicked();

    /** 취소 버튼 클릭 시 다이얼로그를 거부(reject)하고 닫는다. */
    void onCancelClicked();

    /**
     * @brief 아이디 입력 텍스트가 변경될 때마다 중복 확인 상태를 초기화한다.
     *
     * m_idChecked를 false로 되돌려 새로 입력된 아이디로 다시 확인하도록 유도한다.
     */
    void onIdTextChanged();

    /**
     * @brief NetworkClient의 responseReceived 시그널에 연결되는 슬롯.
     *
     * "check_id_response": 중복 여부에 따라 아이디 테두리 색상을 변경하고 m_idChecked를 설정.
     * "register_response": 성공 시 다이얼로그를 accept()로 닫음.
     */
    void onNetworkResponse(const QJsonObject& response);

    /** 네트워크 오류 발생 시 메시지를 표시하고 버튼들을 다시 활성화한다. */
    void onNetworkError(const QString& message);

private:
    /** UI 위젯을 생성하고 레이아웃을 구성한다. */
    void setupUI();

    /** objectName 기반으로 QSS 스타일을 적용한다. */
    void applyStyles();

    /**
     * @brief 모든 입력 필드의 유효성을 검사한다.
     * @return 모든 조건을 만족하면 true, 하나라도 실패하면 false.
     *
     * 검사 항목: 이름 공백, 생년월일 미래 여부, 아이디 공백,
     *             중복 확인 완료 여부, 비밀번호 6자 이상, 비밀번호 일치.
     */
    bool validateInputs();

    /**
     * @brief 아이디 입력 필드의 테두리 색상을 변경한다.
     * @param color CSS 색상 문자열 (예: "#00C896" 성공, "#F04452" 실패)
     */
    void setIdBorderColor(const QString &color);

    QFrame      *m_card;              ///< 폼을 감싸는 카드 프레임

    QLabel      *m_titleLabel;        ///< "회원가입" 제목 라벨

    QLabel      *m_nameLabel;         ///< 이름 필드 라벨
    QLineEdit   *m_nameEdit;          ///< 이름 입력 필드

    QLabel      *m_birthLabel;        ///< 생년월일 필드 라벨
    QDateEdit   *m_birthEdit;         ///< 생년월일 날짜 선택 위젯 (달력 팝업 포함)

    QLabel      *m_idLabel;           ///< 아이디 필드 라벨
    QLineEdit   *m_idEdit;            ///< 아이디 입력 필드 (테두리 색으로 중복 결과 표시)
    QPushButton *m_dupCheckBtn;       ///< 중복 확인 버튼 — 요청 중 비활성화

    QLabel      *m_pwLabel;           ///< 비밀번호 필드 라벨
    QLineEdit   *m_pwEdit;            ///< 비밀번호 입력 필드 (EchoMode::Password)

    QLabel      *m_pwConfirmLabel;    ///< 비밀번호 확인 필드 라벨
    QLineEdit   *m_pwConfirmEdit;     ///< 비밀번호 확인 입력 필드

    QPushButton *m_confirmBtn;        ///< 가입 완료 버튼 — 요청 중 비활성화
    QPushButton *m_cancelBtn;         ///< 취소 버튼

    bool         m_idChecked = false; ///< 아이디 중복 확인 완료 여부 (가입 전 필수)
};

#endif // RESISTERDIALOG_H
