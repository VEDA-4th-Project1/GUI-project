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

/** 비모달 QMessageBox를 표시하는 파일-로컬 헬퍼 함수. */
static void showBox(QWidget *parent, QMessageBox::Icon icon,
                    const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->open();
}

/**
 * 모든 멤버 포인터를 nullptr로 초기화하고 setupUI()를 호출한다.
 * NetworkClient 시그널 연결 후 초기 계좌 목록을 로드한다.
 */
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

/** 수평 레이아웃에 좌(11)·우(9) 비율로 두 카드 프레임을 배치하고 각 패널을 구성한다. */
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

/**
 * 좌측 패널을 구성한다.
 * - 검색 입력 + 드롭다운 목록
 * - 선택 계좌번호·잔액 라벨
 * - 거래 내역 헤더 + 스크롤 영역
 * - 총 건수 라벨
 */
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

    // 검색 결과 드롭다운 — 평소 숨김, 결과 있을 때만 표시
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

    // 거래 내역 스크롤 영역
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
    m_historyLayout->addStretch();  // 항목이 없을 때 위쪽으로 밀리지 않도록 stretch

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

/**
 * 우측 패널을 구성한다.
 * - 계좌 선택 콤보박스
 * - QGridLayout으로 계좌번호·타입·잔액·비밀번호·거래종류·금액·설명·실행 버튼 배치
 */
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

    QLabel *accTitle     = new QLabel("계좌번호",    detailBox);
    QLabel *typeTitle    = new QLabel("타입",        detailBox);
    QLabel *balanceTitle = new QLabel("잔액",        detailBox);
    QLabel *pwTitle      = new QLabel("계좌 비밀번호", detailBox);
    QLabel *amountTitle  = new QLabel("금액",        detailBox);
    QLabel *descTitle    = new QLabel("설명",        detailBox);

    m_selectedAccountLabel = new QLabel("-", detailBox);
    m_selectedTypeLabel    = new QLabel("-", detailBox);
    m_selectedBalanceLabel = new QLabel("0원", detailBox);

    const QString &inputStyle = AppStyle::INPUT;
    const QString &radioStyle = AppStyle::RADIO;

    m_accountPasswordEdit = new QLineEdit(detailBox);
    m_accountPasswordEdit->setEchoMode(QLineEdit::Password);
    m_accountPasswordEdit->setPlaceholderText("계좌 비밀번호 입력");
    m_accountPasswordEdit->setFixedHeight(38);
    m_accountPasswordEdit->setStyleSheet(inputStyle);

    m_depositRadio  = new QRadioButton("입금", detailBox);
    m_withdrawRadio = new QRadioButton("출금", detailBox);
    m_depositRadio->setChecked(true);  // 기본값: 입금
    m_depositRadio->setStyleSheet(radioStyle);
    m_withdrawRadio->setStyleSheet(radioStyle);

    // 입금·출금을 하나의 그룹으로 묶어 상호 배타적으로 만든다
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

    grid->addWidget(accTitle,              0, 0);  grid->addWidget(m_selectedAccountLabel, 0, 1);
    grid->addWidget(typeTitle,             1, 0);  grid->addWidget(m_selectedTypeLabel,    1, 1);
    grid->addWidget(balanceTitle,          2, 0);  grid->addWidget(m_selectedBalanceLabel, 2, 1);
    grid->addWidget(pwTitle,               3, 0);  grid->addWidget(m_accountPasswordEdit,  3, 1);
    grid->addWidget(new QLabel("거래 종류", detailBox), 4, 0);
    grid->addWidget(radioBox,              4, 1);
    grid->addWidget(amountTitle,           5, 0);  grid->addWidget(m_amountSpin,           5, 1);
    grid->addWidget(descTitle,             6, 0);  grid->addWidget(m_descriptionEdit,      6, 1);
    grid->addWidget(m_executeBtn,          7, 0, 1, 2);  // 두 열 병합

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
}

/** 페이지가 표시될 때마다 최신 계좌 목록을 서버에서 로드한다. */
void AccountProcessPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadAccounts();
}

