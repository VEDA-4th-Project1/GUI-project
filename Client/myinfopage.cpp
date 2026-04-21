#include "myinfopage.h"
#include "networkclient.h"
#include "sessioncontext.h"
#include "appstyle.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QScrollArea>
#include <QLocale>


static int showMsg(QWidget *parent, QMessageBox::Icon icon,
                   const QString &title, const QString &text,
                   QMessageBox::StandardButtons btns = QMessageBox::Ok)
{
    QMessageBox box(icon, title, text, btns, parent);
    box.setStyleSheet(AppStyle::MSGBOX);
    return box.exec();
}

// ─────────────────────────────────────────────────────────────────────────────

MyInfoPage::MyInfoPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onNetworkResponse(QJsonObject)));
}

void MyInfoPage::setupUI()
{
    setStyleSheet("background-color: #F2F4F6;");

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: transparent; width: 8px; margin: 4px; }"
        "QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 4px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #94A3B8; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }");

    QWidget *content = new QWidget(scroll);
    content->setStyleSheet("background-color: #F2F4F6;");
    scroll->setWidget(content);
    outer->addWidget(scroll);

    QVBoxLayout *root = new QVBoxLayout(content);
    root->setContentsMargins(40, 40, 40, 40);
    root->setSpacing(20);

    // ── 프로필 헤더 카드 (그라데이션 + 원형 아바타) ───────────────────────────
    QFrame *header = new QFrame(content);
    header->setStyleSheet(
        "QFrame {"
        "  border-radius: 18px;"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "      stop:0 #3182F6, stop:1 #00C896);"
        "}");
    header->setMinimumHeight(140);
    AppStyle::applyHeroShadow(header);

    QHBoxLayout *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(28, 24, 28, 24);
    headerLayout->setSpacing(20);

    m_avatarLabel = new QLabel("?", header);
    m_avatarLabel->setFixedSize(72, 72);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setStyleSheet(
        "background-color: rgba(255,255,255,0.22);"
        "color: white;"
        "font-size: 30px; font-weight: 800;"
        "border-radius: 36px; border: 2px solid rgba(255,255,255,0.55);");

    QVBoxLayout *headerTextCol = new QVBoxLayout();
    headerTextCol->setSpacing(4);

    QLabel *headerKicker = new QLabel("Easy Bank 회원", header);
    headerKicker->setStyleSheet(
        "color: rgba(255,255,255,0.8); font-size: 12px; font-weight: 700;"
        "letter-spacing: 0.5px; background: transparent; border: none;");

    m_headerNameLabel = new QLabel("-", header);
    m_headerNameLabel->setStyleSheet(
        "color: white; font-size: 24px; font-weight: 800;"
        "background: transparent; border: none;");

    QLabel *headerSub = new QLabel("소중한 자산을 안전하게 관리해 드립니다.", header);
    headerSub->setStyleSheet(
        "color: rgba(255,255,255,0.82); font-size: 13px; font-weight: 500;"
        "background: transparent; border: none;");

    headerTextCol->addStretch();
    headerTextCol->addWidget(headerKicker);
    headerTextCol->addWidget(m_headerNameLabel);
    headerTextCol->addWidget(headerSub);
    headerTextCol->addStretch();

    headerLayout->addWidget(m_avatarLabel);
    headerLayout->addLayout(headerTextCol, 1);

    root->addWidget(header);

    // ── 통계 카드 3개 ────────────────────────────────────────────────────────
    auto makeStatCard = [&](const QString &title, QLabel *&valueOut) -> QFrame* {
        QFrame *c = new QFrame(content);
        c->setStyleSheet(AppStyle::CARD);
        AppStyle::applyCardShadow(c);
        QVBoxLayout *cl = new QVBoxLayout(c);
        cl->setContentsMargins(24, 20, 24, 20);
        cl->setSpacing(6);
        QLabel *t = new QLabel(title, c);
        t->setStyleSheet(
            "color: #8B95A1; font-size: 12px; font-weight: 700;"
            "letter-spacing: 0.3px; background: transparent; border: none;");
        valueOut = new QLabel("-", c);
        valueOut->setStyleSheet(
            "color: #191F28; font-size: 22px; font-weight: 800;"
            "background: transparent; border: none;");
        cl->addWidget(t);
        cl->addWidget(valueOut);
        return c;
    };

    QHBoxLayout *statsRow = new QHBoxLayout();
    statsRow->setSpacing(16);
    statsRow->addWidget(makeStatCard("보유 계좌", m_statAccountsVal), 1);
    statsRow->addWidget(makeStatCard("총 자산",    m_statTotalVal),    1);
    statsRow->addWidget(makeStatCard("가입 경과일", m_statDaysVal),    1);
    root->addLayout(statsRow);

    // ── 내 정보 카드 ──────────────────────────────────────────────────────────
    QFrame *card = new QFrame(content);
    card->setStyleSheet(AppStyle::CARD);
    AppStyle::applyCardShadow(card);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(16);

    QLabel *title = new QLabel("기본 정보", card);
    title->setStyleSheet(AppStyle::LABEL_TITLE);
    cardLayout->addWidget(title);

    QLabel *titleSub = new QLabel("회원님의 프로필 정보입니다.", card);
    titleSub->setStyleSheet(AppStyle::LABEL_MUTED);
    cardLayout->addWidget(titleSub);
    cardLayout->addSpacing(6);

    auto makeDivider = [&]() -> QFrame* {
        QFrame *div = new QFrame;
        div->setFrameShape(QFrame::HLine);
        div->setStyleSheet("background-color: #E5E8EB; border: none; max-height: 1px;");
        return div;
    };

    // 아이콘(이모지) 칩 + 라벨 + 값 구성의 카드형 행
    auto addInfoRow = [&](const QString &icon, const QString &chipBg, const QString &chipFg,
                          const QString &fieldName, QLabel *&valueLabel) {
        QFrame *row = new QFrame(card);
        row->setStyleSheet(
            "QFrame {"
            "  background-color: #F8FAFC;"
            "  border: 1px solid #EEF2F6;"
            "  border-radius: 12px;"
            "}");
        QHBoxLayout *rl = new QHBoxLayout(row);
        rl->setContentsMargins(16, 12, 16, 12);
        rl->setSpacing(14);

        QLabel *iconChip = new QLabel(icon, row);
        iconChip->setFixedSize(40, 40);
        iconChip->setAlignment(Qt::AlignCenter);
        iconChip->setStyleSheet(QString(
            "background-color: %1; color: %2;"
            "font-size: 18px; font-weight: 800;"
            "border-radius: 20px; border: none;").arg(chipBg, chipFg));

        QVBoxLayout *col = new QVBoxLayout();
        col->setContentsMargins(0, 0, 0, 0);
        col->setSpacing(2);

        QLabel *field = new QLabel(fieldName, row);
        field->setStyleSheet(
            "color: #8B95A1; font-size: 12px; font-weight: 700;"
            "letter-spacing: 0.3px; background: transparent; border: none;");

        valueLabel = new QLabel("-", row);
        valueLabel->setStyleSheet(
            "color: #191F28; font-size: 16px; font-weight: 700;"
            "background: transparent; border: none;");

        col->addWidget(field);
        col->addWidget(valueLabel);

        rl->addWidget(iconChip);
        rl->addLayout(col, 1);
        cardLayout->addWidget(row);
    };

    addInfoRow("@",  "#DBEAFE", "#1D4ED8", "아이디",  m_idLabel);
    addInfoRow("👤", "#FCE7F3", "#BE185D", "이름",    m_nameLabel);
    addInfoRow("📅", "#DCFCE7", "#047857", "가입일",  m_createdAtLabel);

    cardLayout->addSpacing(6);
    cardLayout->addWidget(makeDivider());
    cardLayout->addSpacing(4);

    QPushButton *btnChangePw = new QPushButton("비밀번호 변경");
    QPushButton *btnCloseAcc = new QPushButton("계좌 해지");
    btnChangePw->setMinimumHeight(44);
    btnCloseAcc->setMinimumHeight(44);
    btnChangePw->setCursor(Qt::PointingHandCursor);
    btnCloseAcc->setCursor(Qt::PointingHandCursor);
    btnChangePw->setStyleSheet(AppStyle::BTN_BLUE);
    btnCloseAcc->setStyleSheet(AppStyle::BTN_RED);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);
    btnRow->addWidget(btnChangePw);
    btnRow->addWidget(btnCloseAcc);
    cardLayout->addLayout(btnRow);

    root->addWidget(card);

    // ── 내 계좌 목록 카드 ────────────────────────────────────────────────────
    QFrame *accCard = new QFrame(content);
    accCard->setStyleSheet(AppStyle::CARD);
    AppStyle::applyCardShadow(accCard);

    QVBoxLayout *accLayout = new QVBoxLayout(accCard);
    accLayout->setContentsMargins(32, 28, 32, 28);
    accLayout->setSpacing(12);

    QLabel *accTitle = new QLabel("내 계좌", accCard);
    accTitle->setStyleSheet(AppStyle::LABEL_TITLE);
    accLayout->addWidget(accTitle);
    accLayout->addWidget(makeDivider());

    m_accountsListBox = new QWidget(accCard);
    m_accountsListBox->setStyleSheet("background: transparent;");
    QVBoxLayout *listLayout = new QVBoxLayout(m_accountsListBox);
    listLayout->setContentsMargins(0, 4, 0, 0);
    listLayout->setSpacing(8);
    accLayout->addWidget(m_accountsListBox);

    root->addWidget(accCard);
    root->addStretch();

    connect(btnChangePw, SIGNAL(clicked()), this, SLOT(openChangePasswordDialog()));
    connect(btnCloseAcc, SIGNAL(clicked()), this, SLOT(openCloseAccountDialog()));
}

