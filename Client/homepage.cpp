#include "homepage.h"
#include "networkclient.h"
#include "sessioncontext.h"
#include "appstyle.h"
#include "balancepiechart.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QShowEvent>
#include <QJsonObject>
#include <QJsonArray>
#include <QLocale>
#include <QScrollArea>

/**
 * 스크롤 영역 안에 히어로 카드·안내 카드·파이차트 카드를 배치하고
 * NetworkClient 시그널을 onResponseReceived 슬롯에 연결한다.
 */
HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet("background-color: #F2F4F6;");

    QVBoxLayout *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    // 페이지 전체를 감싸는 스크롤 영역 (콘텐츠가 길어질 때 수직 스크롤 제공)
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollBar:vertical { background: transparent; width: 8px; margin: 4px; }"
        "QScrollBar::handle:vertical { background: #CBD5E1; border-radius: 4px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background: #94A3B8; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }");

    QWidget *content = new QWidget(scroll);
    content->setStyleSheet("background-color: #F2F4F6;");
    scroll->setWidget(content);
    outer->addWidget(scroll);

    QVBoxLayout *root = new QVBoxLayout(content);
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

    // 초기값: 서버 응답 수신 전 임시 표시
    m_labelMoney = new QLabel("불러오는 중...");
    m_labelMoney->setStyleSheet(
        "color: white; font-size: 32px; font-weight: 800;"
        "background: transparent; border: none;");

    heroLayout->addWidget(greetLabel);
    heroLayout->addSpacing(4);
    heroLayout->addWidget(balanceTitleLabel);
    heroLayout->addWidget(m_labelMoney);

    // ── 서비스 안내 카드 ──────────────────────────────────────────────────────
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

    // ── 자산 분포 파이 차트 카드 ──────────────────────────────────────────────
    QFrame *pieCard = new QFrame(this);
    pieCard->setStyleSheet(AppStyle::CARD);
    AppStyle::applyCardShadow(pieCard);

    QVBoxLayout *pieOuter = new QVBoxLayout(pieCard);
    pieOuter->setContentsMargins(28, 24, 28, 24);
    pieOuter->setSpacing(12);

    QLabel *pieTitle = new QLabel("계좌별 자산 분포");
    pieTitle->setStyleSheet(AppStyle::LABEL_SUBTITLE);

    QLabel *pieSub = new QLabel("각 조각에 마우스를 올리면 계좌 잔액이 표시됩니다.");
    pieSub->setStyleSheet(AppStyle::LABEL_MUTED);

    QHBoxLayout *pieRow = new QHBoxLayout();
    pieRow->setContentsMargins(0, 0, 0, 0);
    pieRow->setSpacing(24);

    m_pieChart = new BalancePieChart(pieCard);
    m_pieChart->setMinimumSize(200, 200);

    // 파이차트 오른쪽 범례 컨테이너 (onResponseReceived에서 동적으로 내용 구성)
    m_legendCard = new QFrame(pieCard);
    m_legendCard->setStyleSheet("QFrame { background: transparent; border: none; }");
    QVBoxLayout *legendLayout = new QVBoxLayout(m_legendCard);
    legendLayout->setContentsMargins(0, 4, 0, 4);
    legendLayout->setSpacing(8);
    legendLayout->addStretch();

    pieRow->addWidget(m_pieChart, 1);
    pieRow->addWidget(m_legendCard, 1);

    pieOuter->addWidget(pieTitle);
    pieOuter->addWidget(pieSub);
    pieOuter->addSpacing(4);
    pieOuter->addLayout(pieRow);

    root->addWidget(m_heroCard);
    root->addWidget(infoCard);
    root->addWidget(pieCard, 1);
    root->addStretch();

    connect(NetworkClient::instance(), SIGNAL(responseReceived(QJsonObject)),
            this, SLOT(onResponseReceived(QJsonObject)));
}

/** QWidget::showEvent를 호출한 뒤 loadTotalBalance()로 서버 요청을 보낸다. */
void HomePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    loadTotalBalance();
}

/** 서버에 "list_accounts" 요청을 전송한다. 세션 토큰을 함께 전달한다. */
void HomePage::loadTotalBalance()
{
    QJsonObject req;
    req["type"]  = "list_accounts";
    req["token"] = SessionContext::instance().token();
    req["data"]  = QJsonObject{};
    NetworkClient::instance()->sendRequest(req);
}

