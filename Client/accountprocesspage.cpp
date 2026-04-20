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
#include <QTableWidget>
#include <QHeaderView>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QMessageBox>
#include <QButtonGroup>
#include <QAbstractItemView>
#include <QTableWidgetItem>

AccountProcessPage::AccountProcessPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();

    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onResponseReceived(QJsonObject)));

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
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(12);

    m_leftTitleLabel = new QLabel("계좌 목록", m_leftCard);
    m_leftTitleLabel->setStyleSheet(
        "font-size: 22px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    m_searchEdit = new QLineEdit(m_leftCard);
    m_searchEdit->setPlaceholderText("계좌번호 또는 타입 검색");
    m_searchEdit->setFixedHeight(40);

    m_accountTable = new QTableWidget(m_leftCard);
    m_accountTable->setColumnCount(3);
    m_accountTable->setHorizontalHeaderLabels(QStringList() << "계좌번호" << "타입" << "잔액");
    m_accountTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_accountTable->verticalHeader()->setVisible(false);
    m_accountTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_accountTable->setSelectionMode(QAbstractItemView::SingleSelection);
    m_accountTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_accountTable->setAlternatingRowColors(true);

    leftLayout->addWidget(m_leftTitleLabel);
    leftLayout->addWidget(m_searchEdit);
    leftLayout->addWidget(m_accountTable, 1);

    connect(m_searchEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onSearchChanged(QString)));

    connect(m_accountTable, SIGNAL(cellClicked(int,int)),
            this, SLOT(onAccountSelected(int,int)));
}