// ── 서버 요청 ─────────────────────────────────────────────────────────────────
void MyInfoPage::loadUserInfo()
{
    QJsonObject req1;
    req1["type"]  = "get_user_info";
    req1["token"] = SessionContext::instance().token();
    req1["data"]  = QJsonObject{};
    NetworkClient::instance()->sendRequest(req1);

    QJsonObject req2;
    req2["type"]  = "list_accounts";
    req2["token"] = SessionContext::instance().token();
    req2["data"]  = QJsonObject{};
    NetworkClient::instance()->sendRequest(req2);
}

// ── 비밀번호 변경 다이얼로그 ──────────────────────────────────────────────────
void MyInfoPage::openChangePasswordDialog()
{
    m_changePwDlg = new QDialog(this);
    m_changePwDlg->setWindowTitle("비밀번호 변경");
    m_changePwDlg->setFixedWidth(380);
    m_changePwDlg->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout(m_changePwDlg);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(8);

    QLabel *title = new QLabel("비밀번호 변경");
    title->setStyleSheet("font-size: 17px; font-weight: 700; color: #191F28;");
    layout->addWidget(title);
    layout->addSpacing(8);

    auto addField = [&](const QString &labelText, QLineEdit *&edit) {
        QLabel *lbl = new QLabel(labelText);
        lbl->setStyleSheet(AppStyle::LABEL_MUTED);
        edit = new QLineEdit;
        edit->setEchoMode(QLineEdit::Password);
        edit->setFixedHeight(40);
        edit->setStyleSheet(AppStyle::INPUT);
        layout->addWidget(lbl);
        layout->addWidget(edit);
    };

    addField("현재 비밀번호",    m_currentPwEdit);
    addField("새 비밀번호",      m_newPwEdit);
    addField("새 비밀번호 확인", m_confirmPwEdit);
    layout->addSpacing(12);

    QPushButton *btnOk     = new QPushButton("변경");
    QPushButton *btnCancel = new QPushButton("취소");
    btnOk->setMinimumHeight(42);
    btnCancel->setMinimumHeight(42);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnOk->setStyleSheet(AppStyle::BTN_BLUE);
    btnCancel->setStyleSheet(AppStyle::BTN_GRAY);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnCancel);
    layout->addLayout(btnRow);

    connect(btnCancel, SIGNAL(clicked()), m_changePwDlg, SLOT(reject()));
    connect(btnOk,     SIGNAL(clicked()), this,          SLOT(onChangePwOkClicked()));

    m_changePwDlg->exec();
}

