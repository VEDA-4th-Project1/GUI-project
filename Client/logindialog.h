#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QJsonObject>

/**
 * @class LoginDialog
 * @brief 앱 시작 시 표시되는 로그인 다이얼로그.
 *
 * 사용자 아이디·비밀번호를 입력받아 서버에 "login" 요청을 전송한다.
 * 로그인 성공 시 SessionContext에 토큰·사용자 정보를 저장하고 MainWindow를 열며,
 * 이 다이얼로그는 닫힌다. 회원가입 버튼을 누르면 RegisterDialog를 모달로 연다.
 */
class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    /** 회원가입 버튼 클릭 시 RegisterDialog를 모달로 띄운다. */
    void onRegisterClicked();

    /** 확인 버튼 클릭 시 입력값을 검증하고 서버에 로그인 요청을 전송한다. */
    void onConfirmClicked();

    /** 취소 버튼 클릭 시 다이얼로그를 거부(reject)하고 닫는다. */
    void onCancelClicked();

    /**
     * @brief NetworkClient의 responseReceived 시그널에 연결되는 슬롯.
     *
     * "login_response" 타입만 처리하며, 성공 시 세션을 저장하고 MainWindow를 연다.
     * 이후 이 슬롯이 중복 호출되지 않도록 disconnect한다.
     */
    void onNetworkResponse(const QJsonObject& response);

    /** 네트워크 오류 발생 시 오류 메시지를 표시하고 확인 버튼을 다시 활성화한다. */
    void onNetworkError(const QString& message);

private:
    /** UI 위젯을 생성하고 레이아웃을 구성한다. */
    void setupUI();

    /** objectName 기반으로 QSS 스타일을 적용한다. */
    void applyStyles();

    QFrame      *m_loginCard;      ///< 로그인 폼을 감싸는 흰색 카드 프레임

    QLabel      *m_titleLabel;     ///< "환영합니다" 제목 라벨
    QLabel      *m_subtitleLabel;  ///< "계정에 로그인하세요" 부제목 라벨
    QLabel      *m_idLabel;        ///< 아이디 필드 라벨
    QLabel      *m_pwLabel;        ///< 비밀번호 필드 라벨

    QLineEdit   *m_idEdit;         ///< 아이디 입력 필드
    QLineEdit   *m_pwEdit;         ///< 비밀번호 입력 필드 (EchoMode::Password)

    QPushButton *m_registerBtn;    ///< 회원가입 버튼
    QPushButton *m_confirmBtn;     ///< 확인(로그인) 버튼 — 요청 중 비활성화
    QPushButton *m_cancelBtn;      ///< 취소 버튼
};

#endif // LOGINDIALOG_H
