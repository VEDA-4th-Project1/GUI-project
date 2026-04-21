#include "myinfopage.h"
#include "networkclient.h"
#include "sessioncontext.h"

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

static const QString S_INPUT =
    "QLineEdit {"
    "  background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
    "  border-radius: 8px; color: #111827; font-size: 13px; padding: 0 12px;"
    "}"
    "QLineEdit:focus { border-color: #2563EB; background-color: white; }";

static const QString S_COMBO =
    "QComboBox {"
    "  background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
    "  border-radius: 8px; color: #111827; font-size: 13px; padding: 0 12px;"
    "}"
    "QComboBox::drop-down { border: none; width: 28px; }"
    "QComboBox QAbstractItemView { background-color: white; color: #111827; }";

static const QString S_BTN_BLUE =
    "QPushButton { background-color: #2563EB; color: white; border: none;"
    " border-radius: 8px; font-size: 13px; font-weight: bold; }"
    "QPushButton:hover { background-color: #1D4ED8; }";

static const QString S_BTN_RED =
    "QPushButton { background-color: #EF4444; color: white; border: none;"
    " border-radius: 8px; font-size: 13px; font-weight: bold; }"
    "QPushButton:hover { background-color: #DC2626; }";

static const QString S_BTN_GRAY =
    "QPushButton { background-color: #6B7280; color: white; border: none;"
    " border-radius: 8px; font-size: 13px; font-weight: bold; }"
    "QPushButton:hover { background-color: #4B5563; }";

// ─────────────────────────────────────────────────────────────────────────────

MyInfoPage::MyInfoPage(QWidget *parent) : QWidget(parent)
{
    setupUI();
    connect(NetworkClient::instance(), &NetworkClient::responseReceived,
            this, &MyInfoPage::onNetworkResponse);
}

void MyInfoPage::setupUI()
{
    setStyleSheet("MyInfoPage { background-color: #F5F7FB; }");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 40, 40, 40);

    QWidget *card = new QWidget;
    card->setStyleSheet(
        "background-color: white; border-radius: 16px; border: 1px solid #E5E7EB;");
    card->setMaximumWidth(500);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(14);

    QLabel *title = new QLabel("내 정보");
    title->setStyleSheet("font-size: 20px; font-weight: 700; color: #111827; border: none;");
    cardLayout->addWidget(title);

    auto makeDivider = [&]() -> QFrame* {
        QFrame *div = new QFrame;
        div->setFrameShape(QFrame::HLine);
        div->setStyleSheet("background-color: #F3F4F6; border: none; max-height: 1px;");
        return div;
    };
    cardLayout->addWidget(makeDivider());

    auto addInfoRow = [&](const QString &fieldName, QLabel *&valueLabel) {
        QHBoxLayout *row = new QHBoxLayout;
        QLabel *field = new QLabel(fieldName);
        field->setStyleSheet(
            "font-size: 13px; font-weight: 600; color: #6B7280; border: none; min-width: 80px;");
        valueLabel = new QLabel("-");
        valueLabel->setStyleSheet("font-size: 14px; color: #111827; border: none;");
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
    btnChangePw->setFixedHeight(44);
    btnCloseAcc->setFixedHeight(44);
    btnChangePw->setCursor(Qt::PointingHandCursor);
    btnCloseAcc->setCursor(Qt::PointingHandCursor);
    btnChangePw->setStyleSheet(S_BTN_BLUE);
    btnCloseAcc->setStyleSheet(S_BTN_RED);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setSpacing(12);
    btnRow->addWidget(btnChangePw);
    btnRow->addWidget(btnCloseAcc);
    cardLayout->addLayout(btnRow);

    QHBoxLayout *center = new QHBoxLayout;
    center->addStretch();
    center->addWidget(card);
    center->addStretch();

    root->addStretch();
    root->addLayout(center);
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
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("비밀번호 변경");
    dlg->setFixedWidth(380);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout(dlg);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(8);

    QLabel *title = new QLabel("비밀번호 변경");
    title->setStyleSheet("font-size: 17px; font-weight: 700; color: #111827;");
    layout->addWidget(title);
    layout->addSpacing(8);

    auto addField = [&](const QString &labelText, QLineEdit *&edit) {
        QLabel *lbl = new QLabel(labelText);
        lbl->setStyleSheet("font-size: 12px; font-weight: 600; color: #6B7280;");
        edit = new QLineEdit;
        edit->setEchoMode(QLineEdit::Password);
        edit->setFixedHeight(40);
        edit->setStyleSheet(S_INPUT);
        layout->addWidget(lbl);
        layout->addWidget(edit);
    };

    QLineEdit *currentPwEdit, *newPwEdit, *confirmPwEdit;
    addField("현재 비밀번호",    currentPwEdit);
    addField("새 비밀번호",      newPwEdit);
    addField("새 비밀번호 확인", confirmPwEdit);
    layout->addSpacing(12);

    QPushButton *btnOk     = new QPushButton("변경");
    QPushButton *btnCancel = new QPushButton("취소");
    btnOk->setFixedHeight(42);
    btnCancel->setFixedHeight(42);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnOk->setStyleSheet(S_BTN_BLUE);
    btnCancel->setStyleSheet(S_BTN_GRAY);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnCancel);
    layout->addLayout(btnRow);

    connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);
    connect(btnOk, &QPushButton::clicked, dlg, [=]() {
        if (currentPwEdit->text().isEmpty() || newPwEdit->text().isEmpty()) {
            QMessageBox::warning(dlg, "오류", "모든 항목을 입력해주세요.");
            return;
        }
        if (newPwEdit->text() != confirmPwEdit->text()) {
            QMessageBox::warning(dlg, "오류", "새 비밀번호가 일치하지 않습니다.");
            return;
        }

        QJsonObject data;
        data["currentPassword"] = currentPwEdit->text();
        data["newPassword"]     = newPwEdit->text();

        QJsonObject request;
        request["type"]  = "change_password";
        request["token"] = SessionContext::instance().token();
        request["data"]  = data;
        NetworkClient::instance()->sendRequest(request);

        dlg->accept();
    });

    dlg->exec();
}

