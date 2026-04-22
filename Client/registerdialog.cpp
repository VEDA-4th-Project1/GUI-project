#include "registerdialog.h"
#include "networkclient.h"
#include "appstyle.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>
#include <QRegularExpression>
#include <QCalendarWidget>
#include <QDate>
#include <QJsonObject>

/** setupUI/applyStyles를 호출하고 NetworkClient 시그널을 연결한다. */
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    applyStyles();
    setWindowTitle(tr("회원가입"));
    setFixedSize(480, 590);
    setModal(true);

    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onNetworkResponse(QJsonObject)));
    connect(NetworkClient::instance(), SIGNAL(errorOccurred(QString)),
            this, SLOT(onNetworkError(QString)));
}

/**
 * 카드 프레임 안에 이름·생년월일·아이디(+중복확인)·
 * 비밀번호·비밀번호 확인·버튼 순으로 위젯을 배치한다.
 * 생년월일은 QDateEdit + QCalendarWidget 팝업으로 구현한다.
 */
void RegisterDialog::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(28, 28, 28, 28);

    // ── 카드 ─────────────────────────────────────────────────────────────────
    m_card = new QFrame(this);
    m_card->setObjectName("regCard");

    auto *shadow = new QGraphicsDropShadowEffect(m_card);
    shadow->setBlurRadius(28);
    shadow->setOffset(0, 6);
    shadow->setColor(QColor(0, 0, 0, 80));
    m_card->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(m_card);
    cardLayout->setSpacing(10);
    cardLayout->setContentsMargins(32, 32, 32, 32);

    // ── 타이틀 ───────────────────────────────────────────────────────────────
    m_titleLabel = new QLabel(tr("회원가입"), m_card);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(m_titleLabel);
    cardLayout->addSpacing(6);

    // ── 이름 ─────────────────────────────────────────────────────────────────
    m_nameLabel = new QLabel(tr("이름"), m_card);
    m_nameLabel->setObjectName("fieldLabel");
    m_nameEdit  = new QLineEdit(m_card);
    m_nameEdit->setObjectName("inputField");
    m_nameEdit->setPlaceholderText(tr("이름을 입력하세요"));
    m_nameEdit->setFixedHeight(42);
    cardLayout->addWidget(m_nameLabel);
    cardLayout->addWidget(m_nameEdit);

    // ── 생년월일 (달력 팝업) ──────────────────────────────────────────────────
    m_birthLabel = new QLabel(tr("생년월일"), m_card);
    m_birthLabel->setObjectName("fieldLabel");

    m_birthEdit = new QDateEdit(m_card);
    m_birthEdit->setObjectName("dateField");
    m_birthEdit->setFixedHeight(42);
    m_birthEdit->setDisplayFormat("yyyy-MM-dd");
    m_birthEdit->setDate(QDate(2000, 1, 1));
    m_birthEdit->setMinimumDate(QDate(1900, 1, 1));
    m_birthEdit->setMaximumDate(QDate::currentDate());  // 미래 날짜 선택 불가
    m_birthEdit->setCalendarPopup(true);

    // 달력 위젯 라이트 테마 커스터마이징
    QCalendarWidget *calendar = m_birthEdit->calendarWidget();
    calendar->setGridVisible(false);
    calendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    calendar->setStyleSheet(R"(
        QCalendarWidget {
            background-color: #FFFFFF;
            color: #191F28;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
            font-size: 12px;
        }
        QCalendarWidget QToolButton {
            background-color: #F2F4F6;
            color: #191F28;
            border: none;
            border-radius: 6px;
            padding: 4px 10px;
            font-size: 12px;
            font-weight: 600;
        }
        QCalendarWidget QToolButton:hover {
            background-color: #3182F6;
            color: white;
        }
        QCalendarWidget QToolButton::menu-indicator { image: none; }
        QCalendarWidget QSpinBox {
            background-color: #F9FAFB;
            color: #191F28;
            border: 1px solid #E5E8EB;
            border-radius: 4px;
            padding: 2px 6px;
            font-size: 12px;
        }
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #F9FAFB;
            padding: 6px;
            border-bottom: 1px solid #E5E8EB;
        }
        QCalendarWidget QAbstractItemView {
            background-color: #FFFFFF;
            color: #191F28;
            selection-background-color: #3182F6;
            selection-color: #FFFFFF;
            gridline-color: #F2F4F6;
            font-size: 12px;
        }
        QCalendarWidget QAbstractItemView:disabled { color: #B0B8C1; }
        QCalendarWidget QMenu {
            background-color: #FFFFFF;
            color: #191F28;
            border: 1px solid #E5E8EB;
        }
        QCalendarWidget QMenu::item:selected { background-color: #3182F6; color: white; }
    )");

    cardLayout->addWidget(m_birthLabel);
    cardLayout->addWidget(m_birthEdit);

    // ── 아이디 + 중복 확인 ───────────────────────────────────────────────────
    m_idLabel = new QLabel(tr("아이디"), m_card);
    m_idLabel->setObjectName("fieldLabel");

    auto *idRow = new QHBoxLayout();
    idRow->setSpacing(8);

    m_idEdit = new QLineEdit(m_card);
    m_idEdit->setObjectName("inputField");
    m_idEdit->setPlaceholderText(tr("아이디를 입력하세요"));
    m_idEdit->setFixedHeight(42);

    m_dupCheckBtn = new QPushButton(tr("중복 확인"), m_card);
    m_dupCheckBtn->setObjectName("dupCheckBtn");
    m_dupCheckBtn->setFixedSize(88, 42);
    m_dupCheckBtn->setCursor(Qt::PointingHandCursor);

    idRow->addWidget(m_idEdit);
    idRow->addWidget(m_dupCheckBtn);

    cardLayout->addWidget(m_idLabel);
    cardLayout->addLayout(idRow);

    // ── 비밀번호 ─────────────────────────────────────────────────────────────
    m_pwLabel = new QLabel(tr("비밀번호"), m_card);
    m_pwLabel->setObjectName("fieldLabel");
    m_pwEdit  = new QLineEdit(m_card);
    m_pwEdit->setObjectName("inputField");
    m_pwEdit->setPlaceholderText(tr("비밀번호 (6자 이상)"));
    m_pwEdit->setEchoMode(QLineEdit::Password);
    m_pwEdit->setFixedHeight(42);
    cardLayout->addWidget(m_pwLabel);
    cardLayout->addWidget(m_pwEdit);

    // ── 비밀번호 확인 ─────────────────────────────────────────────────────────
    m_pwConfirmLabel = new QLabel(tr("비밀번호 확인"), m_card);
    m_pwConfirmLabel->setObjectName("fieldLabel");
    m_pwConfirmEdit  = new QLineEdit(m_card);
    m_pwConfirmEdit->setObjectName("inputField");
    m_pwConfirmEdit->setPlaceholderText(tr("비밀번호를 다시 입력하세요"));
    m_pwConfirmEdit->setEchoMode(QLineEdit::Password);
    m_pwConfirmEdit->setFixedHeight(42);
    cardLayout->addWidget(m_pwConfirmLabel);
    cardLayout->addWidget(m_pwConfirmEdit);

    cardLayout->addSpacing(6);

    // ── 버튼 행 ─────────────────────────────────────────────────────────────
    auto *btnRow = new QHBoxLayout();
    btnRow->setSpacing(10);

    m_confirmBtn = new QPushButton(tr("O  가입 완료"), m_card);
    m_confirmBtn->setObjectName("confirmBtn");
    m_confirmBtn->setFixedHeight(46);
    m_confirmBtn->setCursor(Qt::PointingHandCursor);

    m_cancelBtn = new QPushButton(tr("X  취소"), m_card);
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedHeight(46);
    m_cancelBtn->setCursor(Qt::PointingHandCursor);

    btnRow->addWidget(m_confirmBtn);
    btnRow->addWidget(m_cancelBtn);
    cardLayout->addLayout(btnRow);

    rootLayout->addWidget(m_card);

    connect(m_dupCheckBtn, SIGNAL(clicked()),            this, SLOT(onDuplicateCheckClicked()));
    connect(m_confirmBtn,  SIGNAL(clicked()),            this, SLOT(onConfirmClicked()));
    connect(m_cancelBtn,   SIGNAL(clicked()),            this, SLOT(onCancelClicked()));
    connect(m_idEdit,      SIGNAL(textChanged(QString)), this, SLOT(onIdTextChanged()));
}

