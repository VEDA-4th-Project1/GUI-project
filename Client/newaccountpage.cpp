#include "newaccountpage.h"
#include "networkclient.h"
#include "sessioncontext.h"
#include "appstyle.h"

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

/** setupUI를 호출하고 NetworkClient 시그널을 연결한다. */
NewAccountPage::NewAccountPage(QWidget* parent) : QWidget(parent) {
    setupUI();
    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onNetworkResponse(QJsonObject)));
}

/**
 * 페이지 레이아웃을 구성한다.
 * - 상단: 페이지 제목·부제목
 * - 카드: 계좌 종류 / 비밀번호 / 초기 입금액 / 이자율(또는 마이너스 한도) / 개설 버튼
 *
 * addRow 람다로 (라벨, 입력 위젯) 쌍을 수평으로 배치한다.
 * 이자율(m_rateWidget)과 마이너스 한도(m_limitWidget)는 계좌 종류에 따라
 * 한 쪽만 표시된다.
 */
void NewAccountPage::setupUI() {
    setStyleSheet("background-color: #F2F4F6;");

    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(48, 40, 48, 40);
    root->setSpacing(0);

    QLabel* pageTitle = new QLabel("신규 계좌 개설", this);
    pageTitle->setStyleSheet(
        "font-size: 24px; font-weight: 800; color: #191F28; border: none;");
    root->addWidget(pageTitle);
    root->addSpacing(6);

    QLabel* pageSub = new QLabel("새 계좌를 개설하여 자산을 관리하세요.", this);
    pageSub->setStyleSheet("font-size: 14px; color: #8B95A1; border: none;");
    root->addWidget(pageSub);
    root->addSpacing(24);

    QFrame* card = new QFrame(this);
    card->setStyleSheet(AppStyle::CARD);
    AppStyle::applyCardShadow(card);

    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(40, 36, 40, 36);
    cl->setSpacing(20);

    const QString &inputStyle    = AppStyle::INPUT_ALL;
    const QString &fixedValStyle = AppStyle::FIXED_VAL;

    // (라벨 텍스트, 입력 위젯)을 수평으로 묶는 헬퍼 람다
    auto addRow = [&](const QString& labelText, QWidget* field) {
        QHBoxLayout* row = new QHBoxLayout();
        row->setSpacing(0);
        QLabel* lbl = new QLabel(labelText, card);
        lbl->setStyleSheet(
            "color: #4E5968; font-size: 14px; font-weight: 600; border: none;");
        lbl->setFixedWidth(140);
        row->addWidget(lbl);
        row->addWidget(field, 1);
        cl->addLayout(row);
    };

    // 계좌 종류 선택 콤보박스
    m_typeCombo = new QComboBox(card);
    m_typeCombo->addItem("저축 계좌 (savings)",    "savings");
    m_typeCombo->addItem("입출금 계좌 (checking)", "checking");
    m_typeCombo->setMinimumHeight(48);
    m_typeCombo->setStyleSheet(inputStyle +
        "QComboBox::drop-down { border: none; width: 32px; }");
    addRow("계좌 종류", m_typeCombo);

    // 계좌 비밀번호 (4자 이상 필수)
    m_accPasswordEdit = new QLineEdit(card);
    m_accPasswordEdit->setPlaceholderText("4자 이상");
    m_accPasswordEdit->setEchoMode(QLineEdit::Password);
    m_accPasswordEdit->setMinimumHeight(48);
    m_accPasswordEdit->setStyleSheet(inputStyle);
    addRow("계좌 비밀번호", m_accPasswordEdit);

    // 초기 입금액 (0원 가능)
    m_initBalanceSpin = new QDoubleSpinBox(card);
    m_initBalanceSpin->setRange(0, 999999999);
    m_initBalanceSpin->setSingleStep(10000);
    m_initBalanceSpin->setDecimals(0);
    m_initBalanceSpin->setSuffix(" 원");
    m_initBalanceSpin->setMinimumHeight(48);
    m_initBalanceSpin->setStyleSheet(inputStyle);
    addRow("초기 입금액", m_initBalanceSpin);

    // 이자율 행 — 저축계좌 선택 시 표시 (고정값, 편집 불가)
    m_rateWidget = new QWidget(card);
    m_rateWidget->setStyleSheet("QWidget { border: none; background: transparent; }");
    QHBoxLayout* rateRow = new QHBoxLayout(m_rateWidget);
    rateRow->setContentsMargins(0, 0, 0, 0);
    rateRow->setSpacing(0);
    QLabel* rateLabel = new QLabel("이자율", m_rateWidget);
    rateLabel->setStyleSheet(
        "color: #4E5968; font-size: 14px; font-weight: 600; border: none;");
    rateLabel->setFixedWidth(140);
    QLabel* rateVal = new QLabel("연 3.0% (고정)", m_rateWidget);
    rateVal->setMinimumHeight(48);
    rateVal->setStyleSheet(fixedValStyle);
    rateRow->addWidget(rateLabel);
    rateRow->addWidget(rateVal, 1);
    cl->addWidget(m_rateWidget);

    // 마이너스 한도 행 — 입출금계좌 선택 시 표시 (고정 0원, 편집 불가)
    m_limitWidget = new QWidget(card);
    m_limitWidget->setStyleSheet("QWidget { border: none; background: transparent; }");
    QHBoxLayout* limitRow = new QHBoxLayout(m_limitWidget);
    limitRow->setContentsMargins(0, 0, 0, 0);
    limitRow->setSpacing(0);
    QLabel* limitLabel = new QLabel("마이너스 한도", m_limitWidget);
    limitLabel->setStyleSheet(
        "color: #4E5968; font-size: 14px; font-weight: 600; border: none;");
    limitLabel->setFixedWidth(140);
    QLabel* limitVal = new QLabel("0 원 (고정)", m_limitWidget);
    limitVal->setMinimumHeight(48);
    limitVal->setStyleSheet(fixedValStyle);
    limitRow->addWidget(limitLabel);
    limitRow->addWidget(limitVal, 1);
    m_limitWidget->setVisible(false);  // 기본 선택이 savings이므로 숨김
    cl->addWidget(m_limitWidget);

    // 개설 버튼
    m_createBtn = new QPushButton("계좌 개설", card);
    m_createBtn->setMinimumHeight(52);
    m_createBtn->setCursor(Qt::PointingHandCursor);
    m_createBtn->setStyleSheet(AppStyle::BTN_BLUE);
    cl->addSpacing(4);
    cl->addWidget(m_createBtn);

    root->addWidget(card);
    root->addStretch();

    connect(m_createBtn, SIGNAL(clicked()),                this, SLOT(onCreateClicked()));
    connect(m_typeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onTypeChanged(int)));
}

