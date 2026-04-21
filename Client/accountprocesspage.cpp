#include "accountprocesspage.h"
#include "networkclient.h"
#include "sessioncontext.h"
#include "appstyle.h"

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
#include <QLayoutItem>
#include <QListWidget>
#include <QListWidgetItem>

static void showBox(QWidget *parent, QMessageBox::Icon icon,
                    const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->open();
}

AccountProcessPage::AccountProcessPage(QWidget *parent)
    : QWidget(parent),
    m_leftCard(nullptr),
    m_rightCard(nullptr),
    m_titleLabel(nullptr),
    m_searchEdit(nullptr),
    m_searchResultList(nullptr),
    m_accountNumberLabel(nullptr),
    m_balanceLabel(nullptr),
    m_historyContainer(nullptr),
    m_historyLayout(nullptr),
    m_totalLabel(nullptr),
    m_detailTitleLabel(nullptr),
    m_detailAccountCombo(nullptr),
    m_selectedAccountLabel(nullptr),
    m_selectedTypeLabel(nullptr),
    m_selectedBalanceLabel(nullptr),
    m_accountPasswordEdit(nullptr),
    m_depositRadio(nullptr),
    m_withdrawRadio(nullptr),
    m_amountSpin(nullptr),
    m_descriptionEdit(nullptr),
    m_executeBtn(nullptr),
    m_leftBalance(0),
    m_rightBalance(0),
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
    setStyleSheet(AppStyle::PAGE_BG);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    m_leftCard = new QFrame(this);
    m_leftCard->setStyleSheet(AppStyle::CARD_LARGE);

    m_rightCard = new QFrame(this);
    m_rightCard->setStyleSheet(AppStyle::CARD_LARGE);

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
    m_titleLabel->setStyleSheet(AppStyle::LABEL_TITLE);

    m_searchEdit = new QLineEdit(m_leftCard);
    m_searchEdit->setPlaceholderText("계좌번호로 검색");
    m_searchEdit->setFixedHeight(38);
    m_searchEdit->setStyleSheet(AppStyle::INPUT);

    m_searchResultList = new QListWidget(m_leftCard);
    m_searchResultList->setVisible(false);
    m_searchResultList->setMaximumHeight(120);
    m_searchResultList->setStyleSheet(AppStyle::LIST);

    m_accountNumberLabel = new QLabel("계좌번호 : -", m_leftCard);
    m_accountNumberLabel->setStyleSheet(AppStyle::LABEL_SUBTITLE);

    m_balanceLabel = new QLabel("잔액 : 0원", m_leftCard);
    m_balanceLabel->setStyleSheet(
        "font-size: 20px; font-weight: 800; color: #3182F6; border: none;");

    QLabel *historyHeader = new QLabel("입/출   |   계좌번호   |   금액 / 설명", m_leftCard);
    historyHeader->setStyleSheet(
        "font-size: 13px; font-weight: 700; color: #4E5968;"
        "padding: 8px 0; border-bottom: 1px solid #E5E8EB;");

    QScrollArea *scrollArea = new QScrollArea(m_leftCard);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet(AppStyle::SCROLL_TRANSPARENT);
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
        "font-size: 13px; font-weight: 700; color: #4E5968;"
        "padding-top: 8px; border-top: 1px solid #E5E8EB;");

    leftLayout->addWidget(m_titleLabel);
    leftLayout->addWidget(m_searchEdit);
    leftLayout->addWidget(m_searchResultList);
    leftLayout->addWidget(m_accountNumberLabel);
    leftLayout->addWidget(m_balanceLabel);
    leftLayout->addSpacing(10);
    leftLayout->addWidget(historyHeader);
    leftLayout->addWidget(scrollArea, 1);
    leftLayout->addWidget(m_totalLabel);

    connect(m_searchEdit, SIGNAL(textChanged(QString)),
            this, SLOT(onSearchTextChanged(QString)));

    connect(m_searchResultList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(onSearchResultClicked(QListWidgetItem*)));
}