/** objectName 기반 QSS로 스타일을 적용한다. */
void RegisterDialog::applyStyles()
{
    setStyleSheet(AppStyle::DARK_DIALOG_BG);

    m_card->setStyleSheet(
        "#regCard { " + AppStyle::DARK_CARD_BODY + " }"
        "#titleLabel { color: #191F28; font-size: 22px; font-weight: 800; }"
        + AppStyle::DARK_FIELD_LABEL
        + AppStyle::DARK_INPUT
        + R"(
        QDateEdit#dateField {
            background-color: #F9FAFB;
            border: 1.5px solid #E5E8EB;
            border-radius: 10px;
            color: #191F28;
            font-size: 14px;
            padding: 0 12px;
        }
        QDateEdit#dateField:focus {
            border: 1.5px solid #3182F6;
            background-color: white;
        }
        QDateEdit#dateField::drop-down {
            subcontrol-origin: border;
            subcontrol-position: right center;
            width: 32px;
            border-left: 1px solid #E5E8EB;
            border-top-right-radius: 10px;
            border-bottom-right-radius: 10px;
            background-color: #F2F4F6;
        }
        QDateEdit#dateField::down-arrow {
            image: none;
            width: 0; height: 0;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #4E5968;
        }
        QPushButton#dupCheckBtn {
            background-color: #F2F4F6;
            color: #4E5968;
            border: 1.5px solid #E5E8EB;
            border-radius: 10px;
            font-size: 12px;
            font-weight: 600;
        }
        QPushButton#dupCheckBtn:hover { background-color: #E5E8EB; color: #191F28; }
        QPushButton#dupCheckBtn:pressed { background-color: #D1D6DB; }
        QPushButton#confirmBtn {
            background-color: #3182F6;
            color: white;
            border: none;
            border-radius: 12px;
            font-size: 14px;
            font-weight: 700;
        }
        QPushButton#confirmBtn:hover { background-color: #1B64DA; }
        QPushButton#confirmBtn:pressed { background-color: #1B56BC; }
        QPushButton#confirmBtn:disabled { background-color: #B0C8F0; }
        QPushButton#cancelBtn {
            background-color: #F2F4F6;
            color: #4E5968;
            border: 1px solid #E5E8EB;
            border-radius: 12px;
            font-size: 14px;
            font-weight: 600;
        }
        QPushButton#cancelBtn:hover { background-color: #E5E8EB; color: #191F28; }
    )");
}