/** 서버에 "list_accounts" 요청을 전송한다. */
void AccountProcessPage::loadAccounts()
{
    QJsonObject request;
    request["type"]  = "list_accounts";
    request["token"] = SessionContext::instance().token();
    request["data"]  = QJsonObject{};
    NetworkClient::instance()->sendRequest(request);
}

/** m_leftAccountNumber, m_leftBalance를 갱신하고 좌측 라벨을 업데이트한다. */
void AccountProcessPage::updateLeftSummary(const QJsonObject &acc)
{
    m_leftAccountNumber = acc["accountNumber"].toString();
    m_leftBalance = static_cast<int>(acc["balance"].toDouble());

    m_accountNumberLabel->setText(QString("계좌번호 : %1").arg(m_leftAccountNumber));
    m_balanceLabel->setText(QString("잔액 : %1원").arg(m_leftBalance));
}

/**
 * m_rightAccountNumber, m_rightAccountType, m_rightBalance를 갱신하고
 * 우측 라벨을 업데이트한다.
 * 저축계좌는 출금 라디오를 비활성화한다.
 */
void AccountProcessPage::updateRightSummary(const QJsonObject &acc)
{
    m_rightAccountNumber = acc["accountNumber"].toString();
    m_rightAccountType   = acc["type"].toString();
    m_rightBalance       = static_cast<int>(acc["balance"].toDouble());

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

/**
 * 콤보박스 인덱스에 해당하는 계좌 정보를 우측에 갱신한다.
 * [변경] 이전에는 비밀번호 입력 여부에 따라 조건부로 상세 조회를 트리거했으나,
 *        상세 조회가 비밀번호 불필요로 바뀌었으므로 여기서는 우측 요약만 갱신한다.
 *        (거래 내역은 좌측 패널 기준으로 loadAccountDetail 호출)
 */
void AccountProcessPage::onDetailAccountChanged(int index)
{
    if (index < 0 || index >= m_allAccounts.size())
        return;

    QJsonObject acc = m_allAccounts[index].toObject();
    updateRightSummary(acc);
}

/** 저축계좌에서 출금을 시도하면 경고를 표시하고 입금 라디오로 되돌린다. */
void AccountProcessPage::onWithdrawRadioClicked(bool checked)
{
    if (!checked) return;

    if (m_rightAccountType == "savings") {
        showBox(this, QMessageBox::Warning,
                "출금 불가",
                "저축계좌(savings)는 출금할 수 없습니다.");
        m_depositRadio->setChecked(true);
    }
}

/**
 * 계좌 상세를 서버에 요청한다.
 * [변경] 서버 handleGetAccountDetail의 비밀번호 검증이 제거되어,
 *        data에 accountPassword를 넣지 않는다. token만으로 조회 가능.
 */
void AccountProcessPage::loadAccountDetail(const QString &accountNumber)
{
    if (accountNumber.isEmpty()) return;

    QJsonObject data;
    data["accountNumber"] = accountNumber;  // accountPassword 필드 삭제됨

    QJsonObject request;
    request["type"]  = "get_account_detail";
    request["token"] = SessionContext::instance().token();
    request["data"]  = data;

    NetworkClient::instance()->sendRequest(request);
}

/**
 * 유효성 검사 후 서버에 입금 또는 출금 요청을 전송한다.
 * 설명이 비어 있으면 "입금" 또는 "출금"이 기본값으로 사용된다.
 */
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
    data["accountNumber"]   = m_rightAccountNumber;
    data["accountPassword"] = m_accountPasswordEdit->text().trimmed();
    data["amount"]          = m_amountSpin->value();
    data["description"]     = m_descriptionEdit->text().trimmed().isEmpty()
                              ? (isDeposit ? "입금" : "출금")
                              : m_descriptionEdit->text().trimmed();

    QJsonObject request;
    request["type"]  = reqType;
    request["token"] = SessionContext::instance().token();
    request["data"]  = data;

    NetworkClient::instance()->sendRequest(request);
}

