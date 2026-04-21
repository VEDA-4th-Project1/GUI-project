#include "newaccountpage.h"
#include "networkclient.h"
#include "sessioncontext.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QFrame>
#include <QMessageBox>
#include <QJsonObject>

NewAccountPage::NewAccountPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    connect(NetworkClient::instance(), &NetworkClient::responseReceived,
            this, &NewAccountPage::onNetworkResponse);
}

void NewAccountPage::setupUI() {
    setStyleSheet(
        "background-color: #F5F7FB;"
        "QLabel { color: #111827; background: transparent; border: none; }"
    );

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(40, 40, 40, 40);
    root->setSpacing(0);
    root->addStretch();

    // ── 계좌 개설 카드 ────────────────────────────────────────────────────────
    QFrame* card = new QFrame(this);
    card->setStyleSheet(
        "QFrame { background-color: white; border-radius: 16px; border: 1px solid #E5E7EB; }");
    card->setMaximumWidth(560);

    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(32, 28, 32, 28);
    cl->setSpacing(16);

    QLabel* title = new QLabel("신규 계좌 개설", card);
    title->setStyleSheet(
        "font-size: 20px; font-weight: 700; color: #111827; border: none;");
    cl->addWidget(title);
    cl->addSpacing(4);

    const QString inputStyle =
        "QLineEdit, QDoubleSpinBox, QComboBox {"
        "  background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
        "  border-radius: 8px; color: #111827; font-size: 13px; padding: 0 12px; }"
        "QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus {"
        "  border-color: #2563EB; background-color: white; }";
    const QString labelStyle =
        "color: #374151; font-size: 13px; font-weight: 600; border: none; min-width: 120px;";
    const QString fixedValStyle =
        "color: #6B7280; font-size: 13px; border: 1.5px solid #E5E7EB;"
        "border-radius: 8px; background-color: #F3F4F6; padding: 0 12px;";

    auto addRow = [&](const QString& labelText, QWidget* field) {
        QHBoxLayout* row = new QHBoxLayout();
        QLabel* lbl = new QLabel(labelText, card);
        lbl->setStyleSheet(labelStyle);
        row->addWidget(lbl);
        row->addWidget(field, 1);
        cl->addLayout(row);
    };

    // 계좌 종류
    m_typeCombo = new QComboBox(card);
    m_typeCombo->addItem("저축 계좌 (savings)",    "savings");
    m_typeCombo->addItem("입출금 계좌 (checking)", "checking");
    m_typeCombo->setFixedHeight(40);
    m_typeCombo->setStyleSheet(inputStyle +
        "QComboBox::drop-down { border: none; width: 28px; }");
    addRow("계좌 종류", m_typeCombo);

    // 계좌 비밀번호
    m_accPasswordEdit = new QLineEdit(card);
    m_accPasswordEdit->setPlaceholderText("4자 이상");
    m_accPasswordEdit->setEchoMode(QLineEdit::Password);
    m_accPasswordEdit->setFixedHeight(40);
    m_accPasswordEdit->setStyleSheet(inputStyle);
    addRow("계좌 비밀번호", m_accPasswordEdit);

    // 초기 입금액
    m_initBalanceSpin = new QDoubleSpinBox(card);
    m_initBalanceSpin->setRange(0, 999999999);
    m_initBalanceSpin->setSingleStep(10000);
    m_initBalanceSpin->setDecimals(0);
    m_initBalanceSpin->setSuffix(" 원");
    m_initBalanceSpin->setFixedHeight(40);
    m_initBalanceSpin->setStyleSheet(inputStyle);
    addRow("초기 입금액", m_initBalanceSpin);

    // 이자율 — 저축계좌 전용, 고정값(3%) 표시만
    m_rateWidget = new QWidget(card);
    m_rateWidget->setStyleSheet("QWidget { border: none; background: transparent; }");
    QHBoxLayout* rateRow = new QHBoxLayout(m_rateWidget);
    rateRow->setContentsMargins(0, 0, 0, 0);
    QLabel* rateLabel = new QLabel("이자율", m_rateWidget);
    rateLabel->setStyleSheet(labelStyle);
    QLabel* rateVal = new QLabel("연 3.0% (고정)", m_rateWidget);
    rateVal->setFixedHeight(40);
    rateVal->setStyleSheet(fixedValStyle);
    rateRow->addWidget(rateLabel);
    rateRow->addWidget(rateVal, 1);
    cl->addWidget(m_rateWidget);

    // 마이너스 한도 — 입출금계좌 전용, 0원 고정 표시만
    m_limitWidget = new QWidget(card);
    m_limitWidget->setStyleSheet("QWidget { border: none; background: transparent; }");
    QHBoxLayout* limitRow = new QHBoxLayout(m_limitWidget);
    limitRow->setContentsMargins(0, 0, 0, 0);
    QLabel* limitLabel = new QLabel("마이너스 한도", m_limitWidget);
    limitLabel->setStyleSheet(labelStyle);
    QLabel* limitVal = new QLabel("0 원 (고정)", m_limitWidget);
    limitVal->setFixedHeight(40);
    limitVal->setStyleSheet(fixedValStyle);
    limitRow->addWidget(limitLabel);
    limitRow->addWidget(limitVal, 1);
    m_limitWidget->setVisible(false);
    cl->addWidget(m_limitWidget);

    cl->addSpacing(4);

    // 개설 버튼
    m_createBtn = new QPushButton("계좌 개설", card);
    m_createBtn->setFixedHeight(46);
    m_createBtn->setCursor(Qt::PointingHandCursor);
    m_createBtn->setStyleSheet(
        "QPushButton { background-color: #2563EB; color: white; border: none;"
        "  border-radius: 10px; font-size: 14px; font-weight: 700; }"
        "QPushButton:hover    { background-color: #1D4ED8; }"
        "QPushButton:pressed  { background-color: #1E40AF; }"
        "QPushButton:disabled { background-color: #93C5FD; }");
    cl->addWidget(m_createBtn);

    // 카드를 가운데 정렬
    QHBoxLayout* centerRow = new QHBoxLayout();
    centerRow->addStretch();
    centerRow->addWidget(card);
    centerRow->addStretch();
    root->addLayout(centerRow);
    root->addStretch();

    connect(m_createBtn, &QPushButton::clicked, this, &NewAccountPage::onCreateClicked);
    connect(m_typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NewAccountPage::onTypeChanged);
}

