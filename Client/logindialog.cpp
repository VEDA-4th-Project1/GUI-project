#include "logindialog.h"
#include "registerdialog.h"
#include "mainwindow.h"
#include "networkclient.h"
#include "sessioncontext.h"
#include "appstyle.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QJsonObject>

/** setupUI/applyStyles를 호출하고 NetworkClient 시그널을 연결한다. */
LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
    setWindowTitle(tr("로그인"));
    setFixedSize(460, 520);

    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onNetworkResponse(QJsonObject)));
    connect(NetworkClient::instance(), SIGNAL(errorOccurred(QString)),
            this, SLOT(onNetworkError(QString)));
}

LoginDialog::~LoginDialog() = default;

/** 카드 프레임 → 제목/부제목 → 아이디/비밀번호 입력 → 버튼 순으로 위젯을 배치한다. */
void LoginDialog::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setAlignment(Qt::AlignCenter);
    rootLayout->setContentsMargins(40, 40, 40, 40);

    m_loginCard = new QFrame(this);
    m_loginCard->setObjectName("loginCard");
    m_loginCard->setFixedWidth(360);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_loginCard);
    shadow->setBlurRadius(32);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 70));
    m_loginCard->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(m_loginCard);
    cardLayout->setSpacing(12);
    cardLayout->setContentsMargins(32, 36, 32, 32);

    m_titleLabel = new QLabel(tr("환영합니다"), m_loginCard);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_subtitleLabel = new QLabel(tr("계정에 로그인하세요"), m_loginCard);
    m_subtitleLabel->setObjectName("subtitleLabel");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    QFrame *divider = new QFrame(m_loginCard);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("divider");

    m_idLabel = new QLabel(tr("아이디"), m_loginCard);
    m_idLabel->setObjectName("fieldLabel");

    m_idEdit = new QLineEdit(m_loginCard);
    m_idEdit->setObjectName("inputField");
    m_idEdit->setPlaceholderText(tr("아이디를 입력하세요"));
    m_idEdit->setFixedHeight(44);

    m_pwLabel = new QLabel(tr("비밀번호"), m_loginCard);
    m_pwLabel->setObjectName("fieldLabel");

    m_pwEdit = new QLineEdit(m_loginCard);
    m_pwEdit->setObjectName("inputField");
    m_pwEdit->setPlaceholderText(tr("비밀번호를 입력하세요"));
    m_pwEdit->setEchoMode(QLineEdit::Password);
    m_pwEdit->setFixedHeight(44);

    m_registerBtn = new QPushButton(tr("회원가입"), m_loginCard);
    m_registerBtn->setObjectName("registerBtn");
    m_registerBtn->setFixedHeight(44);
    m_registerBtn->setAutoDefault(false);
    m_registerBtn->setDefault(false);

    m_confirmBtn = new QPushButton(tr("확인"), m_loginCard);
    m_confirmBtn->setObjectName("confirmBtn");
    m_confirmBtn->setFixedHeight(48);
    m_confirmBtn->setAutoDefault(true);
    m_confirmBtn->setDefault(true);

    m_cancelBtn = new QPushButton(tr("취소"), m_loginCard);
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedHeight(48);
    m_cancelBtn->setAutoDefault(false);
    m_cancelBtn->setDefault(false);

    QHBoxLayout *actionRow = new QHBoxLayout();
    actionRow->setSpacing(10);
    actionRow->addWidget(m_confirmBtn);
    actionRow->addWidget(m_cancelBtn);

    cardLayout->addWidget(m_titleLabel);
    cardLayout->addWidget(m_subtitleLabel);
    cardLayout->addSpacing(2);
    cardLayout->addWidget(divider);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_idLabel);
    cardLayout->addWidget(m_idEdit);
    cardLayout->addWidget(m_pwLabel);
    cardLayout->addWidget(m_pwEdit);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_registerBtn);
    cardLayout->addSpacing(4);
    cardLayout->addLayout(actionRow);

    rootLayout->addWidget(m_loginCard);

    connect(m_registerBtn, SIGNAL(clicked()), this, SLOT(onRegisterClicked()));
    connect(m_confirmBtn, SIGNAL(clicked()), this, SLOT(onConfirmClicked()));
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
}

