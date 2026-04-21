#include "homepage.h"
#include "networkclient.h"
#include "sessioncontext.h"
#include "appstyle.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QShowEvent>
#include <QJsonObject>
#include <QJsonArray>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #F2F4F6;");

    QVBoxLayout *root = new QVBoxLayout(this);
    root->setContentsMargins(40, 40, 40, 40);
    root->setSpacing(24);

    // ── 히어로 잔액 카드 ──────────────────────────────────────────────────────
    m_heroCard = new QFrame(this);
    m_heroCard->setStyleSheet(AppStyle::HERO_CARD);
    m_heroCard->setMinimumHeight(160);
    AppStyle::applyHeroShadow(m_heroCard);

    QVBoxLayout *heroLayout = new QVBoxLayout(m_heroCard);
    heroLayout->setContentsMargins(32, 28, 32, 28);
    heroLayout->setSpacing(8);

    const QString userName = SessionContext::instance().userName();

    QLabel *greetLabel = new QLabel(QString("%1님, 안녕하세요").arg(userName));
    greetLabel->setStyleSheet(
        "color: rgba(255,255,255,0.85); font-size: 14px; font-weight: 600;"
        "background: transparent; border: none;");

    QLabel *balanceTitleLabel = new QLabel("총 보유 자산");
    balanceTitleLabel->setStyleSheet(
        "color: rgba(255,255,255,0.65); font-size: 12px; font-weight: 600;"
        "letter-spacing: 0.5px; background: transparent; border: none;");

    m_labelMoney = new QLabel("불러오는 중...");
    m_labelMoney->setStyleSheet(
        "color: white; font-size: 32px; font-weight: 800;"
        "background: transparent; border: none;");

    heroLayout->addWidget(greetLabel);
    heroLayout->addSpacing(4);
    heroLayout->addWidget(balanceTitleLabel);
    heroLayout->addWidget(m_labelMoney);

    // ── 정보 카드 ─────────────────────────────────────────────────────────────
    QFrame *infoCard = new QFrame(this);
    infoCard->setStyleSheet(AppStyle::CARD);
    AppStyle::applyCardShadow(infoCard);

    QVBoxLayout *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(28, 24, 28, 24);
    infoLayout->setSpacing(8);

    QLabel *infoTitle = new QLabel("Easy Bank에 오신 것을 환영합니다");
    infoTitle->setStyleSheet(AppStyle::LABEL_SUBTITLE);

    QLabel *infoDesc = new QLabel(
        "계좌 개설, 입출금, 잔액 조회 등 다양한 금융 서비스를\n"
        "왼쪽 메뉴에서 이용하실 수 있습니다.");
    infoDesc->setStyleSheet(AppStyle::LABEL_MUTED);
    infoDesc->setWordWrap(true);

    infoLayout->addWidget(infoTitle);
    infoLayout->addWidget(infoDesc);

    root->addWidget(m_heroCard);
    root->addWidget(infoCard);
    root->addStretch();

    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onResponseReceived(QJsonObject)));
}

void HomePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadTotalBalance();
}

void HomePage::loadTotalBalance()
{
    QJsonObject req;
    req["type"]  = "list_accounts";
    req["token"] = SessionContext::instance().token();
    req["data"]  = QJsonObject{};
    NetworkClient::instance()->sendRequest(req);
}

void HomePage::onResponseReceived(const QJsonObject &response)
{
    if (response["type"].toString() != "list_accounts_response") return;
    if (response["status"].toString() != "success") return;

    QJsonArray accounts = response["data"].toObject()["accounts"].toArray();

    double total = 0;
    for (const auto &v : accounts)
        total += v.toObject()["balance"].toDouble();

    // 천 단위 콤마 포맷
    m_labelMoney->setText(
        QString("%L1원").arg(static_cast<long long>(total)));
}
