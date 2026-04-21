#include "homepage.h"
#include "networkclient.h"
#include "sessioncontext.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QShowEvent>
#include <QJsonObject>
#include <QJsonArray>

HomePage::HomePage(const QString &userName, QWidget *parent)
    : QWidget(parent), m_userName(userName)
{
    setStyleSheet(
        "background-color: #F5F7FB;"
        "QLabel { color: #111827; background: transparent; border: none; }"
        );

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(40, 40, 40, 40);
    layout->setSpacing(20);

    QFrame *card = new QFrame(this);
    card->setStyleSheet(
        "QFrame {"
        " background-color: white;"
        " border-radius: 20px;"
        " border: 1px solid #E5E7EB;"
        "}"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(30, 30, 30, 30);
    cardLayout->setSpacing(16);

    m_labelWelcome = new QLabel(QString("%1님 환영합니다").arg(userName), card);
    m_labelWelcome->setStyleSheet(
        "font-size: 28px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    m_labelMoney = new QLabel("보유 금액 : 불러오는 중...", card);
    m_labelMoney->setStyleSheet(
        "font-size: 20px;"
        "font-weight: 600;"
        "color: #2563EB;"
        );

    QLabel *subText = new QLabel("간편 계좌 관리 시스템에 오신 것을 환영합니다.", card);
    subText->setStyleSheet(
        "font-size: 14px;"
        "color: #6B7280;"
        );

    cardLayout->addWidget(m_labelWelcome);
    cardLayout->addWidget(m_labelMoney);
    cardLayout->addWidget(subText);

    layout->addWidget(card);
    layout->addStretch();

    connect(NetworkClient::instance(), &NetworkClient::responseReceived,
            this, &HomePage::onResponseReceived);
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

    m_labelMoney->setText(QString("보유 금액 : %1원").arg(static_cast<int>(total)));
}
