#include "balancepiechart.h"

#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QVariantAnimation>
#include <QEasingCurve>
#include <QToolTip>
#include <QtMath>
#include <QLocale>

/**
 * 마우스 트래킹을 활성화하고 900ms OutCubic 등장 애니메이션을 초기화한다.
 * 툴팁 스타일은 다크 테마로 지정한다.
 */
BalancePieChart::BalancePieChart(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(180, 180);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);
    setStyleSheet(
        "QToolTip {"
        "  background-color: #191F28;"
        "  color: #FFFFFF;"
        "  border: 1px solid #3182F6;"
        "  border-radius: 6px;"
        "  padding: 8px 10px;"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "}");

    m_anim = new QVariantAnimation(this);
    m_anim->setStartValue(0.0);
    m_anim->setEndValue(1.0);
    m_anim->setDuration(900);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_anim, SIGNAL(valueChanged(QVariant)),
            this, SLOT(onAnimValueChanged(QVariant)));
}

/** 애니메이션 진행률(0~1)을 m_progress에 저장하고 repaint를 요청한다. */
void BalancePieChart::onAnimValueChanged(const QVariant &value)
{
    m_progress = value.toReal();
    update();
}

/**
 * 슬라이스 데이터를 설정하고 애니메이션을 처음부터 재시작한다.
 * 이전 호버 상태와 진행률을 초기화한다.
 */
void BalancePieChart::setAccounts(const QVector<Slice> &slices)
{
    m_slices = slices;
    m_total  = 0;
    for (const auto &s : m_slices) m_total += s.value;

    m_hoverIndex = -1;
    m_progress   = 0.0;
    m_anim->stop();
    m_anim->start();
    update();
}

/** 슬라이스를 비우고 빈 상태로 다시 그린다. */
void BalancePieChart::clearAccounts()
{
    m_slices.clear();
    m_total      = 0;
    m_hoverIndex = -1;
    m_progress   = 0.0;
    update();
}

/**
 * 위젯 크기에서 여백을 뺀 정사각형 파이 영역(rect)을 계산한다.
 * 리사이즈 시 중심이 이동하므로 paintEvent와 sliceAt 모두 이 함수를 사용한다.
 */
static QRectF pieRectFor(const QSize &size)
{
    const int   margin   = 16;
    const int   diameter = qMin(size.width(), size.height()) - margin * 2;
    const qreal cx = size.width()  / 2.0;
    const qreal cy = size.height() / 2.0;
    return QRectF(cx - diameter / 2.0, cy - diameter / 2.0, diameter, diameter);
}

/**
 * 파이차트를 다음 순서로 렌더링한다:
 * 1. 회색 원형 배경 트랙
 * 2. 슬라이스 — m_progress만큼 그린 뒤 나머지는 건너뜀
 *    (Qt drawPie 좌표: 0°=3시, CW는 음수 span으로 표현)
 * 3. 중앙 흰색 구멍 (holeRatio = 0.58)
 * 4. 구멍 안 "총 자산" 텍스트 + 총액
 */
void BalancePieChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF rect = pieRectFor(size());

    // 배경 도넛 트랙 (슬라이스가 없거나 잔액이 0일 때도 표시)
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#EEF1F5"));
    p.drawEllipse(rect);

    if (m_slices.isEmpty() || m_total <= 0.0) {
        p.setPen(QColor("#8B95A1"));
        QFont f = font(); f.setPointSize(11); f.setWeight(QFont::DemiBold);
        p.setFont(f);
        p.drawText(rect, Qt::AlignCenter, QString::fromUtf8("계좌가 없습니다"));
        return;
    }

    // 12시(=90°) 기준 CW로 각 슬라이스를 그린다.
    // maxSweep: 현재 애니메이션 진행률에 해당하는 최대 각도
    const qreal maxSweep = 360.0 * m_progress;
    qreal startDeg = 0.0;  // 누적 시작 각도
    qreal swept    = 0.0;  // 지금까지 그린 총 각도

    for (int i = 0; i < m_slices.size(); ++i) {
        const Slice &s = m_slices[i];
        qreal spanDeg = 360.0 * (s.value / m_total);

        qreal remaining = maxSweep - swept;
        if (remaining <= 0.0) break;
        qreal drawDeg = qMin(spanDeg, remaining);

        QColor color = s.color;
        if (m_hoverIndex == i) color = color.lighter(112);  // 호버 시 밝게

        p.setBrush(color);
        p.setPen(Qt::NoPen);

        // Qt 각도 단위 변환 (1/16도):
        // CW 시작 = 12시에서 startDeg만큼 CW 이동 → 90 - startDeg
        int qtStart = static_cast<int>(std::lround(16.0 * (90.0 - startDeg)));
        int qtSpan  = static_cast<int>(std::lround(-16.0 * drawDeg));  // 음수 = CW
        p.drawPie(rect, qtStart, qtSpan);

        startDeg += spanDeg;
        swept    += spanDeg;
    }

    // 도넛 중앙 구멍 — 흰색 원으로 파이 위에 덮어 그린다
    const qreal holeRatio = 0.58;
    qreal hw = rect.width()  * holeRatio;
    qreal hh = rect.height() * holeRatio;
    QRectF hole(rect.center().x() - hw / 2.0,
                rect.center().y() - hh / 2.0,
                hw, hh);
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(hole);

    // 구멍 상단 "총 자산" 보조 텍스트
    p.setPen(QColor("#8B95A1"));
    QFont subFont = font(); subFont.setPointSize(9); subFont.setWeight(QFont::DemiBold);
    p.setFont(subFont);
    QRectF topText = hole.adjusted(0, hole.height() * 0.28, 0, 0);
    p.drawText(topText, Qt::AlignHCenter | Qt::AlignTop, QString::fromUtf8("총 자산"));

    // 구멍 중앙 총 자산 금액 (한국어 로케일로 천 단위 콤마 포함)
    p.setPen(QColor("#191F28"));
    QFont mainFont = font(); mainFont.setPointSize(14); mainFont.setWeight(QFont::Black);
    p.setFont(mainFont);
    QString totalText = QLocale(QLocale::Korean).toString(static_cast<qlonglong>(m_total))
                        + QString::fromUtf8("원");
    QRectF midText = hole.adjusted(0, hole.height() * 0.44, 0, 0);
    p.drawText(midText, Qt::AlignHCenter | Qt::AlignTop, totalText);
}