/**
 * "list_accounts_response" 타입·성공 응답만 처리한다.
 *
 * 처리 내용:
 * 1. 모든 계좌 잔액을 합산해 m_labelMoney에 표시한다.
 * 2. 잔액이 0을 초과하는 계좌만 BalancePieChart::Slice로 변환해 파이차트에 전달한다.
 * 3. m_legendCard의 기존 위젯을 제거하고 계좌별 색상 도트·이름·잔액·비중을 새로 추가한다.
 */
void HomePage::onResponseReceived(const QJsonObject &response)
{
    if (response["type"].toString() != "list_accounts_response") return;
    if (response["status"].toString() != "success") return;

    QJsonArray accounts = response["data"].toObject()["accounts"].toArray();

    double total = 0;
    for (const auto &v : accounts)
        total += v.toObject()["balance"].toDouble();

    m_labelMoney->setText(
        QString("%L1원").arg(static_cast<long long>(total)));

    // 계좌별 색상 팔레트 (최대 10개, 이후 순환)
    static const QVector<QColor> palette = {
        QColor("#3182F6"), QColor("#00C896"), QColor("#F59E0B"),
        QColor("#EF4444"), QColor("#8B5CF6"), QColor("#06B6D4"),
        QColor("#EC4899"), QColor("#10B981"), QColor("#F97316"),
        QColor("#6366F1")
    };

    QVector<BalancePieChart::Slice> slices;
    for (int i = 0; i < accounts.size(); ++i) {
        QJsonObject acc = accounts[i].toObject();
        double balance = acc["balance"].toDouble();
        if (balance <= 0) continue;  // 잔액 0 이하 계좌는 차트에서 제외
        BalancePieChart::Slice s;
        s.label    = acc["accountNumber"].toString();
        s.subLabel = acc["type"].toString();
        s.value    = balance;
        s.color    = palette[i % palette.size()];
        slices.append(s);
    }
    m_pieChart->setAccounts(slices);

    // 범례 재구성: 기존 위젯 삭제 후 새로 추가
    QLayout *oldLayout = m_legendCard->layout();
    QLayoutItem *item;
    while (oldLayout && (item = oldLayout->takeAt(0)) != nullptr) {
        if (QWidget *w = item->widget()) w->deleteLater();
        delete item;
    }

    QVBoxLayout *legendLayout = static_cast<QVBoxLayout*>(oldLayout);
    for (const auto &s : slices) {
        QWidget *row = new QWidget(m_legendCard);
        row->setStyleSheet("background: transparent;");
        QHBoxLayout *rowL = new QHBoxLayout(row);
        rowL->setContentsMargins(0, 0, 0, 0);
        rowL->setSpacing(10);

        // 색상 원형 도트
        QLabel *dot = new QLabel(row);
        dot->setFixedSize(12, 12);
        dot->setStyleSheet(QString(
            "background-color: %1; border-radius: 6px; border: none;")
            .arg(s.color.name()));

        // 계좌번호 + 계좌 종류 (부연 텍스트)
        QLabel *name = new QLabel(
            QString("%1  <span style='color:#8B95A1;'>(%2)</span>")
                .arg(s.label, s.subLabel), row);
        name->setTextFormat(Qt::RichText);
        name->setStyleSheet(
            "color: #4E5968; font-size: 13px; font-weight: 600;"
            "background: transparent; border: none;");

        // 잔액 + 비중(%)
        const double pct = total > 0 ? (s.value / total) * 100.0 : 0.0;
        QLabel *val = new QLabel(
            QString("%1원  <span style='color:#8B95A1;'>%2%</span>")
                .arg(QLocale(QLocale::Korean).toString(static_cast<qlonglong>(s.value)))
                .arg(QString::number(pct, 'f', 1)), row);
        val->setTextFormat(Qt::RichText);
        val->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        val->setStyleSheet(
            "color: #191F28; font-size: 13px; font-weight: 700;"
            "background: transparent; border: none;");

        rowL->addWidget(dot);
        rowL->addWidget(name);
        rowL->addStretch();
        rowL->addWidget(val);

        legendLayout->addWidget(row);
    }
    legendLayout->addStretch();
}