void AccountProcessPage::setupRightPanel()
{
    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightCard);
    rightLayout->setContentsMargins(12, 12, 12, 12);
    rightLayout->setSpacing(12);

    m_rightTitleLabel = new QLabel("선택 계좌 상세", m_rightCard);
    m_rightTitleLabel->setStyleSheet(
        "font-size: 22px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    QWidget *detailBox = new QWidget(m_rightCard);
    QGridLayout *detailLayout = new QGridLayout(detailBox);
    detailLayout->setHorizontalSpacing(10);
    detailLayout->setVerticalSpacing(10);

    QLabel *accountNumberLabel = new QLabel("계좌번호", detailBox);
    QLabel *accountTypeLabel = new QLabel("타입", detailBox);
    QLabel *balanceLabel = new QLabel("잔액", detailBox);
    QLabel *passwordLabel = new QLabel("계좌 비밀번호", detailBox);
    QLabel *amountLabel = new QLabel("금액", detailBox);
    QLabel *descLabel = new QLabel("설명", detailBox);

    m_accountNumberValueLabel = new QLabel("-", detailBox);
    m_accountTypeValueLabel = new QLabel("-", detailBox);
    m_balanceValueLabel = new QLabel("0원", detailBox);

    m_accountPasswordEdit = new QLineEdit(detailBox);
    m_accountPasswordEdit->setEchoMode(QLineEdit::Password);
    m_accountPasswordEdit->setPlaceholderText("계좌 비밀번호 입력");
    m_accountPasswordEdit->setFixedHeight(36);

    m_depositRadio = new QRadioButton("입금", detailBox);
    m_withdrawRadio = new QRadioButton("출금", detailBox);
    m_depositRadio->setChecked(true);

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
    m_amountSpin->setMaximum(1000000000.0);
    m_amountSpin->setDecimals(0);
    m_amountSpin->setSuffix(" 원");
    m_amountSpin->setFixedHeight(36);

    m_descriptionEdit = new QLineEdit(detailBox);
    m_descriptionEdit->setPlaceholderText("거래 설명 입력");
    m_descriptionEdit->setFixedHeight(36);

    m_executeBtn = new QPushButton("실행", detailBox);
    m_executeBtn->setFixedHeight(40);
    m_executeBtn->setStyleSheet(
        "QPushButton {"
        " background-color: #2563EB;"
        " color: white;"
        " border: none;"
        " border-radius: 8px;"
        " font-weight: 700;"
        "}"
        "QPushButton:hover {"
        " background-color: #3B82F6;"
        "}"
        );

    detailLayout->addWidget(accountNumberLabel, 0, 0);
    detailLayout->addWidget(m_accountNumberValueLabel, 0, 1);

    detailLayout->addWidget(accountTypeLabel, 1, 0);
    detailLayout->addWidget(m_accountTypeValueLabel, 1, 1);

    detailLayout->addWidget(balanceLabel, 2, 0);
    detailLayout->addWidget(m_balanceValueLabel, 2, 1);

    detailLayout->addWidget(passwordLabel, 3, 0);
    detailLayout->addWidget(m_accountPasswordEdit, 3, 1);

    detailLayout->addWidget(new QLabel("거래 종류", detailBox), 4, 0);
    detailLayout->addWidget(radioBox, 4, 1);

    detailLayout->addWidget(amountLabel, 5, 0);
    detailLayout->addWidget(m_amountSpin, 5, 1);

    detailLayout->addWidget(descLabel, 6, 0);
    detailLayout->addWidget(m_descriptionEdit, 6, 1);

    detailLayout->addWidget(m_executeBtn, 7, 0, 1, 2);

    QLabel *historyTitle = new QLabel("거래내역", m_rightCard);
    historyTitle->setStyleSheet(
        "font-size: 18px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    m_transactionTable = new QTableWidget(m_rightCard);
    m_transactionTable->setColumnCount(4);
    m_transactionTable->setHorizontalHeaderLabels(QStringList() << "일시" << "타입" << "금액" << "설명");
    m_transactionTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_transactionTable->verticalHeader()->setVisible(false);
    m_transactionTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_transactionTable->setAlternatingRowColors(true);

    rightLayout->addWidget(m_rightTitleLabel);
    rightLayout->addWidget(detailBox);
    rightLayout->addWidget(historyTitle);
    rightLayout->addWidget(m_transactionTable, 1);

    connect(m_executeBtn, SIGNAL(clicked()),
            this, SLOT(onTransactionClicked()));
}

void AccountProcessPage::refreshTable(const QJsonArray &accounts)
{
    m_accountTable->setRowCount(accounts.size());

    for (int i = 0; i < accounts.size(); ++i) {
        QJsonObject acc = accounts[i].toObject();

        QString accountNumber = acc["accountNumber"].toString();
        QString type = acc["type"].toString();
        double balance = acc["balance"].toDouble();

        m_accountTable->setItem(i, 0, new QTableWidgetItem(accountNumber));
        m_accountTable->setItem(i, 1, new QTableWidgetItem(type));
        m_accountTable->setItem(i, 2, new QTableWidgetItem(QString("%1원").arg(balance, 0, 'f', 0)));
    }
}

void AccountProcessPage::onSearchChanged(const QString &keyword)
{
    if (keyword.isEmpty()) {
        refreshTable(m_allAccounts);
        return;
    }

    QJsonArray filtered;
    for (const auto &v : m_allAccounts) {
        QJsonObject acc = v.toObject();
        if (acc["accountNumber"].toString().contains(keyword, Qt::CaseInsensitive) ||
            acc["type"].toString().contains(keyword, Qt::CaseInsensitive)) {
            filtered.append(acc);
        }
    }

    refreshTable(filtered);
}

void AccountProcessPage::onAccountSelected(int row, int)
{
    QTableWidgetItem *item = m_accountTable->item(row, 0);
    if (!item)
        return;

    m_selectedAccountNumber = item->text();
    loadAccountDetail(m_selectedAccountNumber);
}

void AccountProcessPage::loadAccountDetail(const QString &accountNumber)
{
    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["accountPassword"] = m_accountPasswordEdit->text();

    QJsonObject request;
    request["type"] = "get_account_detail";
    request["token"] = SessionContext::instance().token();
    request["data"] = data;

    NetworkClient::instance()->sendRequest(request);
}

void AccountProcessPage::showDetail(const QJsonObject &account)
{
    m_accountNumberValueLabel->setText(account["accountNumber"].toString());
    m_accountTypeValueLabel->setText(account["type"].toString());
    m_balanceValueLabel->setText(QString("%1원").arg(account["balance"].toDouble(), 0, 'f', 0));

    QJsonArray transactions = account["transactions"].toArray();
    m_transactionTable->setRowCount(transactions.size());

    for (int i = 0; i < transactions.size(); ++i) {
        QJsonObject tx = transactions[i].toObject();

        m_transactionTable->setItem(i, 0, new QTableWidgetItem(tx["timestamp"].toString()));
        m_transactionTable->setItem(i, 1, new QTableWidgetItem(tx["type"].toString()));
        m_transactionTable->setItem(i, 2, new QTableWidgetItem(QString("%1원").arg(tx["amount"].toDouble(), 0, 'f', 0)));
        m_transactionTable->setItem(i, 3, new QTableWidgetItem(tx["description"].toString()));
    }
}

void AccountProcessPage::onTransactionClicked()
{
    if (m_selectedAccountNumber.isEmpty()) {
        QMessageBox::warning(this, "오류", "계좌를 먼저 선택하세요.");
        return;
    }

    bool isDeposit = m_depositRadio->isChecked();
    QString reqType = isDeposit ? "deposit" : "withdraw";

    QJsonObject data;
    data["accountNumber"] = m_selectedAccountNumber;
    data["accountPassword"] = m_accountPasswordEdit->text();
    data["amount"] = m_amountSpin->value();
    data["description"] = m_descriptionEdit->text();

    QJsonObject request;
    request["type"] = reqType;
    request["token"] = SessionContext::instance().token();
    request["data"] = data;

    NetworkClient::instance()->sendRequest(request);
}

void AccountProcessPage::onResponseReceived(const QJsonObject &response)
{
    QString type = response["type"].toString();
    QString status = response["status"].toString();

    if (type == "list_accounts_response") {
        if (status == "success") {
            m_allAccounts = response["data"].toObject()["accounts"].toArray();
            refreshTable(m_allAccounts);
        } else {
            QMessageBox::warning(this, "오류", response["message"].toString());
        }
    }
    else if (type == "get_account_detail_response") {
        if (status == "success") {
            showDetail(response["data"].toObject()["account"].toObject());
        } else {
            QMessageBox::warning(this, "오류", response["message"].toString());
        }
    }
    else if (type == "deposit_response" || type == "withdraw_response") {
        if (status == "success") {
            double newBalance = response["data"].toObject()["balance"].toDouble();
            m_balanceValueLabel->setText(QString("%1원").arg(newBalance, 0, 'f', 0));
            loadAccountDetail(m_selectedAccountNumber);
            loadAccounts();
        } else {
            QMessageBox::warning(this, "오류", response["message"].toString());
        }
    }
}
