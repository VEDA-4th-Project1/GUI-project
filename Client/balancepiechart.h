#ifndef BALANCEPIECHART_H
#define BALANCEPIECHART_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>

class QVariantAnimation;

/**
 * @class BalancePieChart
 * @brief 계좌별 자산 분포를 표시하는 커스텀 도넛 파이차트 위젯.
 *
 * 동작:
 * - setAccounts() 호출 시 900ms OutCubic 애니메이션으로 파이를 그린다.
 * - 12시 기준 시계방향(CW)으로 각 슬라이스를 그린다.
 * - 마우스를 올리면 해당 슬라이스를 살짝 밝게 강조하고 툴팁을 표시한다.
 * - 중앙 구멍에 총 자산 금액을 표시한다.
 * - 리사이즈 시 호버 상태를 초기화해 중심 이동 후 올바른 슬라이스를 탐지한다.
 *
 * 좌표 관례:
 * - Qt drawPie: 0° = 3시, 양의 span = 반시계방향(CCW)
 * - 이 위젯: 12시 기준 CW → qtStart = 16*(90 - startDeg), qtSpan = -16*drawDeg
 */
class BalancePieChart : public QWidget
{
    Q_OBJECT
public:
    /** 파이차트 한 조각(슬라이스)의 데이터 구조체. */
    struct Slice {
        QString label;      ///< 계좌번호 또는 계좌 식별 라벨
        QString subLabel;   ///< 계좌 종류 ("savings" / "checking")
        double  value;      ///< 잔액 (파이 비중 계산에 사용)
        QColor  color;      ///< 슬라이스 색상
    };

    explicit BalancePieChart(QWidget *parent = nullptr);

    /**
     * @brief 파이차트 데이터를 설정하고 애니메이션을 시작한다.
     * @param slices 표시할 슬라이스 벡터 (value > 0인 항목만 전달)
     */
    void setAccounts(const QVector<Slice> &slices);

    /** 모든 슬라이스를 제거하고 빈 상태로 다시 그린다. */
    void clearAccounts();

protected:
    /**
     * @brief 파이차트를 그린다.
     *
     * 배경 원형 트랙 → 각 슬라이스(애니메이션 진행률 반영) →
     * 중앙 흰색 구멍 → "총 자산" 텍스트 순으로 렌더링한다.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * @brief 마우스 커서 위치의 슬라이스를 강조하고 툴팁을 표시한다.
     *
     * sliceAt()으로 슬라이스 인덱스를 구해 m_hoverIndex를 갱신하고
     * 인덱스가 변경된 경우에만 update()를 호출해 불필요한 repaint를 방지한다.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /** 마우스가 위젯을 벗어나면 호버 상태를 해제하고 툴팁을 숨긴다. */
    void leaveEvent(QEvent *event) override;

    /**
     * @brief 위젯 크기가 변경될 때 파이 중심 좌표가 이동하므로 호버를 초기화한다.
     *
     * 다음 mouseMoveEvent에서 새 위치 기준으로 슬라이스를 재계산한다.
     */
    void resizeEvent(QResizeEvent *event) override;

private slots:
    /**
     * @brief m_anim의 valueChanged 시그널에 연결되는 슬롯.
     * @param value 현재 애니메이션 진행률 (0.0 ~ 1.0)
     *
     * m_progress를 갱신하고 update()로 repaint를 요청한다.
     */
    void onAnimValueChanged(const QVariant &value);

private:
    /**
     * @brief pos 위치에 해당하는 슬라이스 인덱스를 반환한다.
     * @param pos 위젯 좌표계 기준 마우스 위치
     * @return 슬라이스 인덱스 (파이 바깥이면 -1)
     *
     * 각도 계산: atan2(dx, -dy)로 12시 기준 CW 각도[0,360)를 구한다.
     */
    int sliceAt(const QPointF &pos) const;

    QVector<Slice>     m_slices;        ///< 현재 표시 중인 슬라이스 목록
    double             m_total = 0.0;   ///< 전체 잔액 합산값 (비중 계산용)
    qreal              m_progress = 0.0;///< 애니메이션 진행률 (0.0 → 1.0)
    int                m_hoverIndex = -1; ///< 현재 마우스가 올라간 슬라이스 인덱스 (-1: 없음)
    QVariantAnimation *m_anim = nullptr;  ///< 파이 등장 애니메이션 객체
};

#endif // BALANCEPIECHART_H
