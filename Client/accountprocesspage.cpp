#include "accountprocesspage.h"
#include "networkclient.h"
#include "sessioncontext.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QLineEdit>
#include <QScrollArea>
#include <QComboBox>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QButtonGroup>
#include <QMessageBox>
#include <QShowEvent>
#include <QJsonObject>
#include <QJsonArray>

AccountProcessPage::AccountProcessPage(QWidget *parent)
    : QWidget(parent),
    m_leftCard(nullptr),
    m_rightCard(nullptr),
    m_titleLabel(nullptr),
    m_accountCombo(nullptr),
    m_accountNumberLabel(nullptr),
    m_balanceLabel(nullptr),
    m_historyContainer(nullptr),
    m_historyLayout(nullptr),
    m_totalLabel(nullptr),
    m_detailTitleLabel(nullptr),
    m_selectedAccountLabel(nullptr),
    m_selectedTypeLabel(nullptr),
    m_selectedBalanceLabel(nullptr),
    m_accountPasswordEdit(nullptr),
    m_depositRadio(nullptr),
    m_withdrawRadio(nullptr),
    m_amountSpin(nullptr),
    m_descriptionEdit(nullptr),
    m_executeBtn(nullptr),
    m_currentBalance(0),
    m_historyCount(0)
{
    setupUI();
    updateTotalLabel();

    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onResponseReceived(QJsonObject)));

    loadAccounts();
}

void AccountProcessPage::setupUI()
{
    setStyleSheet("background-color: #F5F7FB;");

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    m_leftCard = new QFrame(this);
    m_leftCard->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border: 1px solid #E5E7EB;"
        " border-radius: 18px;"
        "}"
        );

    m_rightCard = new QFrame(this);
    m_rightCard->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border: 1px solid #E5E7EB;"
        " border-radius: 18px;"
        "}"
        );

    mainLayout->addWidget(m_leftCard, 11);
    mainLayout->addWidget(m_rightCard, 9);

    setupLeftPanel();
    setupRightPanel();
}

void AccountProcessPage::setupLeftPanel()
{
    QVBoxLayout *leftLayout = new QVBoxLayout(m_leftCard);
    leftLayout->setContentsMargins(25, 25, 25, 25);
    leftLayout->setSpacing(15);

    m_titleLabel = new QLabel("계좌 조회", m_leftCard);
    m_titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    m_accountCombo = new QComboBox(m_leftCard);
    m_accountCombo->setFixedHeight(40);
    m_accountCombo->setStyleSheet(
        "QComboBox {"
        " background-color: #F9FAFB;"
        " border: 1px solid #D1D5DB;"
        " border-radius: 8px;"
        " padding: 0 12px;"
        " font-size: 13px;"
        " color: #111827;"
        "}"
        "QComboBox QAbstractItemView {"
        " background-color: white;"
        " color: #111827;"
        " selection-background-color: #EFF6FF;"
        " selection-color: #2563EB;"
        "}"
        );

    m_accountNumberLabel = new QLabel("계좌번호 : -", m_leftCard);
    m_accountNumberLabel->setStyleSheet(
        "font-size: 16px;"
        "font-weight: 600;"
        "color: #374151;"
        );

    m_balanceLabel = new QLabel("잔액 : 0원", m_leftCard);
    m_balanceLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 700;"
        "color: #2563EB;"
        );

    QLabel *historyHeader = new QLabel("입/출   |   계좌번호   |   금액 / 설명", m_leftCard);
    historyHeader->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 700;"
        "color: #111827;"
        "padding: 8px 0;"
        "border-bottom: 1px solid #E5E7EB;"
        );

    QScrollArea *scrollArea = new QScrollArea(m_leftCard);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("QScrollArea { background: transparent; border: none; }");
    scrollArea->viewport()->setStyleSheet("background: transparent;");

    m_historyContainer = new QWidget(scrollArea);
    m_historyContainer->setStyleSheet("background: transparent;");
    m_historyLayout = new QVBoxLayout(m_historyContainer);
    m_historyLayout->setContentsMargins(0, 0, 0, 0);
    m_historyLayout->setSpacing(8);
    m_historyLayout->addStretch();

    scrollArea->setWidget(m_historyContainer);

    m_totalLabel = new QLabel(m_leftCard);
    m_totalLabel->setStyleSheet(
        "font-size: 14px;"
        "font-weight: 700;"
        "color: #374151;"
        "padding-top: 8px;"
        "border-top: 1px solid #E5E7EB;"
        );

    leftLayout->addWidget(m_titleLabel);
    leftLayout->addWidget(m_accountCombo);
    leftLayout->addWidget(m_accountNumberLabel);
    leftLayout->addWidget(m_balanceLabel);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(historyHeader);
    leftLayout->addWidget(scrollArea, 1);
    leftLayout->addWidget(m_totalLabel);

    connect(m_accountCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onAccountChanged(int)));
}