void MyInfoPage::onChangePwOkClicked()
{
    if (m_currentPwEdit->text().isEmpty() || m_newPwEdit->text().isEmpty()) {
        showMsg(m_changePwDlg, QMessageBox::Warning, "오류", "모든 항목을 입력해주세요.");
        return;
    }
    if (m_newPwEdit->text().length() < 6) {
        showMsg(m_changePwDlg, QMessageBox::Warning, "오류", "새 비밀번호는 6자 이상이어야 합니다.");
        return;
    }
    if (m_newPwEdit->text() != m_confirmPwEdit->text()) {
        showMsg(m_changePwDlg, QMessageBox::Warning, "오류", "새 비밀번호가 일치하지 않습니다.");
        return;
    }
    if (m_currentPwEdit->text() == m_newPwEdit->text()){
        showMsg(m_changePwDlg, QMessageBox::Warning, "오류", "현재 비밀번호와 새 비밀번호가 같습니다.");
        return;
    }

    QJsonObject data;
    data["currentPassword"] = m_currentPwEdit->text();
    data["newPassword"]     = m_newPwEdit->text();

    QJsonObject request;
    request["type"]  = "change_password";
    request["token"] = SessionContext::instance().token();
    request["data"]  = data;
    NetworkClient::instance()->sendRequest(request);

    m_changePwDlg->accept();
}