void NewAccountPage::onTypeChanged(int index) {
    bool isSavings = (m_typeCombo->itemData(index).toString() == "savings");
    m_rateWidget->setVisible(isSavings);
    m_limitWidget->setVisible(!isSavings);
}

void NewAccountPage::onCreateClicked() {
    if (m_accPasswordEdit->text().length() < 4) {
        QMessageBox::warning(this, "입력 오류", "계좌 비밀번호는 4자 이상이어야 합니다.");
        return;
    }

    m_createBtn->setEnabled(false);

    const QString type = m_typeCombo->currentData().toString();
    QJsonObject data;
    data["type"]            = type;
    data["accountPassword"] = m_accPasswordEdit->text();
    data["initBalance"]     = m_initBalanceSpin->value();
    if (type == "savings")
        data["interestRate"]   = 0.03;   // 고정 이율
    else
        data["overdraftLimit"] = 0.0;    // 마이너스 한도 0 고정

    QJsonObject req;
    req["type"]  = "create_account";
    req["token"] = SessionContext::instance().token();
    req["data"]  = data;
    NetworkClient::instance()->sendRequest(req);
}

static void showBox(QWidget *parent, QMessageBox::Icon icon,
                    const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->open();
}

void NewAccountPage::onNetworkResponse(const QJsonObject& resp) {
    if (resp["type"].toString() != "create_account_response") return;

    m_createBtn->setEnabled(true);

    if (resp["status"].toString() == "success") {
        const QString accNum = resp["data"].toObject()["accountNumber"].toString();
        showBox(this, QMessageBox::Information, "계좌 개설",
                QString("계좌가 개설되었습니다.\n계좌번호: %1").arg(accNum));
        m_accPasswordEdit->clear();
        m_initBalanceSpin->setValue(0);
    } else {
        showBox(this, QMessageBox::Warning, "계좌 개설 실패", resp["message"].toString());
    }
}