/**
 * savings 선택 시 m_rateWidget 표시·m_limitWidget 숨김.
 * checking 선택 시 반대로 전환한다.
 */
void NewAccountPage::onTypeChanged(int index) {
    bool isSavings = (m_typeCombo->itemData(index).toString() == "savings");
    m_rateWidget->setVisible(isSavings);
    m_limitWidget->setVisible(!isSavings);
}

/**
 * 비밀번호 4자 미만이면 경고하고 중단한다.
 * 유효 시 계좌 종류·비밀번호·초기 입금액과
 * 계좌 종류별 추가 필드(interestRate 또는 overdraftLimit)를 담아 요청을 전송한다.
 */
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
        data["interestRate"]   = 0.03;   // 저축계좌 고정 이율 3%
    else
        data["overdraftLimit"] = 0.0;    // 입출금계좌 마이너스 한도 0

    QJsonObject req;
    req["type"]  = "create_account";
    req["token"] = SessionContext::instance().token();
    req["data"]  = data;
    NetworkClient::instance()->sendRequest(req);
}

/** 비모달 QMessageBox를 표시하는 파일-로컬 헬퍼 함수. */
static void showBox(QWidget *parent, QMessageBox::Icon icon,
                    const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(icon, title, text, QMessageBox::Ok, parent);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->open();
}

/**
 * "create_account_response" 타입만 처리한다.
 * 성공: 발급된 계좌번호를 안내하고 비밀번호·초기 입금액 필드를 초기화한다.
 * 실패: 서버 오류 메시지를 경고로 표시한다.
 */
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