// ── 계좌 해지 다이얼로그 ──────────────────────────────────────────────────────
// 핵심: dlg->accept() 제거 — 다이얼로그를 열어둔 채로 서버 응답을
//       MyInfoPage::onNetworkResponse 에서 받아서 처리함
void MyInfoPage::openCloseAccountDialog()
{
    if (m_accountList.isEmpty()) {
        showMsg(this, QMessageBox::Information, "알림", "해지할 계좌가 없습니다.");
        return;
    }

    m_closeAccDlg = new QDialog(this);
    m_closeAccDlg->setWindowTitle("계좌 해지");
    m_closeAccDlg->setFixedWidth(380);
    m_closeAccDlg->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout(m_closeAccDlg);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(8);

    QLabel *title = new QLabel("계좌 해지");
    title->setStyleSheet("font-size: 17px; font-weight: 700; color: #191F28;");
    layout->addWidget(title);
    layout->addSpacing(8);

    QLabel *accLabel = new QLabel("해지할 계좌 선택");
    accLabel->setStyleSheet(AppStyle::LABEL_MUTED);
    m_closeAccCombo = new QComboBox;
    m_closeAccCombo->setFixedHeight(40);
    m_closeAccCombo->addItems(m_accountList);
    m_closeAccCombo->setStyleSheet(AppStyle::COMBO);

    QLabel *pwLabel = new QLabel("계좌 비밀번호");
    pwLabel->setStyleSheet(AppStyle::LABEL_MUTED);
    m_closeAccPwEdit = new QLineEdit;
    m_closeAccPwEdit->setEchoMode(QLineEdit::Password);
    m_closeAccPwEdit->setPlaceholderText("계좌 비밀번호를 입력하세요");
    m_closeAccPwEdit->setFixedHeight(40);
    m_closeAccPwEdit->setStyleSheet(AppStyle::INPUT);

    layout->addWidget(accLabel);
    layout->addWidget(m_closeAccCombo);
    layout->addWidget(pwLabel);
    layout->addWidget(m_closeAccPwEdit);
    layout->addSpacing(12);

    m_closeAccOkBtn         = new QPushButton("해지");
    QPushButton *btnCancel  = new QPushButton("취소");
    m_closeAccOkBtn->setMinimumHeight(42);
    btnCancel->setMinimumHeight(42);
    m_closeAccOkBtn->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);
    m_closeAccOkBtn->setStyleSheet(AppStyle::BTN_RED);
    btnCancel->setStyleSheet(AppStyle::BTN_GRAY);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);
    btnRow->addWidget(m_closeAccOkBtn);
    btnRow->addWidget(btnCancel);
    layout->addLayout(btnRow);

    connect(btnCancel,      SIGNAL(clicked()), m_closeAccDlg, SLOT(reject()));
    connect(m_closeAccOkBtn, SIGNAL(clicked()), this,         SLOT(onCloseAccOkClicked()));

    m_closeAccDlg->exec();
}