/**
 * 마우스 위치에서 파이 중심까지의 거리·각도를 계산해 슬라이스 인덱스를 반환한다.
 * 파이 바깥(-1)이면 히트 없음.
 *
 * 각도 계산 (12시 기준 CW):
 *   angle = atan2(dx, -dy)  [라디안 → 도]
 *   12시(dy=-r, dx=0): atan2(0, r) = 0°
 *   3시(dx=r, dy=0):  atan2(r, 0) = 90°
 *   6시(dy=r, dx=0):  atan2(0, -r) = 180°
 *   9시(dx=-r, dy=0): atan2(-r, 0) = -90° → +360 = 270°
 */
int BalancePieChart::sliceAt(const QPointF &pos) const
{
    if (m_slices.isEmpty() || m_total <= 0.0) return -1;

    const QRectF  rect   = pieRectFor(size());
    const QPointF center = rect.center();
    const qreal   dx     = pos.x() - center.x();
    const qreal   dy     = pos.y() - center.y();
    const qreal   dist   = std::hypot(dx, dy);

    const qreal outer = rect.width() / 2.0;
    if (outer <= 0.0 || dist > outer) return -1;

    qreal angle = qRadiansToDegrees(std::atan2(dx, -dy));
    if (angle < 0) angle += 360.0;

    qreal acc = 0.0;
    for (int i = 0; i < m_slices.size(); ++i) {
        qreal span = 360.0 * (m_slices[i].value / m_total);
        if (angle >= acc && angle < acc + span) return i;
        acc += span;
    }
    return m_slices.size() - 1;  // 부동소수점 오차로 마지막 슬라이스를 반환
}

/**
 * 슬라이스 인덱스가 변경될 때만 update()를 호출해 불필요한 repaint를 방지한다.
 * 슬라이스 위에 있을 때 HTML 툴팁(계좌번호·잔액·비중)을 표시한다.
 */
void BalancePieChart::mouseMoveEvent(QMouseEvent *event)
{
    int idx = sliceAt(event->position());
    if (idx != m_hoverIndex) {
        m_hoverIndex = idx;
        update();
    }
    if (idx >= 0) {
        const Slice &s = m_slices[idx];
        const double pct = m_total > 0 ? (s.value / m_total) * 100.0 : 0.0;
        QString balStr = QLocale(QLocale::Korean).toString(static_cast<qlonglong>(s.value));
        QString sub    = s.subLabel.isEmpty() ? QString() : QString(" (%1)").arg(s.subLabel);
        QString tip    = QString("<b>%1</b>%2<br>잔액: %3원<br>비중: %4%")
                             .arg(s.label, sub, balStr,
                                  QString::number(pct, 'f', 1));
        QToolTip::showText(event->globalPosition().toPoint(), tip, this);
    } else {
        QToolTip::hideText();
    }
    QWidget::mouseMoveEvent(event);
}

/** 마우스가 위젯을 벗어나면 호버 강조를 해제하고 툴팁을 숨긴다. */
void BalancePieChart::leaveEvent(QEvent *event)
{
    if (m_hoverIndex != -1) {
        m_hoverIndex = -1;
        update();
    }
    QToolTip::hideText();
    QWidget::leaveEvent(event);
}

/**
 * 리사이즈 시 파이 중심이 이동하므로 m_hoverIndex를 -1로 초기화한다.
 * 다음 mouseMoveEvent에서 새 중심 기준으로 슬라이스를 재계산한다.
 */
void BalancePieChart::resizeEvent(QResizeEvent *event)
{
    m_hoverIndex = -1;
    QToolTip::hideText();
    QWidget::resizeEvent(event);
    update();
}
