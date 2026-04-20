#ifndef RESISTERDIALOG_H
#define RESISTERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QDateEdit>
#include <QJsonObject>

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(QWidget *parent = nullptr);

private slots:
    void onDuplicateCheckClicked();
    void onConfirmClicked();
    void onCancelClicked();

    // 아이디 수정 시 중복확인 상태 초기화
    void onIdTextChanged();

    // 서버 응답 처리 (check_id_response / register_response)
    void onNetworkResponse(const QJsonObject& response);

private:
    void setupUI();
    void applyStyles();
    bool validateInputs();
    void setIdBorderColor(const QString &color);

    QFrame      *m_card;

    QLabel      *m_titleLabel;

    QLabel      *m_nameLabel;
    QLineEdit   *m_nameEdit;

    QLabel      *m_birthLabel;
    QDateEdit   *m_birthEdit;

    QLabel      *m_idLabel;
    QLineEdit   *m_idEdit;
    QPushButton *m_dupCheckBtn;

    QLabel      *m_pwLabel;
    QLineEdit   *m_pwEdit;

    QLabel      *m_pwConfirmLabel;
    QLineEdit   *m_pwConfirmEdit;

    QPushButton *m_confirmBtn;
    QPushButton *m_cancelBtn;

    bool         m_idChecked = false;
};


#endif // RESISTERDIALOG_H