void MyInfoPage::onCloseAccOkClicked()
{
    if (m_closeAccPwEdit->text().isEmpty()) {
        showMsg(m_closeAccDlg, QMessageBox::Warning, "오류", "계좌 비밀번호를 입력해주세요.");
        return;
    }

    const QString selectedText  = m_closeAccCombo->currentText();
    const QString accountNumber = selectedText.left(selectedText.indexOf(" ("));

    int reply = showMsg(m_closeAccDlg, QMessageBox::Question, "계좌 해지",
                        QString("계좌 [%1]을 정말 해지하시겠습니까?").arg(accountNumber),
                        QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    m_closeAccOkBtn->setEnabled(false);

    QJsonObject data;
    data["accountNumber"]   = accountNumber;
    data["accountPassword"] = m_closeAccPwEdit->text();

    QJsonObject request;
    request["type"]  = "close_account";
    request["token"] = SessionContext::instance().token();
    request["data"]  = data;
    NetworkClient::instance()->sendRequest(request);
    // 응답은 onNetworkResponse 에서 처리 → close_account_response 성공 시 m_closeAccDlg->accept()
}

// ── 서버 응답 처리 ────────────────────────────────────────────────────────────
void MyInfoPage::onNetworkResponse(const QJsonObject &resp)
{
    const QString type   = resp["type"].toString();
    const QString status = resp["status"].toString();

    if (type == "get_user_info_response") {
        if (status != "success") return;
        QJsonObject user = resp["data"].toObject()["user"].toObject();
        const QString name = user["name"].toString();
        m_idLabel->setText(user["id"].toString());
        m_nameLabel->setText(name);

        if (m_headerNameLabel) m_headerNameLabel->setText(name + "님");
        if (m_avatarLabel)
            m_avatarLabel->setText(name.isEmpty() ? "?" : name.left(1));

        QString rawDate = user["createdAt"].toString();
        QDateTime dt = QDateTime::fromString(rawDate, Qt::ISODate);
        if (!dt.isValid())
            dt = QDateTime::fromString(rawDate, Qt::ISODateWithMs);
        QString displayDate = dt.isValid()
            ? dt.date().toString("yyyy년 MM월 dd일")
            : rawDate;
        m_createdAtLabel->setText(displayDate);

        if (m_statDaysVal) {
            if (dt.isValid()) {
                qint64 days = dt.date().daysTo(QDate::currentDate());
                m_statDaysVal->setText(QString("%1일").arg(days));
            } else {
                m_statDaysVal->setText("-");
            }
        }
    }

    else if (type == "list_accounts_response") {
        if (status != "success") return;
        m_accountList.clear();
        m_totalBalance = 0.0;
        const QJsonArray accounts = resp["data"].toObject()["accounts"].toArray();

        // 기존 계좌 행 제거
        if (m_accountsListBox) {
            QLayout *ll = m_accountsListBox->layout();
            QLayoutItem *item;
            while (ll && (item = ll->takeAt(0)) != nullptr) {
                if (QWidget *w = item->widget()) w->deleteLater();
                delete item;
            }
        }

        QVBoxLayout *listLayout = m_accountsListBox
            ? static_cast<QVBoxLayout*>(m_accountsListBox->layout())
            : nullptr;

        for (const QJsonValue &v : accounts) {
            QJsonObject acc = v.toObject();
            QString accNum  = acc["accountNumber"].toString();
            QString accType = acc["type"].toString();
            double  balance = acc["balance"].toDouble();
            m_totalBalance += balance;
            m_accountList << QString("%1 (%2)").arg(accNum, accType);

            if (!listLayout) continue;

            QFrame *row = new QFrame(m_accountsListBox);
            row->setStyleSheet(
                "QFrame {"
                "  background-color: #F8FAFC;"
                "  border: 1px solid #E5E8EB;"
                "  border-radius: 12px;"
                "}");
            QHBoxLayout *rl = new QHBoxLayout(row);
            rl->setContentsMargins(16, 12, 16, 12);
            rl->setSpacing(12);

            QLabel *typeBadge = new QLabel(accType, row);
            const bool savings = (accType == "savings");
            typeBadge->setStyleSheet(QString(
                "background-color: %1;"
                "color: %2;"
                "font-size: 11px; font-weight: 800;"
                "padding: 4px 10px; border-radius: 10px; border: none;")
                .arg(savings ? "#DBEAFE" : "#DCFCE7",
                     savings ? "#1D4ED8" : "#047857"));

            QLabel *numLabel = new QLabel(accNum, row);
            numLabel->setStyleSheet(
                "color: #191F28; font-size: 14px; font-weight: 700;"
                "background: transparent; border: none;");

            QLabel *balLabel = new QLabel(
                QLocale(QLocale::Korean).toString(static_cast<qlonglong>(balance)) + "원", row);
            balLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
            balLabel->setStyleSheet(
                "color: #3182F6; font-size: 14px; font-weight: 800;"
                "background: transparent; border: none;");

            rl->addWidget(typeBadge);
            rl->addWidget(numLabel);
            rl->addStretch();
            rl->addWidget(balLabel);
            listLayout->addWidget(row);
        }

        if (listLayout && accounts.isEmpty()) {
            QLabel *empty = new QLabel("보유 중인 계좌가 없습니다.", m_accountsListBox);
            empty->setAlignment(Qt::AlignCenter);
            empty->setStyleSheet(
                "color: #8B95A1; font-size: 13px; padding: 20px;"
                "background: transparent; border: none;");
            listLayout->addWidget(empty);
        }

        if (m_statAccountsVal)
            m_statAccountsVal->setText(QString("%1개").arg(accounts.size()));
        if (m_statTotalVal)
            m_statTotalVal->setText(
                QLocale(QLocale::Korean).toString(static_cast<qlonglong>(m_totalBalance)) + "원");
    }

    else if (type == "change_password_response") {
        if (status == "success")
            showMsg(this, QMessageBox::Information, "완료", "비밀번호가 변경되었습니다.");
        else
            showMsg(this, QMessageBox::Critical, "실패", resp["message"].toString());
    }

    else if (type == "close_account_response") {
        if (status == "success") {
            if (m_closeAccDlg) {
                m_closeAccDlg->accept();
                m_closeAccDlg = nullptr;
            }
            showMsg(this, QMessageBox::Information, "완료", "계좌가 해지되었습니다.");
            loadUserInfo();
        } else {
            if (m_closeAccOkBtn) m_closeAccOkBtn->setEnabled(true);
            showMsg(this, QMessageBox::Critical, "실패", resp["message"].toString());
        }
    }
}