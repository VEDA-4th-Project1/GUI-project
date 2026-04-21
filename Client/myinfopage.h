#ifndef MYINFOPAGE_H
#define MYINFOPAGE_H

#include <QWidget>
#include <QLabel>
#include <QJsonObject>

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

private:
    void setupUI();

    QLabel      *m_idLabel;
    QLabel      *m_nameLabel;
    QLabel      *m_createdAtLabel;

    QStringList  m_accountList;
};

#endif // MYINFOPAGE_H