/** objectName 기반 QSS로 각 위젯의 색상·폰트·모서리를 지정한다. */
void LoginDialog::applyStyles()
{
    setStyleSheet(AppStyle::DARK_DIALOG_BG);

    m_loginCard->setStyleSheet(
        "#loginCard { " + AppStyle::DARK_CARD_BODY + " }"
        "#titleLabel { color: #191F28; font-size: 26px; font-weight: 800; }"
        "#subtitleLabel { color: #8B95A1; font-size: 13px; }"
        "QFrame#divider { color: #E5E8EB; background-color: #E5E8EB; max-height: 1px; }"
        + AppStyle::DARK_FIELD_LABEL
        + AppStyle::DARK_INPUT
        + "QPushButton#registerBtn {"
          "  background-color: #F2F4F6; color: #4E5968; border: 1px solid #E5E8EB;"
          "  border-radius: 10px; font-size: 14px; font-weight: 600; padding: 0 12px;"
          "}"
          "QPushButton#registerBtn:hover { background-color: #E5E8EB; color: #191F28; }"
          "QPushButton#confirmBtn {"
          "  background-color: #3182F6; color: white; border: none;"
          "  border-radius: 12px; font-size: 15px; font-weight: 700;"
          "}"
          "QPushButton#confirmBtn:hover { background-color: #1B64DA; }"
          "QPushButton#confirmBtn:pressed { background-color: #1B56BC; }"
          "QPushButton#confirmBtn:disabled { background-color: #B0C8F0; }"
          "QPushButton#cancelBtn {"
          "  background-color: #F2F4F6; color: #4E5968; border: 1px solid #E5E8EB;"
          "  border-radius: 12px; font-size: 15px; font-weight: 600;"
          "}"
          "QPushButton#cancelBtn:hover { background-color: #E5E8EB; }"
    );
}

/** RegisterDialog를 모달로 열고 완료되면 반환한다. */
void LoginDialog::onRegisterClicked()
{
    RegisterDialog dlg(this);
    dlg.exec();
}

/**
 * 아이디·비밀번호 공백 검증 후 서버에 "login" 요청을 전송한다.
 * 요청 중 확인 버튼을 비활성화해 중복 요청을 방지한다.
 */
void LoginDialog::onConfirmClicked()
{
    const QString id = m_idEdit->text().trimmed();
    const QString pw = m_pwEdit->text();

    if (id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, tr("입력 오류"),
                             tr("아이디와 비밀번호를 입력해주세요."));
        return;
    }

    m_confirmBtn->setEnabled(false);
    QJsonObject data;
    data["id"]       = id;
    data["password"] = pw;
    QJsonObject req;
    req["type"] = "login";
    req["data"] = data;
    NetworkClient::instance()->sendRequest(req);
}

/**
 * "login_response" 타입만 처리한다.
 * 성공 시: 시그널 연결을 끊어 중복 처리를 방지하고, 세션을 저장한 뒤 MainWindow를 연다.
 * 실패 시: 경고 다이얼로그를 표시하고 확인 버튼을 다시 활성화한다.
 */
void LoginDialog::onNetworkResponse(const QJsonObject& resp)
{
    if (resp["type"].toString() != "login_response") return;

    m_confirmBtn->setEnabled(true);

    if (resp["status"].toString() == "success") {
        // 이후 LoginDialog가 다시 응답을 처리하지 않도록 연결 해제
        disconnect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
                   this, SLOT(onNetworkResponse(QJsonObject)));

        QJsonObject respData = resp["data"].toObject();
        QJsonObject user     = respData["user"].toObject();

        SessionContext::instance().setSession(
            respData["token"].toString(),
            user["id"].toString(),
            user["name"].toString()
        );

        MainWindow *mainWindow = new MainWindow();
        mainWindow->show();
        accept();
    } else {
        QMessageBox::warning(this, tr("로그인 실패"), resp["message"].toString());
    }
}

/** 네트워크 오류 메시지를 표시하고 확인 버튼을 다시 활성화한다. */
void LoginDialog::onNetworkError(const QString& message)
{
    QMessageBox::critical(this, tr("네트워크 오류"), message);
    m_confirmBtn->setEnabled(true);
}

/** 다이얼로그를 거부(reject)하여 닫는다. */
void LoginDialog::onCancelClicked()
{
    reject();
}
