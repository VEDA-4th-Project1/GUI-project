#include "registerdialog.h"
#include "networkclient.h"

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

    // 서버 응답을 이 다이얼로그에서 처리
    connect(NetworkClient::instance(), &NetworkClient::responseReceived,
            this, &RegisterDialog::onNetworkResponse);
    connect(NetworkClient::instance(), &NetworkClient::errorOccurred,
            this, [this](const QString& msg) {
                QMessageBox::critical(this, tr("네트워크 오류"), msg);
                m_confirmBtn->setEnabled(true);
                m_dupCheckBtn->setEnabled(true);
            });
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

    // 달력 위젯 스타일 커스터마이징
    QCalendarWidget *calendar = m_birthEdit->calendarWidget();
    calendar->setGridVisible(true);
    calendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    calendar->setStyleSheet(R"(
        QCalendarWidget {
            background-color: #1e293b;
            color: #f1f5f9;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
            font-size: 12px;
        }
        QCalendarWidget QToolButton {
            background-color: #334155;
            color: #f1f5f9;
            border: none;
            border-radius: 6px;
            padding: 4px 10px;
            font-size: 12px;
            font-weight: 600;
        }
        QCalendarWidget QToolButton:hover {
            background-color: #6366f1;
        }
        QCalendarWidget QToolButton::menu-indicator {
            image: none;
        }
        QCalendarWidget QSpinBox {
            background-color: #0f172a;
            color: #f1f5f9;
            border: 1px solid #334155;
            border-radius: 4px;
            padding: 2px 6px;
            font-size: 12px;
        }
        QCalendarWidget QSpinBox::up-button,
        QCalendarWidget QSpinBox::down-button {
            subcontrol-origin: border;
            width: 16px;
            background-color: #334155;
        }
        QCalendarWidget QWidget#qt_calendar_navigationbar {
            background-color: #0f172a;
            padding: 6px;
            border-radius: 8px;
        }
        QCalendarWidget QAbstractItemView {
            background-color: #1e293b;
            color: #f1f5f9;
            selection-background-color: #6366f1;
            selection-color: #ffffff;
            gridline-color: #334155;
            font-size: 12px;
        }
        QCalendarWidget QAbstractItemView:disabled {
            color: #475569;
        }
        QCalendarWidget QMenu {
            background-color: #1e293b;
            color: #f1f5f9;
            border: 1px solid #334155;
        }
        QCalendarWidget QMenu::item:selected {
            background-color: #6366f1;
        }
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
    connect(m_dupCheckBtn, &QPushButton::clicked,   this, &RegisterDialog::onDuplicateCheckClicked);
    connect(m_confirmBtn,  &QPushButton::clicked,   this, &RegisterDialog::onConfirmClicked);
    connect(m_cancelBtn,   &QPushButton::clicked,   this, &RegisterDialog::onCancelClicked);
    connect(m_idEdit,      &QLineEdit::textChanged, this, &RegisterDialog::onIdTextChanged);
}

// ─────────────────────────────────────────────────────────────────────────────
//  스타일시트
// ─────────────────────────────────────────────────────────────────────────────
void RegisterDialog::applyStyles()
{
    setStyleSheet("QDialog { background-color: #0f172a; }");

    m_card->setStyleSheet(R"(
        #regCard {
            background-color: #1e293b;
            border-radius: 18px;
            border: 1px solid #334155;
        }
        #titleLabel {
            color: #f8fafc;
            font-size: 20px;
            font-weight: 700;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QLabel#fieldLabel {
            color: #cbd5e1;
            font-size: 12px;
            font-weight: 600;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QLineEdit#inputField {
            background-color: #0f172a;
            border: 1.5px solid #334155;
            border-radius: 8px;
            color: #f1f5f9;
            font-size: 13px;
            padding: 0 12px;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QLineEdit#inputField:focus {
            border: 1.5px solid #6366f1;
            background-color: #1a2540;
        }

        /* ── QDateEdit 달력 버튼 ── */
        QDateEdit#dateField {
            background-color: #0f172a;
            border: 1.5px solid #334155;
            border-radius: 8px;
            color: #f1f5f9;
            font-size: 13px;
            padding: 0 12px;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QDateEdit#dateField:focus {
            border: 1.5px solid #6366f1;
            background-color: #1a2540;
        }
        QDateEdit#dateField::drop-down {
            subcontrol-origin: border;
            subcontrol-position: right center;
            width: 32px;
            border-left: 1px solid #334155;
            border-top-right-radius: 8px;
            border-bottom-right-radius: 8px;
            background-color: #334155;
        }
        QDateEdit#dateField::down-arrow {
            image: none;
            width: 0;
            height: 0;
            border-left: 5px solid transparent;
            border-right: 5px solid transparent;
            border-top: 6px solid #94a3b8;
        }

        /* ── 중복확인 버튼 ── */
        QPushButton#dupCheckBtn {
            background-color: #334155;
            color: #94a3b8;
            border: 1.5px solid #475569;
            border-radius: 8px;
            font-size: 12px;
            font-weight: 600;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QPushButton#dupCheckBtn:hover  { background-color: #475569; color: #f1f5f9; }
        QPushButton#dupCheckBtn:pressed { background-color: #1e293b; }

        /* ── 가입 완료 버튼 ── */
        QPushButton#confirmBtn {
            background-color: #6366f1;
            color: #ffffff;
            border: none;
            border-radius: 9px;
            font-size: 14px;
            font-weight: 700;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QPushButton#confirmBtn:hover  { background-color: #818cf8; }
        QPushButton#confirmBtn:pressed { background-color: #4f46e5; }

        /* ── 취소 버튼 ── */
        QPushButton#cancelBtn {
            background-color: transparent;
            color: #f87171;
            border: 1.5px solid #f87171;
            border-radius: 9px;
            font-size: 14px;
            font-weight: 700;
            font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
        }
        QPushButton#cancelBtn:hover  { background-color: rgba(248,113,113,0.12); }
        QPushButton#cancelBtn:pressed { background-color: rgba(248,113,113,0.25); }
    )");
}

// ─────────────────────────────────────────────────────────────────────────────
//  헬퍼
// ─────────────────────────────────────────────────────────────────────────────
void RegisterDialog::setIdBorderColor(const QString &color)
{
    m_idEdit->setStyleSheet(
        QString(R"(
            QLineEdit {
                background-color: #0f172a;
                border: 1.5px solid %1;
                border-radius: 8px;
                color: #f1f5f9;
                font-size: 13px;
                padding: 0 12px;
                font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif;
            }
            QLineEdit:focus {
                background-color: #1a2540;
                border: 1.5px solid %1;
            }
        )").arg(color)
        );
}

// ─────────────────────────────────────────────────────────────────────────────
//  슬롯
// ─────────────────────────────────────────────────────────────────────────────
void RegisterDialog::onIdTextChanged()
{
    m_idChecked = false;
    setIdBorderColor("#334155");
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
            setIdBorderColor("#10b981");
            QMessageBox::information(this, tr("중복 확인"), msg);
        } else {
            m_idChecked = false;
            setIdBorderColor("#f87171");
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

void RegisterDialog::onCancelClicked()
{
    reject();
}