#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QJsonObject>

class QLabel;
class QFrame;
class BalancePieChart;

/**
 * @class HomePage
 * @brief 로그인 후 첫 화면에 표시되는 홈 페이지.
 *
 * 구성 요소:
 * - 히어로 카드: 사용자 환영 메시지 + 총 보유 자산 금액
 * - 안내 카드: Easy Bank 서비스 소개 텍스트
 * - 파이차트 카드: 계좌별 자산 분포를 도넛 차트와 범례로 표시
 *
 * 페이지가 보일 때마다(showEvent) 서버에 "list_accounts" 요청을 보내
 * 최신 잔액 정보를 갱신한다.
 */
class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

protected:
    /**
     * @brief 페이지가 화면에 표시될 때 자동으로 호출된다.
     *
     * loadTotalBalance()를 통해 서버에서 최신 계좌 정보를 요청한다.
     */
    void showEvent(QShowEvent *event) override;

private slots:
    /**
     * @brief NetworkClient의 responseReceived 시그널에 연결되는 슬롯.
     *
     * "list_accounts_response" 타입만 처리한다.
     * 응답에서 잔액 합산값을 계산해 m_labelMoney를 업데이트하고,
     * 파이차트 슬라이스 데이터와 범례를 재구성한다.
     */
    void onResponseReceived(const QJsonObject &response);

private:
    /** 서버에 "list_accounts" 요청을 전송해 잔액 정보를 요청한다. */
    void loadTotalBalance();

    QFrame         *m_heroCard;    ///< 총 자산을 표시하는 블루 그라디언트 히어로 카드
    QLabel         *m_labelMoney;  ///< 총 보유 자산 금액을 표시하는 라벨
    BalancePieChart *m_pieChart;   ///< 계좌별 자산 분포 도넛 파이차트 위젯
    QFrame         *m_legendCard;  ///< 파이차트 옆 범례(계좌번호·금액·비중) 컨테이너
};

#endif // HOMEPAGE_H
