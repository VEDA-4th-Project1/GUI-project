#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QWidget;
class QLabel;
class QPushButton;
class NewAccountPage;

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

private:
    QString userName;
    int money;

    QStackedWidget *stackedWidget;

    QWidget *pageHome;
    NewAccountPage *pageNewAccount;
    QWidget *pageMyInfo;
    QWidget *pageAccountProcess;

    QLabel *labelWelcome;
    QLabel *labelMoney;

    QPushButton *btnHome;
    QPushButton *btnNewAccount;
    QPushButton *btnMyInfo;
    QPushButton *btnAccountProcess;

    void updateMenuStyle(QPushButton *selectedButton);
};

#endif // MAINWINDOW_H