// ── 계좌 해지 다이얼로그 ──────────────────────────────────────────────────────
// 핵심: dlg->accept() 제거 — 다이얼로그를 열어둔 채로 서버 응답을
//       MyInfoPage::onNetworkResponse 에서 받아서 처리함
void MyInfoPage::openCloseAccountDialog()
{
    if (m_accountList.isEmpty()) {
        QMessageBox::information(this, "알림", "해지할 계좌가 없습니다.");
        return;
    }

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("계좌 해지");
    dlg->setFixedWidth(380);
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout *layout = new QVBoxLayout(dlg);
    layout->setContentsMargins(28, 24, 28, 24);
    layout->setSpacing(8);

    QLabel *title = new QLabel("계좌 해지");
    title->setStyleSheet("font-size: 17px; font-weight: 700; color: #111827;");
    layout->addWidget(title);
    layout->addSpacing(8);

    QLabel *accLabel = new QLabel("해지할 계좌 선택");
    accLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #6B7280;");
    QComboBox *combo = new QComboBox;
    combo->setFixedHeight(40);
    combo->addItems(m_accountList);
    combo->setStyleSheet(S_COMBO);

    QLabel *pwLabel = new QLabel("계좌 비밀번호");
    pwLabel->setStyleSheet("font-size: 12px; font-weight: 600; color: #6B7280;");
    QLineEdit *pwEdit = new QLineEdit;
    pwEdit->setEchoMode(QLineEdit::Password);
    pwEdit->setPlaceholderText("계좌 비밀번호를 입력하세요");
    pwEdit->setFixedHeight(40);
    pwEdit->setStyleSheet(S_INPUT);

    layout->addWidget(accLabel);
    layout->addWidget(combo);
    layout->addWidget(pwLabel);
    layout->addWidget(pwEdit);
    layout->addSpacing(12);

    QPushButton *btnOk     = new QPushButton("해지");
    QPushButton *btnCancel = new QPushButton("취소");
    btnOk->setFixedHeight(42);
    btnCancel->setFixedHeight(42);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnCancel->setCursor(Qt::PointingHandCursor);
    btnOk->setStyleSheet(S_BTN_RED);
    btnCancel->setStyleSheet(S_BTN_GRAY);

    QHBoxLayout *btnRow = new QHBoxLayout;
    btnRow->setSpacing(10);
    btnRow->addWidget(btnOk);
    btnRow->addWidget(btnCancel);
    layout->addLayout(btnRow);

    connect(btnCancel, &QPushButton::clicked, dlg, &QDialog::reject);

    connect(btnOk, &QPushButton::clicked, dlg, [=]() {
        if (pwEdit->text().isEmpty()) {
            QMessageBox::warning(dlg, "오류", "계좌 비밀번호를 입력해주세요.");
            return;
        }

        auto reply = QMessageBox::question(dlg, "계좌 해지",
                                           QString("계좌 [%1]을 정말 해지하시겠습니까?").arg(combo->currentText()),
                                           QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) return;

        // 버튼 비활성화 (중복 클릭 방지)
        btnOk->setEnabled(false);
        btnCancel->setEnabled(false);

        QJsonObject data;
        data["accountNumber"]   = combo->currentText();
        data["accountPassword"] = pwEdit->text();

        QJsonObject request;
        request["type"]  = "close_account";
        request["token"] = SessionContext::instance().token();
        request["data"]  = data;
        NetworkClient::instance()->sendRequest(request);

        // 응답은 MyInfoPage::onNetworkResponse 에서 처리
        // 성공 시 다이얼로그를 거기서 닫음
        // dlg 포인터를 onNetworkResponse 에서 쓸 수 있도록 연결
        connect(NetworkClient::instance(), &NetworkClient::responseReceived,
                dlg, [dlg](const QJsonObject &resp) {
                    if (resp["type"].toString() == "close_account_response")
                        dlg->accept();   // 응답 받으면 그때 닫기
                });
    });

    dlg->exec();
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
        m_createdAtLabel->setText(user["createdAt"].toString());
    }

    else if (type == "list_accounts_response") {
        if (status != "success") return;
        m_accountList.clear();
        const QJsonArray accounts = resp["data"].toObject()["accounts"].toArray();
        for (const QJsonValue &v : accounts)
            m_accountList << v.toObject()["accountNumber"].toString();
    }

    else if (type == "change_password_response") {
        auto *box = new QMessageBox(
            status == "success" ? QMessageBox::Information : QMessageBox::Critical,
            status == "success" ? "완료" : "실패",
            status == "success" ? "비밀번호가 변경되었습니다." : resp["message"].toString(),
            QMessageBox::Ok, this);
        box->setAttribute(Qt::WA_DeleteOnClose);
        box->open();
    }

    else if (type == "close_account_response") {
        if (status == "success") {
            auto *box = new QMessageBox(QMessageBox::Information, "완료",
                                        "계좌가 해지되었습니다.", QMessageBox::Ok, this);
            box->setAttribute(Qt::WA_DeleteOnClose);
            box->open();
            loadUserInfo();
        } else {
            auto *box = new QMessageBox(QMessageBox::Critical, "실패",
                                        resp["message"].toString(), QMessageBox::Ok, this);
            box->setAttribute(Qt::WA_DeleteOnClose);
            box->open();
        }
    }
}