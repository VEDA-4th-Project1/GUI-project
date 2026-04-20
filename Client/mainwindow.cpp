#include "mainwindow.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QFont>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), userName("홍길동"), money(0)//서버 넣어줘야함
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    resize(1100, 700);

    central->setStyleSheet("background-color: #F5F7FB;");

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 사이드 메뉴
    QWidget *sideMenuWidget = new QWidget;
    sideMenuWidget->setFixedWidth(240);
    sideMenuWidget->setStyleSheet(
        "background-color: #111827;"
        "border-right: 1px solid #1F2937;"
        );

    QVBoxLayout *sideLayout = new QVBoxLayout(sideMenuWidget);
    sideLayout->setContentsMargins(20, 24, 20, 24);
    sideLayout->setSpacing(12);

    QLabel *logoLabel = new QLabel("Easy Bank");
    logoLabel->setStyleSheet(
        "color: white;"
        "font-size: 24px;"
        "font-weight: bold;"
        );

    QLabel *userLabel = new QLabel(QString("%1님\n환영합니다").arg(userName));
    userLabel->setStyleSheet(
        "color: #D1D5DB;"
        "font-size: 14px;"
        "line-height: 1.6;"
        );

    QFrame *line = new QFrame;
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("color: #374151; background-color: #374151; max-height: 1px;");

    btnHome = new QPushButton("홈");
    btnNewAccount = new QPushButton("신규계좌 관리");
    btnMyInfo = new QPushButton("내정보");
    btnAccountProcess = new QPushButton("계좌처리");

    QList<QPushButton*> menuButtons;
    menuButtons << btnHome << btnNewAccount << btnMyInfo << btnAccountProcess;

    for (int i = 0; i < menuButtons.size(); i++) {
        menuButtons[i]->setMinimumHeight(48);
        menuButtons[i]->setCursor(Qt::PointingHandCursor);
        menuButtons[i]->setStyleSheet(
            "QPushButton {"
            "   background-color: transparent;"
            "   color: #E5E7EB;"
            "   text-align: left;"
            "   padding: 12px 16px;"
            "   border: none;"
            "   border-radius: 12px;"
            "   font-size: 15px;"
            "   font-weight: 600;"
            "}"
            "QPushButton:hover {"
            "   background-color: #1F2937;"
            "   color: white;"
            "}"
            );
    }

    sideLayout->addWidget(logoLabel);
    sideLayout->addWidget(userLabel);
    sideLayout->addSpacing(8);
    sideLayout->addWidget(line);
    sideLayout->addSpacing(10);
    sideLayout->addWidget(btnHome);
    sideLayout->addWidget(btnNewAccount);
    sideLayout->addWidget(btnMyInfo);
    sideLayout->addWidget(btnAccountProcess);
    sideLayout->addStretch();

    // 오른쪽 페이지 영역
    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet(
        "QStackedWidget {"
        "   background-color: #F5F7FB;"
        "}"
        );

    // 홈 화면
    pageHome = new QWidget;
    QVBoxLayout *homeLayout = new QVBoxLayout(pageHome);
    homeLayout->setContentsMargins(40, 40, 40, 40);
    homeLayout->setSpacing(20);

    QWidget *homeCard = new QWidget;
    homeCard->setStyleSheet(
        "background-color: white;"
        "border-radius: 20px;"
        "border: 1px solid #E5E7EB;"
        );

    QVBoxLayout *cardLayout = new QVBoxLayout(homeCard);
    cardLayout->setContentsMargins(30, 30, 30, 30);
    cardLayout->setSpacing(16);

    labelWelcome = new QLabel(QString("%1님 환영합니다").arg(userName)); // 서버 입력
    labelMoney = new QLabel(QString("보유 금액 : %1원").arg(money)); // 서버 입력

    labelWelcome->setStyleSheet(
        "font-size: 28px;"
        "font-weight: 700;"
        "color: #111827;"
        );

    labelMoney->setStyleSheet(
        "font-size: 20px;"
        "font-weight: 600;"
        "color: #2563EB;"
        );

    QLabel *subText = new QLabel("간편 계좌 관리 시스템에 오신 것을 환영합니다.");
    subText->setStyleSheet(
        "font-size: 14px;"
        "color: #6B7280;"
        );

    cardLayout->addWidget(labelWelcome);
    cardLayout->addWidget(labelMoney);
    cardLayout->addWidget(subText);

    homeLayout->addWidget(homeCard);
    homeLayout->addStretch();

    // 신규계좌 관리 화면
    pageNewAccount = new QWidget;
    QVBoxLayout *newAccountLayout = new QVBoxLayout(pageNewAccount);
    newAccountLayout->setContentsMargins(40, 40, 40, 40);
    QWidget *newAccountCard = new QWidget;
    newAccountCard->setStyleSheet(
        "background-color: white;"
        "border-radius: 20px;"
        "border: 1px solid #E5E7EB;"
        );
    QVBoxLayout *newCardLayout = new QVBoxLayout(newAccountCard);
    newCardLayout->setContentsMargins(30, 30, 30, 30);
    newCardLayout->addWidget(new QLabel("신규계좌 관리 화면"));
    newCardLayout->addStretch();
    newAccountLayout->addWidget(newAccountCard);

    // 내정보 화면
    pageMyInfo = new QWidget;
    QVBoxLayout *myInfoLayout = new QVBoxLayout(pageMyInfo);
    myInfoLayout->setContentsMargins(40, 40, 40, 40);
    QWidget *myInfoCard = new QWidget;
    myInfoCard->setStyleSheet(
        "background-color: white;"
        "border-radius: 20px;"
        "border: 1px solid #E5E7EB;"
        );
    QVBoxLayout *myCardLayout = new QVBoxLayout(myInfoCard);
    myCardLayout->setContentsMargins(30, 30, 30, 30);
    myCardLayout->addWidget(new QLabel("내정보 화면"));
    myCardLayout->addStretch();
    myInfoLayout->addWidget(myInfoCard);

    // 계좌처리 화면
    pageAccountProcess = new QWidget;
    QVBoxLayout *accountProcessLayout = new QVBoxLayout(pageAccountProcess);
    accountProcessLayout->setContentsMargins(40, 40, 40, 40);
    QWidget *accountCard = new QWidget;
    accountCard->setStyleSheet(
        "background-color: white;"
        "border-radius: 20px;"
        "border: 1px solid #E5E7EB;"
        );
    QVBoxLayout *accountCardLayout = new QVBoxLayout(accountCard);
    accountCardLayout->setContentsMargins(30, 30, 30, 30);
    accountCardLayout->addWidget(new QLabel("계좌처리 화면"));
    accountCardLayout->addStretch();
    accountProcessLayout->addWidget(accountCard);

    stackedWidget->addWidget(pageHome);
    stackedWidget->addWidget(pageNewAccount);
    stackedWidget->addWidget(pageMyInfo);
    stackedWidget->addWidget(pageAccountProcess);

    mainLayout->addWidget(sideMenuWidget);
    mainLayout->addWidget(stackedWidget);

    connect(btnHome, SIGNAL(clicked()), this, SLOT(goHome()));
    connect(btnNewAccount, SIGNAL(clicked()), this, SLOT(goNewAccount()));
    connect(btnMyInfo, SIGNAL(clicked()), this, SLOT(goMyInfo()));
    connect(btnAccountProcess, SIGNAL(clicked()), this, SLOT(goAccountProcess()));

    stackedWidget->setCurrentWidget(pageHome);
    updateMenuStyle(btnHome);
}