void AccountProcessPage::setupRightPanel()
{
    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightCard);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(14);

    m_detailTitleLabel = new QLabel("계좌 상세 / 입출금", m_rightCard);
    m_detailTitleLabel->setStyleSheet(
        "font-size: 22px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    QWidget *detailBox = new QWidget(m_rightCard);
    detailBox->setStyleSheet("QWidget { background: transparent; } QLabel { color: #374151; background: transparent; }");
    QGridLayout *grid = new QGridLayout(detailBox);
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(12);

    QLabel *accTitle = new QLabel("계좌번호", detailBox);
    QLabel *typeTitle = new QLabel("타입", detailBox);
    QLabel *balanceTitle = new QLabel("잔액", detailBox);
    QLabel *pwTitle = new QLabel("계좌 비밀번호", detailBox);
    QLabel *amountTitle = new QLabel("금액", detailBox);
    QLabel *descTitle = new QLabel("설명", detailBox);

    m_selectedAccountLabel = new QLabel("-", detailBox);
    m_selectedTypeLabel = new QLabel("-", detailBox);
    m_selectedBalanceLabel = new QLabel("0원", detailBox);

    const QString inputStyle =
        "QLineEdit, QDoubleSpinBox {"
        " background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
        " border-radius: 8px; color: #111827; font-size: 13px; padding: 0 10px; }"
        "QLineEdit:focus, QDoubleSpinBox:focus {"
        " border-color: #2563EB; background-color: white; }";
    const QString radioStyle =
        "QRadioButton { color: #374151; background: transparent; font-size: 13px; }"
        "QRadioButton::indicator { width: 16px; height: 16px; }";

    m_accountPasswordEdit = new QLineEdit(detailBox);
    m_accountPasswordEdit->setEchoMode(QLineEdit::Password);
    m_accountPasswordEdit->setPlaceholderText("계좌 비밀번호 입력");
    m_accountPasswordEdit->setFixedHeight(38);
    m_accountPasswordEdit->setStyleSheet(inputStyle);

    m_depositRadio = new QRadioButton("입금", detailBox);
    m_withdrawRadio = new QRadioButton("출금", detailBox);
    m_depositRadio->setChecked(true);
    m_depositRadio->setStyleSheet(radioStyle);
    m_withdrawRadio->setStyleSheet(radioStyle);

    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(m_depositRadio);
    group->addButton(m_withdrawRadio);

    QWidget *radioBox = new QWidget(detailBox);
    QHBoxLayout *radioLayout = new QHBoxLayout(radioBox);
    radioLayout->setContentsMargins(0, 0, 0, 0);
    radioLayout->setSpacing(12);
    radioLayout->addWidget(m_depositRadio);
    radioLayout->addWidget(m_withdrawRadio);
    radioLayout->addStretch();

    m_amountSpin = new QDoubleSpinBox(detailBox);
    m_amountSpin->setRange(0, 999999999);
    m_amountSpin->setDecimals(0);
    m_amountSpin->setSingleStep(10000);
    m_amountSpin->setSuffix(" 원");
    m_amountSpin->setFixedHeight(38);
    m_amountSpin->setStyleSheet(inputStyle);

    m_descriptionEdit = new QLineEdit(detailBox);
    m_descriptionEdit->setPlaceholderText("설명 입력");
    m_descriptionEdit->setFixedHeight(38);
    m_descriptionEdit->setStyleSheet(inputStyle);

    m_executeBtn = new QPushButton("실행", detailBox);
    m_executeBtn->setFixedHeight(42);
    m_executeBtn->setStyleSheet(
        "QPushButton {"
        " background-color: #2563EB;"
        " color: white;"
        " border: none;"
        " border-radius: 8px;"
        " font-weight: 700;"
        "}"
        "QPushButton:hover {"
        " background-color: #1D4ED8;"
        "}"
        );

    grid->addWidget(accTitle, 0, 0);
    grid->addWidget(m_selectedAccountLabel, 0, 1);

    grid->addWidget(typeTitle, 1, 0);
    grid->addWidget(m_selectedTypeLabel, 1, 1);

    grid->addWidget(balanceTitle, 2, 0);
    grid->addWidget(m_selectedBalanceLabel, 2, 1);

    grid->addWidget(pwTitle, 3, 0);
    grid->addWidget(m_accountPasswordEdit, 3, 1);

    grid->addWidget(new QLabel("거래 종류", detailBox), 4, 0);
    grid->addWidget(radioBox, 4, 1);

    grid->addWidget(amountTitle, 5, 0);
    grid->addWidget(m_amountSpin, 5, 1);

    grid->addWidget(descTitle, 6, 0);
    grid->addWidget(m_descriptionEdit, 6, 1);

    grid->addWidget(m_executeBtn, 7, 0, 1, 2);

    rightLayout->addWidget(m_detailTitleLabel);
    rightLayout->addWidget(detailBox);
    rightLayout->addStretch();

    connect(m_executeBtn, SIGNAL(clicked()),
            this, SLOT(onExecuteClicked()));
}

void AccountProcessPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);  // 페이지가 표시될 때마다 계좌 목록 갱신
    loadAccounts();
}

void AccountProcessPage::loadAccounts()
{
    QJsonObject request;
    request["type"] = "list_accounts";
    request["token"] = SessionContext::instance().token();
    request["data"] = QJsonObject{};

    NetworkClient::instance()->sendRequest(request);
}

void AccountProcessPage::onAccountChanged(int index)
{
    if (index < 0 || index >= m_allAccounts.size())
        return;

    QJsonObject acc = m_allAccounts[index].toObject();

    m_currentAccountNumber = acc["accountNumber"].toString();
    m_currentAccountType = acc["type"].toString();
    m_currentBalance = static_cast<int>(acc["balance"].toDouble());

    m_accountNumberLabel->setText(QString("계좌번호 : %1").arg(m_currentAccountNumber));
    m_balanceLabel->setText(QString("잔액 : %1원").arg(m_currentBalance));

    m_selectedAccountLabel->setText(m_currentAccountNumber);
    m_selectedTypeLabel->setText(m_currentAccountType);
    m_selectedBalanceLabel->setText(QString("%1원").arg(m_currentBalance));

    clearHistoryLabels();
    updateTotalLabel();

    if (!m_accountPasswordEdit->text().trimmed().isEmpty()) {
        loadAccountDetail(m_currentAccountNumber);
    }
}

void AccountProcessPage::loadAccountDetail(const QString &accountNumber)
{
    QString password = m_accountPasswordEdit->text().trimmed();
    if (password.isEmpty())
        return;

    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["accountPassword"] = password;

    QJsonObject request;
    request["type"] = "get_account_detail";
    request["token"] = SessionContext::instance().token();
    request["data"] = data;

    NetworkClient::instance()->sendRequest(request);
}

void AccountProcessPage::onExecuteClicked()
{
    if (m_currentAccountNumber.isEmpty()) {
        QMessageBox::warning(this, "오류", "계좌를 먼저 선택하세요.");
        return;
    }

    if (m_accountPasswordEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "오류", "계좌 비밀번호를 입력하세요.");
        return;
    }

    if (m_amountSpin->value() <= 0) {
        QMessageBox::warning(this, "오류", "금액을 입력하세요.");
        return;
    }

    bool isDeposit = m_depositRadio->isChecked();
    QString reqType = isDeposit ? "deposit" : "withdraw";

    QJsonObject data;
    data["accountNumber"] = m_currentAccountNumber;
    data["accountPassword"] = m_accountPasswordEdit->text();
    data["amount"] = m_amountSpin->value();
    data["description"] = m_descriptionEdit->text().trimmed().isEmpty()
                              ? (isDeposit ? "입금" : "출금")
                              : m_descriptionEdit->text().trimmed();

    QJsonObject request;
    request["type"] = reqType;
    request["token"] = SessionContext::instance().token();
    request["data"] = data;

    NetworkClient::instance()->sendRequest(request);
}

void AccountProcessPage::clearHistoryLabels()
{
    while (m_historyLayout->count() > 1) {
        QLayoutItem *item = m_historyLayout->takeAt(0);
        if (item->widget())
            delete item->widget();
        delete item;
    }
    m_historyCount = 0;
}

void AccountProcessPage::addHistoryLabel(const QString &type, const QString &accountNumber, const QString &text)
{
    QLabel *historyLabel = new QLabel(
        QString("%1   |   %2   |   %3")
            .arg(type)
            .arg(accountNumber)
            .arg(text),
        m_historyContainer
        );

    historyLabel->setStyleSheet(
        "font-size: 13px;"
        "color: #374151;"
        "background-color: #F9FAFB;"
        "border: 1px solid #E5E7EB;"
        "border-radius: 8px;"
        "padding: 8px;"
        );

    m_historyLayout->insertWidget(m_historyLayout->count() - 1, historyLabel);
    m_historyCount++;
    updateTotalLabel();
}

