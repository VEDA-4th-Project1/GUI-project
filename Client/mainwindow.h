#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QWidget;
class QLabel;
class QPushButton;
class HomePage;
class NewAccountPage;
class AccountProcessPage;
class MyInfoPage;
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
    QStackedWidget *stackedWidget;

    HomePage *pageHome;
    NewAccountPage *pageNewAccount;
    MyInfoPage *pageMyInfo;
    AccountProcessPage *pageAccountProcess;

    QPushButton *btnHome;
    QPushButton *btnNewAccount;
    QPushButton *btnMyInfo;
    QPushButton *btnAccountProcess;
    QPushButton *btnLogout;

    void updateMenuStyle(QPushButton *selectedButton);
};

#endif // MAINWINDOW_H