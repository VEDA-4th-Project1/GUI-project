#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QJsonObject>

class MainWindow;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

private slots:
    void onRegisterClicked();
    void onConfirmClicked();
    void onCancelClicked();

    // 서버 응답 처리 (login_response)
    void onNetworkResponse(const QJsonObject& response);

private:
    void setupUI();
    void applyStyles();

    QFrame      *m_loginCard;

    QLabel      *m_titleLabel;
    QLabel      *m_subtitleLabel;
    QLabel      *m_idLabel;
    QLabel      *m_pwLabel;

    QLineEdit   *m_idEdit;
    QLineEdit   *m_pwEdit;

    QPushButton *m_registerBtn;
    QPushButton *m_confirmBtn;
    QPushButton *m_cancelBtn;

    MainWindow  *m_mainWindow;
};

#endif // LOGINDIALOG_H