void AccountProcessPage::setupRightPanel()
{
    QVBoxLayout *rightLayout = new QVBoxLayout(m_rightCard);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(14);

    m_detailTitleLabel = new QLabel("계좌 상세 / 입출금", m_rightCard);
    m_detailTitleLabel->setStyleSheet(AppStyle::LABEL_TITLE);

    m_detailAccountCombo = new QComboBox(m_rightCard);
    m_detailAccountCombo->setFixedHeight(40);
    m_detailAccountCombo->setStyleSheet(AppStyle::COMBO);

    QWidget *detailBox = new QWidget(m_rightCard);
    detailBox->setStyleSheet(
        "QWidget { background: transparent; }"
        "QLabel { color: #4E5968; background: transparent; border: none; font-size: 13px; font-weight: 600; }"
        );

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

    const QString &inputStyle  = AppStyle::INPUT;
    const QString &radioStyle  = AppStyle::RADIO;

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
    m_executeBtn->setStyleSheet(AppStyle::BTN_BLUE);

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
    rightLayout->addWidget(m_detailAccountCombo);
    rightLayout->addWidget(detailBox);
    rightLayout->addStretch();

    connect(m_detailAccountCombo, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onDetailAccountChanged(int)));
    connect(m_executeBtn, SIGNAL(clicked()),
            this, SLOT(onExecuteClicked()));
    connect(m_withdrawRadio, SIGNAL(clicked(bool)),
            this, SLOT(onWithdrawRadioClicked(bool)));

    connect(m_accountPasswordEdit, SIGNAL(returnPressed()),
            this, SLOT(onPasswordReturnPressed()));
}

void AccountProcessPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
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

void AccountProcessPage::updateLeftSummary(const QJsonObject &acc)
{
    m_leftAccountNumber = acc["accountNumber"].toString();
    m_leftBalance = static_cast<int>(acc["balance"].toDouble());

    m_accountNumberLabel->setText(QString("계좌번호 : %1").arg(m_leftAccountNumber));
    m_balanceLabel->setText(QString("잔액 : %1원").arg(m_leftBalance));
}

void AccountProcessPage::updateRightSummary(const QJsonObject &acc)
{
    m_rightAccountNumber = acc["accountNumber"].toString();
    m_rightAccountType = acc["type"].toString();
    m_rightBalance = static_cast<int>(acc["balance"].toDouble());

    m_selectedAccountLabel->setText(m_rightAccountNumber);
    m_selectedTypeLabel->setText(m_rightAccountType);
    m_selectedBalanceLabel->setText(QString("%1원").arg(m_rightBalance));

    if (m_rightAccountType == "savings") {
        m_withdrawRadio->setEnabled(false);
        m_depositRadio->setChecked(true);
    } else {
        m_withdrawRadio->setEnabled(true);
    }
}

void AccountProcessPage::onDetailAccountChanged(int index)
{
    if (index < 0 || index >= m_allAccounts.size())
        return;

    QJsonObject acc = m_allAccounts[index].toObject();
    updateRightSummary(acc);

    QString pw = m_accountPasswordEdit->text().trimmed();
    if (!pw.isEmpty()) {
        loadAccountDetail(m_rightAccountNumber, pw);
    }
}

void AccountProcessPage::onWithdrawRadioClicked(bool checked)
{
    if (!checked)
        return;

    if (m_rightAccountType == "savings") {
        showBox(this, QMessageBox::Warning,
                "출금 불가",
                "저축계좌(savings)는 출금할 수 없습니다.");
        m_depositRadio->setChecked(true);
    }
}

void AccountProcessPage::loadAccountDetail(const QString &accountNumber, const QString &password)
{
    QString pw = password.trimmed();
    if (pw.isEmpty())
        return;

    QJsonObject data;
    data["accountNumber"] = accountNumber;
    data["accountPassword"] = pw;

    QJsonObject request;
    request["type"] = "get_account_detail";
    request["token"] = SessionContext::instance().token();
    request["data"] = data;

    NetworkClient::instance()->sendRequest(request);
}

