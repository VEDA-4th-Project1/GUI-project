#ifndef BALANCEPIECHART_H
#define BALANCEPIECHART_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>

class QVariantAnimation;

class BalancePieChart : public QWidget
{
    Q_OBJECT
public:
    struct Slice {
        QString label;      // 계좌번호 또는 계좌 라벨
        QString subLabel;   // 계좌 종류 등
        double  value;      // 잔액
        QColor  color;
    };

    explicit BalancePieChart(QWidget *parent = nullptr);

    void setAccounts(const QVector<Slice> &slices);
    void clearAccounts();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimValueChanged(const QVariant &value);

private:
    int sliceAt(const QPointF &pos) const;

    QVector<Slice> m_slices;
    double         m_total = 0.0;
    qreal          m_progress = 0.0;   // 0.0 → 1.0 애니메이션
    int            m_hoverIndex = -1;
    QVariantAnimation *m_anim = nullptr;
};

#endif // BALANCEPIECHART_H
