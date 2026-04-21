#include "mainwindow.h"
#include "appstyle.h"
#include "sessioncontext.h"
#include "homepage.h"
#include "newaccountpage.h"
#include "myinfopage.h"
#include "accountprocesspage.h"
#include "logindialog.h"
#include "networkclient.h"

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QFont>
#include <QFrame>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    resize(1100, 700);

    central->setStyleSheet(AppStyle::PAGE_BG);

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

    QLabel *userLabel = new QLabel(QString("%1님\n환영합니다").arg(SessionContext::instance().userName()));
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
        menuButtons[i]->setStyleSheet(AppStyle::SIDEBAR_BTN);
    }

    // 로그아웃 버튼 (팀원2 추가)
    btnLogout = new QPushButton("로그아웃");
    btnLogout->setCursor(Qt::PointingHandCursor);
    btnLogout->setMinimumHeight(35);
    btnLogout->setStyleSheet(AppStyle::BTN_RED);

    sideLayout->addWidget(logoLabel);
    sideLayout->addWidget(userLabel);
    sideLayout->addSpacing(5);
    sideLayout->addWidget(btnLogout);
    sideLayout->addSpacing(10);
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
    pageHome = new HomePage(this);

    // 신규계좌 관리 화면
    pageNewAccount = new NewAccountPage(this);

    // 내정보 화면
    pageMyInfo = new MyInfoPage(this);

    // 계좌처리 화면 (팀원 모듈)
    pageAccountProcess = new AccountProcessPage(this);

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
    connect(btnLogout, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);

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
        if (menuButtons[i] == selectedButton)
            menuButtons[i]->setStyleSheet(AppStyle::SIDEBAR_BTN_ACTIVE);
        else
            menuButtons[i]->setStyleSheet(AppStyle::SIDEBAR_BTN);
    }
}

void MainWindow::goHome()
{
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
    pageMyInfo->loadUserInfo();
    stackedWidget->setCurrentWidget(pageMyInfo);
    updateMenuStyle(btnMyInfo);
}

void MainWindow::goAccountProcess()
{
    stackedWidget->setCurrentWidget(pageAccountProcess);
    updateMenuStyle(btnAccountProcess);

}

void MainWindow::onLogoutClicked()
{
    QJsonObject request;
    request["type"]  = "logout";
    request["token"] = SessionContext::instance().token();
    request["data"]  = QJsonObject{};

    NetworkClient::instance()->sendRequest(request);

    SessionContext::instance().clear();

    LoginDialog *loginDlg = new LoginDialog();
    loginDlg->setAttribute(Qt::WA_DeleteOnClose);
    loginDlg->show();

    this->close();
}