#ifndef ACCOUNTPROCESSPAGE_H
#define ACCOUNTPROCESSPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

class QLabel;
class QPushButton;
class QFrame;
class QLineEdit;
class QVBoxLayout;
class QScrollArea;
class QComboBox;
class QRadioButton;
class QDoubleSpinBox;

class AccountProcessPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountProcessPage(QWidget *parent = nullptr);

public slots:
    void loadAccounts();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onAccountChanged(int index);
    void onExecuteClicked();
    void onResponseReceived(const QJsonObject &response);

private:
    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();

    void loadAccountDetail(const QString &accountNumber);
    void clearHistoryLabels();
    void addHistoryLabel(const QString &type, const QString &accountNumber, const QString &text);
    void updateTotalLabel();

private:
    QFrame *m_leftCard;
    QFrame *m_rightCard;

    // 좌측
    QLabel *m_titleLabel;
    QComboBox *m_accountCombo;
    QLabel *m_accountNumberLabel;
    QLabel *m_balanceLabel;

    QWidget *m_historyContainer;
    QVBoxLayout *m_historyLayout;
    QLabel *m_totalLabel;

    // 우측
    QLabel *m_detailTitleLabel;
    QLabel *m_selectedAccountLabel;
    QLabel *m_selectedTypeLabel;
    QLabel *m_selectedBalanceLabel;

    QLineEdit *m_accountPasswordEdit;
    QRadioButton *m_depositRadio;
    QRadioButton *m_withdrawRadio;
    QDoubleSpinBox *m_amountSpin;
    QLineEdit *m_descriptionEdit;
    QPushButton *m_executeBtn;

    QString m_currentAccountNumber;
    QString m_currentAccountType;
    int m_currentBalance;
    int m_historyCount;

    QJsonArray m_allAccounts;
};

#endif // ACCOUNTPROCESSPAGE_H