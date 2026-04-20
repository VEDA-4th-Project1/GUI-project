#ifndef ACCOUNTPROCESSPAGE_H
#define ACCOUNTPROCESSPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>

class QLabel;
class QPushButton;
class QFrame;
class QLineEdit;
class QTableWidget;
class QRadioButton;
class QDoubleSpinBox;

class AccountProcessPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountProcessPage(QWidget *parent = nullptr);

    void loadAccounts();

private slots:
    void onSearchChanged(const QString &keyword);
    void onAccountSelected(int row, int column);
    void onTransactionClicked();
    void onResponseReceived(const QJsonObject &response);

private:
    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();

    void refreshTable(const QJsonArray &accounts);
    void loadAccountDetail(const QString &accountNumber);
    void showDetail(const QJsonObject &account);

private:
    QFrame *m_leftCard;
    QFrame *m_rightCard;

    // 좌측
    QLabel *m_leftTitleLabel;
    QLineEdit *m_searchEdit;
    QTableWidget *m_accountTable;

    // 우측 상세
    QLabel *m_rightTitleLabel;
    QLabel *m_accountNumberValueLabel;
    QLabel *m_accountTypeValueLabel;
    QLabel *m_balanceValueLabel;

    QLineEdit *m_accountPasswordEdit;
    QRadioButton *m_depositRadio;
    QRadioButton *m_withdrawRadio;
    QDoubleSpinBox *m_amountSpin;
    QLineEdit *m_descriptionEdit;
    QPushButton *m_executeBtn;

    QTableWidget *m_transactionTable;

    QString m_selectedAccountNumber;
    QJsonArray m_allAccounts;
};

#endif // ACCOUNTPROCESSPAGE_H
