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

void BalancePieChart::onAnimValueChanged(const QVariant &value)
{
    m_progress = value.toReal();
    update();
}

void BalancePieChart::setAccounts(const QVector<Slice> &slices)
{
    m_slices = slices;
    m_total = 0;
    for (const auto &s : m_slices) m_total += s.value;

    m_hoverIndex = -1;
    m_progress = 0.0;
    m_anim->stop();
    m_anim->start();
    update();
}

void BalancePieChart::clearAccounts()
{
    m_slices.clear();
    m_total = 0;
    m_hoverIndex = -1;
    m_progress = 0.0;
    update();
}

static QRectF pieRectFor(const QSize &size)
{
    const int margin = 16;
    const int diameter = qMin(size.width(), size.height()) - margin * 2;
    const qreal cx = size.width()  / 2.0;
    const qreal cy = size.height() / 2.0;
    return QRectF(cx - diameter / 2.0, cy - diameter / 2.0, diameter, diameter);
}

void BalancePieChart::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRectF rect = pieRectFor(size());

    // 배경 도넛 트랙
    p.setPen(Qt::NoPen);
    p.setBrush(QColor("#EEF1F5"));
    p.drawEllipse(rect);

    if (m_slices.isEmpty() || m_total <= 0.0) {
        // 빈 상태 텍스트
        p.setPen(QColor("#8B95A1"));
        QFont f = font(); f.setPointSize(11); f.setWeight(QFont::DemiBold);
        p.setFont(f);
        p.drawText(rect, Qt::AlignCenter, QString::fromUtf8("계좌가 없습니다"));
        return;
    }

    // 조각 그리기: 12시 기준 시계방향(CW)
    // Qt drawPie: 0°=3시, 양의 span=CCW. CW로 그리려면 span을 음수로.
    const qreal maxSweep = 360.0 * m_progress;
    qreal startDeg = 0.0;   // 12시에서 CW로 누적된 각도
    qreal swept   = 0.0;

    for (int i = 0; i < m_slices.size(); ++i) {
        const Slice &s = m_slices[i];
        qreal spanDeg = 360.0 * (s.value / m_total);

        qreal remaining = maxSweep - swept;
        if (remaining <= 0.0) break;
        qreal drawDeg = qMin(spanDeg, remaining);

        QColor color = s.color;
        if (m_hoverIndex == i) color = color.lighter(112);

        p.setBrush(color);
        p.setPen(Qt::NoPen);

        // 현재 조각의 Qt 시작 각도(1/16°): 12시(=90°)에서 startDeg만큼 CW로 이동
        int qtStart = static_cast<int>(std::lround(16.0 * (90.0 - startDeg)));
        int qtSpan  = static_cast<int>(std::lround(-16.0 * drawDeg));  // 음수 = CW
        p.drawPie(rect, qtStart, qtSpan);

        startDeg += spanDeg;
        swept   += spanDeg;
    }

    // 도넛 중앙 구멍
    const qreal holeRatio = 0.58;
    QRectF hole = rect;
    qreal hw = rect.width()  * holeRatio;
    qreal hh = rect.height() * holeRatio;
    hole.setX(rect.center().x() - hw / 2.0);
    hole.setY(rect.center().y() - hh / 2.0);
    hole.setWidth(hw);
    hole.setHeight(hh);
    p.setBrush(QColor("#FFFFFF"));
    p.drawEllipse(hole);

    // 중앙 텍스트: 총 잔액
    p.setPen(QColor("#8B95A1"));
    QFont subFont = font(); subFont.setPointSize(9); subFont.setWeight(QFont::DemiBold);
    p.setFont(subFont);
    QRectF topText = hole.adjusted(0, hole.height() * 0.28, 0, 0);
    p.drawText(topText, Qt::AlignHCenter | Qt::AlignTop, QString::fromUtf8("총 자산"));

    p.setPen(QColor("#191F28"));
    QFont mainFont = font(); mainFont.setPointSize(14); mainFont.setWeight(QFont::Black);
    p.setFont(mainFont);
    QString totalText = QLocale(QLocale::Korean).toString(static_cast<qlonglong>(m_total)) + QString::fromUtf8("원");
    QRectF midText = hole.adjusted(0, hole.height() * 0.44, 0, 0);
    p.drawText(midText, Qt::AlignHCenter | Qt::AlignTop, totalText);
}

int BalancePieChart::sliceAt(const QPointF &pos) const
{
    if (m_slices.isEmpty() || m_total <= 0.0) return -1;

    // 현재 위젯 크기 기준 파이 rect — 리사이즈 대응
    const QRectF rect = pieRectFor(size());
    const QPointF center = rect.center();
    const qreal dx = pos.x() - center.x();
    const qreal dy = pos.y() - center.y();
    const qreal dist = std::hypot(dx, dy);

    const qreal outer = rect.width() / 2.0;
    if (outer <= 0.0) return -1;
    if (dist > outer) return -1;   // 파이 원형 바깥은 히트 없음

    // 12시 기준 시계방향(CW) 각도[0,360)
    // 12시: dx=0, dy=-r → atan2(0, r)=0
    // 3시:  dx=r, dy=0  → atan2(r, 0)=π/2=90
    // 6시:  dx=0, dy=r  → atan2(0,-r)=π=180
    // 9시:  dx=-r,dy=0  → atan2(-r,0)=-π/2 → +360=270
    qreal angle = qRadiansToDegrees(std::atan2(dx, -dy));
    if (angle < 0) angle += 360.0;

    qreal acc = 0.0;
    for (int i = 0; i < m_slices.size(); ++i) {
        qreal span = 360.0 * (m_slices[i].value / m_total);
        if (angle >= acc && angle < acc + span) return i;
        acc += span;
    }
    return m_slices.size() - 1;
}

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
        QString sub = s.subLabel.isEmpty() ? QString() : QString(" (%1)").arg(s.subLabel);
        QString tip = QString("<b>%1</b>%2<br>잔액: %3원<br>비중: %4%")
                          .arg(s.label, sub, balStr,
                               QString::number(pct, 'f', 1));
        QToolTip::showText(event->globalPosition().toPoint(), tip, this);
    } else {
        QToolTip::hideText();
    }
    QWidget::mouseMoveEvent(event);
}

void BalancePieChart::leaveEvent(QEvent *event)
{
    if (m_hoverIndex != -1) {
        m_hoverIndex = -1;
        update();
    }
    QToolTip::hideText();
    QWidget::leaveEvent(event);
}

void BalancePieChart::resizeEvent(QResizeEvent *event)
{
    // 리사이즈 시 파이 중심이 이동 → 호버 인덱스가 더이상 유효하지 않으므로 리셋 후 다음 마우스 이동에서 재계산
    m_hoverIndex = -1;
    QToolTip::hideText();
    QWidget::resizeEvent(event);
    update();
}
