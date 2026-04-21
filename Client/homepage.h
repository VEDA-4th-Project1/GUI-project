#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QJsonObject>

class QLabel;
class QFrame;

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onResponseReceived(const QJsonObject &response);

private:
    void loadTotalBalance();

    QFrame *m_heroCard;
    QLabel *m_labelMoney;
};

#endif // HOMEPAGE_H