void AccountProcessPage::updateTotalLabel()
{
    m_totalLabel->setText(QString("TOTAL : %1건").arg(m_historyCount));
}

static void showBox(QWidget *parent, QMessageBox::Icon icon,
                    const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->open();
}

void AccountProcessPage::onResponseReceived(const QJsonObject &response)
{
    QString type = response["type"].toString();
    QString status = response["status"].toString();

    if (type == "list_accounts_response") {
        if (status != "success") {
            showBox(this, QMessageBox::Warning, "오류", response["message"].toString());
            return;
        }

        QString previous = m_currentAccountNumber;
        m_allAccounts = response["data"].toObject()["accounts"].toArray();

        m_accountCombo->blockSignals(true);
        m_accountCombo->clear();

        int selectedIndex = -1;

        for (int i = 0; i < m_allAccounts.size(); ++i) {
            QJsonObject acc = m_allAccounts[i].toObject();
            QString accNum = acc["accountNumber"].toString();
            QString typeText = acc["type"].toString();

            m_accountCombo->addItem(QString("%1 (%2)").arg(accNum, typeText), accNum);

            if (accNum == previous)
                selectedIndex = i;
        }

        m_accountCombo->blockSignals(false);

        if (!m_allAccounts.isEmpty()) {
            if (selectedIndex < 0)
                selectedIndex = 0;

            m_accountCombo->setCurrentIndex(selectedIndex);
        } else {
            m_currentAccountNumber.clear();
            m_currentAccountType.clear();
            m_currentBalance = 0;

            m_accountNumberLabel->setText("계좌번호 : -");
            m_balanceLabel->setText("잔액 : 0원");
            m_selectedAccountLabel->setText("-");
            m_selectedTypeLabel->setText("-");
            m_selectedBalanceLabel->setText("0원");

            clearHistoryLabels();
            updateTotalLabel();
        }
    }
    else if (type == "get_account_detail_response") {
        if (status != "success") {
            showBox(this, QMessageBox::Warning, "오류", response["message"].toString());
            return;
        }

        QJsonObject account = response["data"].toObject()["account"].toObject();

        m_currentAccountNumber = account["accountNumber"].toString();
        m_currentAccountType = account["type"].toString();
        m_currentBalance = static_cast<int>(account["balance"].toDouble());

        m_accountNumberLabel->setText(QString("계좌번호 : %1").arg(m_currentAccountNumber));
        m_balanceLabel->setText(QString("잔액 : %1원").arg(m_currentBalance));

        m_selectedAccountLabel->setText(m_currentAccountNumber);
        m_selectedTypeLabel->setText(m_currentAccountType);
        m_selectedBalanceLabel->setText(QString("%1원").arg(m_currentBalance));

        clearHistoryLabels();

        QJsonArray transactions = account["transactions"].toArray();
        for (const auto &v : transactions) {
            QJsonObject tx = v.toObject();
            QString typeText = tx["type"].toString();
            QString text = QString("%1원 / %2")
                               .arg(tx["amount"].toDouble(), 0, 'f', 0)
                               .arg(tx["description"].toString());
            addHistoryLabel(typeText, m_currentAccountNumber, text);
        }
    }
    else if (type == "deposit_response" || type == "withdraw_response") {
        if (status != "success") {
            showBox(this, QMessageBox::Warning, "오류", response["message"].toString());
            return;
        }

        m_currentBalance = static_cast<int>(response["data"].toObject()["balance"].toDouble());
        m_balanceLabel->setText(QString("잔액 : %1원").arg(m_currentBalance));
        m_selectedBalanceLabel->setText(QString("%1원").arg(m_currentBalance));

        QString txType = (type == "deposit_response") ? "deposit" : "withdraw";
        QString desc = m_descriptionEdit->text().trimmed().isEmpty()
                           ? ((txType == "deposit") ? "입금" : "출금")
                           : m_descriptionEdit->text().trimmed();

        addHistoryLabel(txType, m_currentAccountNumber,
                        QString("%1원 / %2").arg(m_amountSpin->value(), 0, 'f', 0).arg(desc));

        showBox(this, QMessageBox::Information, "완료", response["message"].toString());

        m_amountSpin->setValue(0);
        m_descriptionEdit->clear();

        loadAccounts();
        loadAccountDetail(m_currentAccountNumber);
    }
}