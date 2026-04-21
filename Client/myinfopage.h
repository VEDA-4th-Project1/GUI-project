#ifndef MYINFOPAGE_H
#define MYINFOPAGE_H

#include <QWidget>
#include <QLabel>
#include <QJsonObject>

class QDialog;
class QLineEdit;
class QComboBox;
class QPushButton;

class MyInfoPage : public QWidget
{
    Q_OBJECT

public:
    explicit MyInfoPage(QWidget *parent = nullptr);
    void loadUserInfo();

private slots:
    void onNetworkResponse(const QJsonObject &resp);
    void openChangePasswordDialog();
    void openCloseAccountDialog();
    void onChangePwOkClicked();
    void onCloseAccOkClicked();

private:
    void setupUI();

    QLabel      *m_idLabel;
    QLabel      *m_nameLabel;
    QLabel      *m_createdAtLabel;

    QLabel      *m_avatarLabel      = nullptr;
    QLabel      *m_headerNameLabel  = nullptr;
    QLabel      *m_statAccountsVal  = nullptr;
    QLabel      *m_statTotalVal     = nullptr;
    QLabel      *m_statDaysVal      = nullptr;
    QWidget     *m_accountsListBox  = nullptr;

    QStringList  m_accountList;
    double       m_totalBalance     = 0.0;

    // 비밀번호 변경 다이얼로그 위젯
    QDialog     *m_changePwDlg    = nullptr;
    QLineEdit   *m_currentPwEdit  = nullptr;
    QLineEdit   *m_newPwEdit      = nullptr;
    QLineEdit   *m_confirmPwEdit  = nullptr;

    // 계좌 해지 다이얼로그 위젯
    QDialog     *m_closeAccDlg    = nullptr;
    QComboBox   *m_closeAccCombo  = nullptr;
    QLineEdit   *m_closeAccPwEdit = nullptr;
    QPushButton *m_closeAccOkBtn  = nullptr;
};

#endif // MYINFOPAGE_H