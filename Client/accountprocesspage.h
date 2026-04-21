#ifndef ACCOUNTPROCESSPAGE_H
#define ACCOUNTPROCESSPAGE_H

#include <QWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QHash>

class QLabel;
class QPushButton;
class QFrame;
class QLineEdit;
class QVBoxLayout;
class QScrollArea;
class QComboBox;
class QRadioButton;
class QDoubleSpinBox;
class QShowEvent;
class QListWidget;
class QListWidgetItem;

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
    void onDetailAccountChanged(int index);
    void onExecuteClicked();
    void onResponseReceived(const QJsonObject &response);
    void onWithdrawRadioClicked(bool checked);
    void onSearchTextChanged(const QString &text);
    void onSearchResultClicked(QListWidgetItem *item);

private:
    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();

    void loadAccountDetail(const QString &accountNumber, const QString &password);
    void tryLoadLeftAccountDetail();

    void clearHistoryLabels();
    void addHistoryLabel(const QString &type, const QString &accountNumber, const QString &text);
    void updateTotalLabel();
    void renderHistoryLabels(const QJsonArray &transactions);
    void applyHistoryFilter();
    void fillSearchResults(const QString &keyword);

    void updateLeftSummary(const QJsonObject &acc);
    void updateRightSummary(const QJsonObject &acc);

private:
    QFrame *m_leftCard;
    QFrame *m_rightCard;

    // 좌측: 계좌 조회
    QLabel *m_titleLabel;
    QLineEdit *m_searchEdit;
    QListWidget *m_searchResultList;
    QLabel *m_accountNumberLabel;
    QLabel *m_balanceLabel;

    QWidget *m_historyContainer;
    QVBoxLayout *m_historyLayout;
    QLabel *m_totalLabel;

    // 우측: 계좌 상세 / 입출금
    QLabel *m_detailTitleLabel;
    QComboBox *m_detailAccountCombo;
    QLabel *m_selectedAccountLabel;
    QLabel *m_selectedTypeLabel;
    QLabel *m_selectedBalanceLabel;

    QLineEdit *m_accountPasswordEdit;
    QRadioButton *m_depositRadio;
    QRadioButton *m_withdrawRadio;
    QDoubleSpinBox *m_amountSpin;
    QLineEdit *m_descriptionEdit;
    QPushButton *m_executeBtn;

    // 왼쪽 상태
    QString m_leftAccountNumber;
    int m_leftBalance;

    // 오른쪽 상태
    QString m_rightAccountNumber;
    QString m_rightAccountType;
    int m_rightBalance;

    // 계좌별 비밀번호 캐시
    QHash<QString, QString> m_accountPasswordCache;

    int m_historyCount;

    QJsonArray m_allAccounts;
    QJsonArray m_currentTransactions;
};

#endif // ACCOUNTPROCESSPAGE_H