/** m_historyLayout에서 stretch를 제외한 모든 위젯을 제거하고 카운터를 0으로 초기화한다. */
void AccountProcessPage::clearHistoryLabels()
{
    while (m_historyLayout->count() > 1) {
        QLayoutItem *item = m_historyLayout->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_historyCount = 0;
}

/**
 * 거래 내역 한 건을 "타입 | 계좌번호 | 금액/설명" 형식의 라벨로 추가하고
 * m_historyCount를 증가시킨다.
 */
void AccountProcessPage::addHistoryLabel(const QString &type, const QString &accountNumber, const QString &text)
{
    QLabel *historyLabel = new QLabel(
        QString("%1   |   %2   |   %3").arg(type, accountNumber, text),
        m_historyContainer);

    historyLabel->setStyleSheet(
        "font-size: 13px; color: #191F28;"
        "background-color: #F9FAFB; border: 1px solid #E5E8EB;"
        "border-radius: 8px; padding: 8px 12px;");

    // stretch 바로 앞에 삽입해 항목이 위에서 아래로 쌓이도록 한다
    m_historyLayout->insertWidget(m_historyLayout->count() - 1, historyLabel);
    m_historyCount++;
    updateTotalLabel();
}

/** "TOTAL : N건" 형식으로 m_totalLabel 텍스트를 갱신한다. */
void AccountProcessPage::updateTotalLabel()
{
    m_totalLabel->setText(QString("TOTAL : %1건").arg(m_historyCount));
}

/** 기존 거래 내역을 모두 제거하고 transactions 배열로 새로 구성한다. */
void AccountProcessPage::renderHistoryLabels(const QJsonArray &transactions)
{
    clearHistoryLabels();

    for (const auto &v : transactions) {
        QJsonObject tx  = v.toObject();
        QString typeText = tx["type"].toString();
        QString text = QString("%1원 / %2")
                           .arg(tx["amount"].toDouble(), 0, 'f', 0)
                           .arg(tx["description"].toString());
        addHistoryLabel(typeText, m_leftAccountNumber, text);
    }
}

/** m_currentTransactions를 그대로 렌더링해 필터를 재적용한다. */
void AccountProcessPage::applyHistoryFilter()
{
    renderHistoryLabels(m_currentTransactions);
}

/**
 * keyword를 포함하는 계좌번호를 m_allAccounts에서 검색한다.
 * 결과가 있으면 m_searchResultList를 표시하고, 없거나 키워드가 비면 숨긴다.
 */
void AccountProcessPage::fillSearchResults(const QString &keyword)
{
    m_searchResultList->clear();

    if (keyword.isEmpty()) {
        m_searchResultList->setVisible(false);
        return;
    }

    for (int i = 0; i < m_allAccounts.size(); ++i) {
        QJsonObject acc = m_allAccounts[i].toObject();
        QString accNum  = acc["accountNumber"].toString();
        QString typeText = acc["type"].toString();

        if (accNum.contains(keyword, Qt::CaseInsensitive)) {
            QListWidgetItem *item =
                new QListWidgetItem(QString("%1 (%2)").arg(accNum, typeText), m_searchResultList);
            item->setData(Qt::UserRole, i);  // 원래 인덱스를 UserRole에 저장
        }
    }

    m_searchResultList->setVisible(m_searchResultList->count() > 0);
}

/** 검색 텍스트 변경 시 fillSearchResults()를 공백 제거 후 호출한다. */
void AccountProcessPage::onSearchTextChanged(const QString &text)
{
    fillSearchResults(text.trimmed());
}

/**
 * 검색 결과 항목 클릭 시 해당 계좌를 좌측 패널에 선택하고 거래 내역을 조회한다.
 * [변경] 비밀번호 캐시 확인 단계(tryLoadLeftAccountDetail) 제거 —
 *        비밀번호 없이 바로 loadAccountDetail로 거래 내역을 불러온다.
 */
void AccountProcessPage::onSearchResultClicked(QListWidgetItem *item)
{
    if (!item) return;

    int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= m_allAccounts.size()) return;

    QJsonObject acc = m_allAccounts[index].toObject();
    updateLeftSummary(acc);

    m_searchEdit->setText(acc["accountNumber"].toString());
    m_searchResultList->setVisible(false);

    loadAccountDetail(m_leftAccountNumber);
}

