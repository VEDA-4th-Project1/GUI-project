#ifndef NEWACCOUNTPAGE_H
#define NEWACCOUNTPAGE_H

#include <QWidget>
#include <QJsonObject>

class QLineEdit;
class QComboBox;
class QDoubleSpinBox;
class QPushButton;

class NewAccountPage : public QWidget {
    Q_OBJECT

public:
    explicit NewAccountPage(QWidget* parent = nullptr);

private slots:
    void onCreateClicked();
    void onTypeChanged(int index);
    void onNetworkResponse(const QJsonObject& response);

private:
    void setupUI();

    QComboBox*   m_typeCombo;
    QLineEdit*   m_accPasswordEdit;
    QDoubleSpinBox* m_initBalanceSpin;
    QWidget*     m_rateWidget;   // 저축계좌 전용 (고정 이율 표시)
    QWidget*     m_limitWidget;  // 입출금계좌 전용 (0원 고정 표시)
    QPushButton* m_createBtn;
};

#endif // NEWACCOUNTPAGE_H
