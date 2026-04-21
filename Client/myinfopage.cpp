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

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 40, 40, 40);
    root->setSpacing(20);

    // ── 내 정보 카드 ──────────────────────────────────────────────────────────
    QFrame *card = new QFrame(this);
    card->setStyleSheet(AppStyle::CARD);
    card->setMaximumWidth(560);
    AppStyle::applyCardShadow(card);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(16);

    QLabel *title = new QLabel("내 정보");
    title->setStyleSheet(AppStyle::LABEL_TITLE);
    cardLayout->addWidget(title);

    auto makeDivider = [&]() -> QFrame* {
        QFrame *div = new QFrame;
        div->setFrameShape(QFrame::HLine);
        div->setStyleSheet("background-color: #E5E8EB; border: none; max-height: 1px;");
        return div;
    };
    cardLayout->addWidget(makeDivider());

    auto addInfoRow = [&](const QString &fieldName, QLabel *&valueLabel) {
        QHBoxLayout *row = new QHBoxLayout;
        row->setSpacing(16);
        QLabel *field = new QLabel(fieldName);
        field->setStyleSheet(AppStyle::LABEL_FIELD);
        valueLabel = new QLabel("-");
        valueLabel->setStyleSheet("font-size: 14px; color: #191F28; border: none;");
        row->addWidget(field);
        row->addWidget(valueLabel, 1);
        cardLayout->addLayout(row);
    };

    addInfoRow("아이디",  m_idLabel);
    addInfoRow("이름",    m_nameLabel);
    addInfoRow("가입일",  m_createdAtLabel);

    cardLayout->addSpacing(8);
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

    // 카드를 중앙 정렬하되 가득 차면 늘어나도록
    QHBoxLayout *centerRow = new QHBoxLayout;
    centerRow->addStretch();
    centerRow->addWidget(card, 1);
    centerRow->addStretch();

    root->addStretch();
    root->addLayout(centerRow);
    root->addStretch();

    connect(btnChangePw, &QPushButton::clicked, this, &MyInfoPage::openChangePasswordDialog);
    connect(btnCloseAcc, &QPushButton::clicked, this, &MyInfoPage::openCloseAccountDialog);
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
        m_idLabel->setText(user["id"].toString());
        m_nameLabel->setText(user["name"].toString());
        QString rawDate = user["createdAt"].toString();
        QDateTime dt = QDateTime::fromString(rawDate, Qt::ISODate);
        if (!dt.isValid())
            dt = QDateTime::fromString(rawDate, Qt::ISODateWithMs);
        QString displayDate = dt.isValid()
            ? dt.date().toString("yyyy년 MM월 dd일")
            : rawDate;
        m_createdAtLabel->setText(displayDate);
    }

    else if (type == "list_accounts_response") {
        if (status != "success") return;
        m_accountList.clear();
        const QJsonArray accounts = resp["data"].toObject()["accounts"].toArray();
        for (const QJsonValue &v : accounts) {
            QJsonObject acc = v.toObject();
            QString accNum  = acc["accountNumber"].toString();
            QString accType = acc["type"].toString();
            // 표시: "110-921-427450 (savings)" 형식
            m_accountList << QString("%1 (%2)").arg(accNum, accType);
        }
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