/**
 * 아이디 입력 필드의 테두리 색상을 동적으로 변경한다.
 * 중복 확인 성공 시 초록(#00C896), 실패 시 빨강(#F04452)으로 표시된다.
 */
void RegisterDialog::setIdBorderColor(const QString &color)
{
    m_idEdit->setStyleSheet(
        QString(
            "QLineEdit {"
            "  background-color: #F9FAFB; border: 1.5px solid %1;"
            "  border-radius: 10px; color: #191F28; font-size: 14px; padding: 0 14px;"
            "}"
            "QLineEdit:focus {"
            "  background-color: white; border: 1.5px solid %1;"
            "}"
        ).arg(color)
    );
}

/**
 * 아이디 텍스트가 변경될 때마다 m_idChecked를 false로 초기화하고
 * 테두리 색을 기본값으로 되돌려 재확인을 유도한다.
 */
void RegisterDialog::onIdTextChanged()
{
    m_idChecked = false;
    setIdBorderColor("#E5E8EB");
}

/**
 * 아이디 공백 및 3자 미만 검증 후 서버에 "check_id" 요청을 전송한다.
 * 요청 중 중복 확인 버튼을 비활성화한다.
 */
void RegisterDialog::onDuplicateCheckClicked()
{
    const QString id = m_idEdit->text().trimmed();

    if (id.isEmpty()) {
        QMessageBox::warning(this, tr("입력 오류"), tr("아이디를 먼저 입력해주세요."));
        return;
    }
    if (id.length() < 3) {
        QMessageBox::warning(this, tr("입력 오류"), tr("아이디는 3자 이상이어야 합니다."));
        return;
    }

    m_dupCheckBtn->setEnabled(false);
    QJsonObject data;
    data["id"] = id;
    QJsonObject req;
    req["type"] = "check_id";
    req["data"] = data;
    NetworkClient::instance()->sendRequest(req);
}

