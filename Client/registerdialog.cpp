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

// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
//  UI 구성
// ─────────────────────────────────────────────────────────────────────────────
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
    m_birthEdit->setDisplayFormat("yyyy-MM-dd");        // YYYY-MM-DD 표시 형식
    m_birthEdit->setDate(QDate(2000, 1, 1));            // 기본 날짜
    m_birthEdit->setMinimumDate(QDate(1900, 1, 1));     // 선택 가능 최소 날짜
    m_birthEdit->setMaximumDate(QDate::currentDate());  // 오늘까지만 선택 가능
    m_birthEdit->setCalendarPopup(true);                // ← 클릭 시 달력 팝업 활성화

    // 달력 위젯 스타일 커스터마이징 (라이트 테마)
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

    // ── O / X 버튼 행 ────────────────────────────────────────────────────────
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

    // ── 시그널 연결 ───────────────────────────────────────────────────────────
    connect(m_dupCheckBtn, SIGNAL(clicked()),          this, SLOT(onDuplicateCheckClicked()));
    connect(m_confirmBtn,  SIGNAL(clicked()),          this, SLOT(onConfirmClicked()));
    connect(m_cancelBtn,   SIGNAL(clicked()),          this, SLOT(onCancelClicked()));
    connect(m_idEdit,      SIGNAL(textChanged(QString)), this, SLOT(onIdTextChanged()));
}

// ─────────────────────────────────────────────────────────────────────────────
//  스타일시트
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
//  헬퍼
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
//  슬롯
// ─────────────────────────────────────────────────────────────────────────────
void RegisterDialog::onIdTextChanged()
{
    m_idChecked = false;
    setIdBorderColor("#E5E8EB");
}

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

    // 서버에 중복 여부 확인 요청 (미연결 시 sendRequest 내부에서 자동 재연결 후 재전송)
    m_dupCheckBtn->setEnabled(false);
    QJsonObject data;
    data["id"] = id;
    QJsonObject req;
    req["type"] = "check_id";
    req["data"] = data;
    NetworkClient::instance()->sendRequest(req);
}

bool RegisterDialog::validateInputs()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, tr("입력 오류"), tr("이름을 입력해주세요."));
        m_nameEdit->setFocus();
        return false;
    }

    // QDateEdit 은 항상 유효한 날짜를 가지므로 별도 형식 검사 불필요
    // 미래 날짜 방지 (setMaximumDate 로 이미 막혀 있지만 이중 체크)
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

void RegisterDialog::onConfirmClicked()
{
    if (!validateInputs()) return;

    // 서버에 회원가입 요청 전송
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

// ─────────────────────────────────────────────────────────────────────────────
//  서버 응답 라우팅
// ─────────────────────────────────────────────────────────────────────────────
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

void RegisterDialog::onNetworkError(const QString& message)
{
    QMessageBox::critical(this, tr("네트워크 오류"), message);
    m_confirmBtn->setEnabled(true);
    m_dupCheckBtn->setEnabled(true);
}

void RegisterDialog::onCancelClicked()
{
    reject();
}