MainWindow::~MainWindow()
{
}

void MainWindow::updateMenuStyle(QPushButton *selectedButton)
{
    QList<QPushButton*> menuButtons;
    menuButtons << btnHome << btnNewAccount << btnMyInfo << btnAccountProcess;

    for (int i = 0; i < menuButtons.size(); i++) {
        if (menuButtons[i] == selectedButton) {
            menuButtons[i]->setStyleSheet(
                "QPushButton {"
                "   background-color: #2563EB;"
                "   color: white;"
                "   text-align: left;"
                "   padding: 12px 16px;"
                "   border: none;"
                "   border-radius: 12px;"
                "   font-size: 15px;"
                "   font-weight: 700;"
                "}"
                );
        } else {
            menuButtons[i]->setStyleSheet(
                "QPushButton {"
                "   background-color: transparent;"
                "   color: #E5E7EB;"
                "   text-align: left;"
                "   padding: 12px 16px;"
                "   border: none;"
                "   border-radius: 12px;"
                "   font-size: 15px;"
                "   font-weight: 600;"
                "}"
                "QPushButton:hover {"
                "   background-color: #1F2937;"
                "   color: white;"
                "}"
                );
        }
    }
}

void MainWindow::goHome()
{
    labelWelcome->setText(QString("%1님 환영합니다").arg(userName));
    labelMoney->setText(QString("보유 금액 : %1원").arg(money));
    stackedWidget->setCurrentWidget(pageHome);
    updateMenuStyle(btnHome);
}

void MainWindow::goNewAccount()
{
    stackedWidget->setCurrentWidget(pageNewAccount);
    updateMenuStyle(btnNewAccount);
}

void MainWindow::goMyInfo()
{
    stackedWidget->setCurrentWidget(pageMyInfo);
    updateMenuStyle(btnMyInfo);
}

void MainWindow::goAccountProcess()
{
    stackedWidget->setCurrentWidget(pageAccountProcess);
    updateMenuStyle(btnAccountProcess);
}