/**
 * 모든 입력 필드를 순서대로 검사한다.
 * 하나라도 실패하면 해당 필드에 포커스를 주고 false를 반환한다.
 */
bool RegisterDialog::validateInputs()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("입력 오류"), tr("이름을 입력해주세요."));
        m_nameEdit->setFocus();
        return false;
    }

    // setMaximumDate로 이미 미래 날짜가 막혀 있지만 이중 체크
    if (m_birthEdit->date() > QDate::currentDate()) {
        QMessageBox::warning(this, tr("입력 오류"), tr("생년월일이 올바르지 않습니다."));
        return false;
    }

    if (m_idEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("입력 오류"), tr("아이디를 입력해주세요."));
        m_idEdit->setFocus();
        return false;
    }

    if (!m_idChecked) {
        QMessageBox::warning(this, tr("중복 확인 필요"), tr("아이디 중복 확인을 먼저 진행해주세요."));
        return false;
    }

    if (m_pwEdit->text().length() < 6) {
        QMessageBox::warning(this, tr("입력 오류"), tr("비밀번호는 6자 이상이어야 합니다."));
        m_pwEdit->setFocus();
        return false;
    }

    if (m_pwEdit->text() != m_pwConfirmEdit->text()) {
        QMessageBox::warning(this, tr("입력 오류"), tr("비밀번호가 일치하지 않습니다."));
        m_pwConfirmEdit->setFocus();
        return false;
    }

    return true;
}

/**
 * validateInputs() 통과 후 서버에 "register" 요청을 전송한다.
 * 요청 중 가입 완료 버튼을 비활성화한다.
 */
void RegisterDialog::onConfirmClicked()
{
    if (!validateInputs()) return;

    m_confirmBtn->setEnabled(false);
    QJsonObject data;
    data["id"]       = m_idEdit->text().trimmed();
    data["password"] = m_pwEdit->text();
    data["name"]     = m_nameEdit->text().trimmed();
    QJsonObject req;
    req["type"] = "register";
    req["data"] = data;
    NetworkClient::instance()->sendRequest(req);
}

/**
 * "check_id_response"와 "register_response" 두 가지 타입을 처리한다.
 *
 * check_id_response 성공: m_idChecked = true, 테두리 초록색.
 * check_id_response 실패: m_idChecked = false, 테두리 빨간색.
 * register_response 성공: 안내 메시지 후 accept()로 닫음.
 */
void RegisterDialog::onNetworkResponse(const QJsonObject& resp)
{
    const QString type   = resp["type"].toString();
    const QString status = resp["status"].toString();
    const QString msg    = resp["message"].toString();

    if (type == "check_id_response") {
        m_dupCheckBtn->setEnabled(true);
        if (status == "success") {
            m_idChecked = true;
            setIdBorderColor("#00C896");
            QMessageBox::information(this, tr("중복 확인"), msg);
        } else {
            m_idChecked = false;
            setIdBorderColor("#F04452");
            QMessageBox::warning(this, tr("중복 확인"), msg);
        }
    } else if (type == "register_response") {
        m_confirmBtn->setEnabled(true);
        if (status == "success") {
            QMessageBox::information(this, tr("회원가입"), tr("회원가입이 완료되었습니다."));
            accept();
        } else {
            QMessageBox::warning(this, tr("회원가입 실패"), msg);
        }
    }
}

/** 네트워크 오류 발생 시 메시지를 표시하고 두 버튼을 모두 다시 활성화한다. */
void RegisterDialog::onNetworkError(const QString& message)
{
    QMessageBox::critical(this, tr("네트워크 오류"), message);
    m_confirmBtn->setEnabled(true);
    m_dupCheckBtn->setEnabled(true);
}

/** 다이얼로그를 거부(reject)하여 닫는다. */
void RegisterDialog::onCancelClicked()
{
    reject();
}