void AccountProcessPage::tryLoadLeftAccountDetail()
{
    if (m_leftAccountNumber.isEmpty())
        return;

    if (m_accountPasswordCache.contains(m_leftAccountNumber)) {
        loadAccountDetail(m_leftAccountNumber,
                          m_accountPasswordCache.value(m_leftAccountNumber));
    } else {
        m_currentTransactions = QJsonArray();
        clearHistoryLabels();
        updateTotalLabel();
    }
}

void AccountProcessPage::onExecuteClicked()
{
    if (m_rightAccountNumber.isEmpty()) {
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

    if (!isDeposit && m_rightAccountType == "savings") {
        showBox(this, QMessageBox::Warning,
                "출금 불가",
                "저축계좌(savings)는 출금할 수 없습니다.");
        m_depositRadio->setChecked(true);
        return;
    }

    QString reqType = isDeposit ? "deposit" : "withdraw";

    QJsonObject data;
    data["accountNumber"] = m_rightAccountNumber;
    data["accountPassword"] = m_accountPasswordEdit->text().trimmed();
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
        "font-size: 13px; color: #191F28;"
        "background-color: #F9FAFB; border: 1px solid #E5E8EB;"
        "border-radius: 8px; padding: 8px 12px;");

    m_historyLayout->insertWidget(m_historyLayout->count() - 1, historyLabel);
    m_historyCount++;
    updateTotalLabel();
}

void AccountProcessPage::updateTotalLabel()
{
    m_totalLabel->setText(QString("TOTAL : %1건").arg(m_historyCount));
}

void AccountProcessPage::renderHistoryLabels(const QJsonArray &transactions)
{
    clearHistoryLabels();

    for (const auto &v : transactions) {
        QJsonObject tx = v.toObject();
        QString typeText = tx["type"].toString();
        QString text = QString("%1원 / %2")
                           .arg(tx["amount"].toDouble(), 0, 'f', 0)
                           .arg(tx["description"].toString());
        addHistoryLabel(typeText, m_leftAccountNumber, text);
    }
}

void AccountProcessPage::applyHistoryFilter()
{
    renderHistoryLabels(m_currentTransactions);
}

void AccountProcessPage::fillSearchResults(const QString &keyword)
{
    m_searchResultList->clear();

    if (keyword.isEmpty()) {
        m_searchResultList->setVisible(false);
        return;
    }

    for (int i = 0; i < m_allAccounts.size(); ++i) {
        QJsonObject acc = m_allAccounts[i].toObject();
        QString accNum = acc["accountNumber"].toString();
        QString typeText = acc["type"].toString();

        if (accNum.contains(keyword, Qt::CaseInsensitive)) {
            QListWidgetItem *item =
                new QListWidgetItem(QString("%1 (%2)").arg(accNum, typeText), m_searchResultList);
            item->setData(Qt::UserRole, i);
        }
    }

    m_searchResultList->setVisible(m_searchResultList->count() > 0);
}

void AccountProcessPage::onPasswordReturnPressed()
{
    if (!m_rightAccountNumber.isEmpty()) {
        const QString pw = m_accountPasswordEdit->text().trimmed();
        if (!pw.isEmpty())
            loadAccountDetail(m_rightAccountNumber, pw);
    }
}

void AccountProcessPage::onSearchTextChanged(const QString &text)
{
    fillSearchResults(text.trimmed());
}

