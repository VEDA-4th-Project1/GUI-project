#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QJsonObject>

class QLabel;

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(const QString &userName, QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onResponseReceived(const QJsonObject &response);

private:
    void loadTotalBalance();

    QString m_userName;
    QLabel *m_labelWelcome;
    QLabel *m_labelMoney;
};

#endif // HOMEPAGE_H
