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
#include <QFrame>
#include <QJsonObject>
#include <QIcon>
#include <QSize>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central = new QWidget(this);
    setCentralWidget(central);
    setMinimumSize(900, 600);
    resize(1100, 720);

    central->setStyleSheet("background-color: #F2F4F6;");

    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── 사이드바 ─────────────────────────────────────────────────────────────
    // QWidget#sidebar 셀렉터 대신 직접 스타일 지정 (Qt setStyleSheet 셀렉터 미적용 이슈 방지)
    QWidget *sideMenuWidget = new QWidget;
    sideMenuWidget->setFixedWidth(240);
    sideMenuWidget->setStyleSheet(
        "background-color: #0B1220;"
        "border-right: 1px solid #1E293B;");

    QVBoxLayout *sideLayout = new QVBoxLayout(sideMenuWidget);
    sideLayout->setContentsMargins(16, 28, 16, 24);
    sideLayout->setSpacing(4);

    // 로고
    QLabel *logoLabel = new QLabel("Easy Bank");
    logoLabel->setStyleSheet(AppStyle::SIDEBAR_LOGO);
    logoLabel->setContentsMargins(8, 0, 0, 0);

    QLabel *logoSub = new QLabel("PERSONAL BANKING");
    logoSub->setStyleSheet(AppStyle::SIDEBAR_LOGO_SUB);
    logoSub->setContentsMargins(8, 0, 0, 0);

    // 유저 카드
    QWidget *userCard = new QWidget;
    userCard->setStyleSheet(
        "QWidget {"
        "  background-color: #111B2E; border: 1px solid #1E293B; border-radius: 12px;"
        "}"
        "QLabel { background: transparent; border: none; }");
    QVBoxLayout *userCardLayout = new QVBoxLayout(userCard);
    userCardLayout->setContentsMargins(14, 12, 14, 12);
    userCardLayout->setSpacing(2);

    QLabel *helloLabel = new QLabel("WELCOME BACK");
    helloLabel->setStyleSheet(AppStyle::SIDEBAR_USER_HELLO);

    QLabel *nameLabel = new QLabel(SessionContext::instance().userName() + "님");
    nameLabel->setStyleSheet(AppStyle::SIDEBAR_USER_NAME);

    userCardLayout->addWidget(helloLabel);
    userCardLayout->addWidget(nameLabel);

    // 메뉴 섹션
    QLabel *menuSection = new QLabel("MENU");
    menuSection->setStyleSheet(AppStyle::SIDEBAR_SECTION);
    menuSection->setContentsMargins(8, 0, 0, 0);

    btnHome           = new QPushButton(" 홈");
    btnNewAccount     = new QPushButton(" 신규계좌 관리");
    btnMyInfo         = new QPushButton(" 내 정보");
    btnAccountProcess = new QPushButton(" 계좌 처리");

    btnHome->setIcon(QIcon(":/resources/home_btn.png"));
    btnNewAccount->setIcon(QIcon(":/resources/new_account_btn.png"));
    btnMyInfo->setIcon(QIcon(":/resources/myinfopage_btn.png"));
    btnAccountProcess->setIcon(QIcon(":/resources/accountprocesspage_btn.png"));

    QList<QPushButton*> menuButtons;
    menuButtons << btnHome << btnNewAccount << btnMyInfo << btnAccountProcess;

    for (QPushButton *btn : menuButtons) {
        btn->setMinimumHeight(46);
        btn->setIconSize(QSize(22, 22));
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet(AppStyle::SIDEBAR_BTN);
    }

    // 로그아웃 버튼
    btnLogout = new QPushButton("로그아웃");
    btnLogout->setCursor(Qt::PointingHandCursor);
    btnLogout->setMinimumHeight(40);
    btnLogout->setStyleSheet(AppStyle::SIDEBAR_LOGOUT);

    sideLayout->addWidget(logoLabel);
    sideLayout->addWidget(logoSub);
    sideLayout->addSpacing(20);
    sideLayout->addWidget(userCard);
    sideLayout->addSpacing(20);
    sideLayout->addWidget(menuSection);
    sideLayout->addSpacing(4);
    sideLayout->addWidget(btnHome);
    sideLayout->addWidget(btnNewAccount);
    sideLayout->addWidget(btnMyInfo);
    sideLayout->addWidget(btnAccountProcess);
    sideLayout->addStretch();
    sideLayout->addWidget(btnLogout);

    // ── 오른쪽 페이지 영역 ────────────────────────────────────────────────────
    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet("QStackedWidget { background-color: #F2F4F6; }");

    pageHome          = new HomePage(this);
    pageNewAccount    = new NewAccountPage(this);
    pageMyInfo        = new MyInfoPage(this);
    pageAccountProcess = new AccountProcessPage(this);

    stackedWidget->addWidget(pageHome);
    stackedWidget->addWidget(pageNewAccount);
    stackedWidget->addWidget(pageMyInfo);
    stackedWidget->addWidget(pageAccountProcess);

    mainLayout->addWidget(sideMenuWidget);
    mainLayout->addWidget(stackedWidget);

    connect(btnHome,           SIGNAL(clicked()), this, SLOT(goHome()));
    connect(btnNewAccount,     SIGNAL(clicked()), this, SLOT(goNewAccount()));
    connect(btnMyInfo,         SIGNAL(clicked()), this, SLOT(goMyInfo()));
    connect(btnAccountProcess, SIGNAL(clicked()), this, SLOT(goAccountProcess()));
    connect(btnLogout, SIGNAL(clicked()), this, SLOT(onLogoutClicked()));

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

    for (QPushButton *btn : menuButtons) {
        const bool active = (btn == selectedButton);
        btn->setStyleSheet(active ? AppStyle::SIDEBAR_BTN_ACTIVE : AppStyle::SIDEBAR_BTN);
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