void AccountProcessPage::onSearchResultClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= m_allAccounts.size())
        return;

    QJsonObject acc = m_allAccounts[index].toObject();
    updateLeftSummary(acc);

    m_searchEdit->setText(acc["accountNumber"].toString());
    m_searchResultList->setVisible(false);

    tryLoadLeftAccountDetail();
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

        QString previousRight = m_rightAccountNumber;
        QString previousLeft = m_leftAccountNumber;

        m_allAccounts = response["data"].toObject()["accounts"].toArray();

        m_detailAccountCombo->blockSignals(true);
        m_detailAccountCombo->clear();

        int selectedRightIndex = -1;
        int selectedLeftIndex = -1;

        for (int i = 0; i < m_allAccounts.size(); ++i) {
            QJsonObject acc = m_allAccounts[i].toObject();
            QString accNum = acc["accountNumber"].toString();
            QString typeText = acc["type"].toString();

            m_detailAccountCombo->addItem(QString("%1 (%2)").arg(accNum, typeText), accNum);

            if (accNum == previousRight)
                selectedRightIndex = i;
            if (accNum == previousLeft)
                selectedLeftIndex = i;
        }

        m_detailAccountCombo->blockSignals(false);

        if (!m_allAccounts.isEmpty()) {
            if (selectedRightIndex < 0)
                selectedRightIndex = 0;
            if (selectedLeftIndex < 0)
                selectedLeftIndex = 0;

            m_detailAccountCombo->setCurrentIndex(selectedRightIndex);
            updateRightSummary(m_allAccounts[selectedRightIndex].toObject());
            updateLeftSummary(m_allAccounts[selectedLeftIndex].toObject());

            if (!m_leftAccountNumber.isEmpty()) {
                tryLoadLeftAccountDetail();
            }
        } else {
            m_leftAccountNumber.clear();
            m_leftBalance = 0;
            m_rightAccountNumber.clear();
            m_rightAccountType.clear();
            m_rightBalance = 0;
            m_currentTransactions = QJsonArray();

            m_accountNumberLabel->setText("계좌번호 : -");
            m_balanceLabel->setText("잔액 : 0원");
            m_selectedAccountLabel->setText("-");
            m_selectedTypeLabel->setText("-");
            m_selectedBalanceLabel->setText("0원");

            clearHistoryLabels();
            updateTotalLabel();
            m_searchResultList->clear();
            m_searchResultList->setVisible(false);
        }
    }
    else if (type == "get_account_detail_response") {
        if (status != "success") {
            showBox(this, QMessageBox::Warning, "오류", response["message"].toString());
            return;
        }

        QJsonObject account = response["data"].toObject()["account"].toObject();
        QString accountNumber = account["accountNumber"].toString();

        QString currentPw = m_accountPasswordEdit->text().trimmed();
        if (!currentPw.isEmpty() && accountNumber == m_rightAccountNumber) {
            m_accountPasswordCache[accountNumber] = currentPw;
        }

        if (accountNumber == m_rightAccountNumber) {
            updateRightSummary(account);
        }

        if (accountNumber == m_leftAccountNumber) {
            updateLeftSummary(account);
            m_currentTransactions = account["transactions"].toArray();
            applyHistoryFilter();
        }
    }
    else if (type == "deposit_response" || type == "withdraw_response") {
        if (status != "success") {
            showBox(this, QMessageBox::Warning, "오류", response["message"].toString());
            return;
        }

        m_rightBalance = static_cast<int>(response["data"].toObject()["balance"].toDouble());
        m_selectedBalanceLabel->setText(QString("%1원").arg(m_rightBalance));

        if (!m_rightAccountNumber.isEmpty()) {
            QString pw = m_accountPasswordEdit->text().trimmed();
            if (!pw.isEmpty()) {
                m_accountPasswordCache[m_rightAccountNumber] = pw;
            }
        }

        showBox(this, QMessageBox::Information, "완료", response["message"].toString());

        m_amountSpin->setValue(0);
        m_descriptionEdit->clear();

        loadAccounts();

        if (!m_rightAccountNumber.isEmpty()) {
            QString pw = m_accountPasswordCache.value(m_rightAccountNumber);
            if (!pw.isEmpty()) {
                loadAccountDetail(m_rightAccountNumber, pw);
            }
        }

        if (!m_leftAccountNumber.isEmpty() && m_leftAccountNumber != m_rightAccountNumber) {
            tryLoadLeftAccountDetail();
        }
    }
}