#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QWidget;
class QLabel;
class QPushButton;
class NewAccountPage;
class AccountProcessPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void goHome();
    void goNewAccount();
    void goMyInfo();
    void goAccountProcess();
    void onLogoutClicked();

private:
    QString userName;
    int money;

    QStackedWidget *stackedWidget;

    QWidget *pageHome;
    NewAccountPage *pageNewAccount;
    QWidget *pageMyInfo;
    AccountProcessPage *pageAccountProcess;

    QLabel *labelWelcome;
    QLabel *labelMoney;

    QPushButton *btnHome;
    QPushButton *btnNewAccount;
    QPushButton *btnMyInfo;
    QPushButton *btnAccountProcess;
    QPushButton *btnLogout;

    void updateMenuStyle(QPushButton *selectedButton);
};

#endif // MAINWINDOW_H