/**
 * 서버 응답을 type별로 라우팅한다.
 *
 * list_accounts_response:
 *   - 이전 선택(좌·우)을 기억하고 콤보박스를 재구성한다.
 *   - 계좌가 없으면 모든 상태 변수와 UI를 초기화한다.
 *
 * get_account_detail_response:
 *   - 응답 계좌가 우측이면 updateRightSummary()로 잔액 갱신.
 *   - 응답 계좌가 좌측이면 updateLeftSummary()와 거래 내역 렌더링.
 *   - 비밀번호를 캐시에 저장한다.
 *
 * deposit_response / withdraw_response:
 *   - 우측 잔액 라벨 갱신.
 *   - 성공 메시지 표시 후 입력 필드 초기화.
 *   - 계좌 목록과 거래 내역을 재조회한다.
 */
void AccountProcessPage::onResponseReceived(const QJsonObject &response)
{
    QString type   = response["type"].toString();
    QString status = response["status"].toString();

    if (type == "list_accounts_response") {
        if (status != "success") {
            showBox(this, QMessageBox::Warning, "오류", response["message"].toString());
            return;
        }

        QString previousRight = m_rightAccountNumber;
        QString previousLeft  = m_leftAccountNumber;

        m_allAccounts = response["data"].toObject()["accounts"].toArray();

        // 신호 차단 중 콤보박스를 재구성해 onDetailAccountChanged 중복 호출 방지
        m_detailAccountCombo->blockSignals(true);
        m_detailAccountCombo->clear();

        int selectedRightIndex = -1;
        int selectedLeftIndex  = -1;

        for (int i = 0; i < m_allAccounts.size(); ++i) {
            QJsonObject acc  = m_allAccounts[i].toObject();
            QString accNum   = acc["accountNumber"].toString();
            QString typeText = acc["type"].toString();

            m_detailAccountCombo->addItem(QString("%1 (%2)").arg(accNum, typeText), accNum);

            if (accNum == previousRight) selectedRightIndex = i;
            if (accNum == previousLeft)  selectedLeftIndex  = i;
        }

        m_detailAccountCombo->blockSignals(false);

        if (!m_allAccounts.isEmpty()) {
            if (selectedRightIndex < 0) selectedRightIndex = 0;
            if (selectedLeftIndex  < 0) selectedLeftIndex  = 0;

            m_detailAccountCombo->setCurrentIndex(selectedRightIndex);
            updateRightSummary(m_allAccounts[selectedRightIndex].toObject());
            updateLeftSummary(m_allAccounts[selectedLeftIndex].toObject());

            // [변경] 비밀번호 캐시 조회 없이 바로 상세 요청 — 서버가 토큰만 검증
            if (!m_leftAccountNumber.isEmpty())
                loadAccountDetail(m_leftAccountNumber);
        } else {
            // 보유 계좌가 없는 경우 모든 상태와 UI를 초기화한다
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

        QJsonObject account     = response["data"].toObject()["account"].toObject();
        QString     accountNumber = account["accountNumber"].toString();

        // [변경] 비밀번호 캐시(m_accountPasswordCache) 저장 로직 삭제 —
        //        상세 조회에 비밀번호가 필요 없어 캐시 자체가 불필요해짐

        // 우측 계좌에 대한 응답이면 잔액만 갱신
        if (accountNumber == m_rightAccountNumber)
            updateRightSummary(account);

        // 좌측 계좌에 대한 응답이면 잔액 + 거래 내역 갱신
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

        showBox(this, QMessageBox::Information, "완료", response["message"].toString());

        // 입력 필드 초기화
        m_amountSpin->setValue(0);
        m_descriptionEdit->clear();

        // 계좌 목록 갱신 및 거래 내역 재조회
        loadAccounts();

        // [변경] 비밀번호 캐시 없이 바로 상세 재조회 (조회에는 비밀번호 불필요)
        if (!m_rightAccountNumber.isEmpty())
            loadAccountDetail(m_rightAccountNumber);

        // 좌측과 우측이 다른 계좌라면 좌측도 갱신
        if (!m_leftAccountNumber.isEmpty() && m_leftAccountNumber != m_rightAccountNumber)
            loadAccountDetail(m_leftAccountNumber);
    }
}
