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

/**
 * 생성자에서 레이아웃 전체를 구성한다.
 * - 왼쪽: 사이드바 (로고, 사용자 카드, 메뉴 버튼, 로그아웃 버튼)
 * - 오른쪽: QStackedWidget (홈/신규계좌/내정보/계좌처리 페이지)
 */
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

    // 로고 텍스트 + 서브타이틀
    QLabel *logoLabel = new QLabel("Easy Bank");
    logoLabel->setStyleSheet(AppStyle::SIDEBAR_LOGO);
    logoLabel->setContentsMargins(8, 0, 0, 0);

    QLabel *logoSub = new QLabel("PERSONAL BANKING");
    logoSub->setStyleSheet(AppStyle::SIDEBAR_LOGO_SUB);
    logoSub->setContentsMargins(8, 0, 0, 0);

    // 사용자 환영 카드 (로그인한 사용자 이름 표시)
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

    // 메뉴 섹션 라벨
    QLabel *menuSection = new QLabel("MENU");
    menuSection->setStyleSheet(AppStyle::SIDEBAR_SECTION);
    menuSection->setContentsMargins(8, 0, 0, 0);

    // 메뉴 버튼 생성 + 아이콘 설정
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

    // 로그아웃 버튼 (사이드바 하단)
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
    sideLayout->addWidget(btnAccountProcess);
    sideLayout->addWidget(btnMyInfo);
    sideLayout->addStretch();
    sideLayout->addWidget(btnLogout);

    // ── 오른쪽 페이지 영역 ────────────────────────────────────────────────────
    stackedWidget = new QStackedWidget;
    stackedWidget->setStyleSheet("QStackedWidget { background-color: #F2F4F6; }");

    pageHome           = new HomePage(this);
    pageNewAccount     = new NewAccountPage(this);
    pageMyInfo         = new MyInfoPage(this);
    pageAccountProcess = new AccountProcessPage(this);

    stackedWidget->addWidget(pageHome);
    stackedWidget->addWidget(pageNewAccount);
    stackedWidget->addWidget(pageMyInfo);
    stackedWidget->addWidget(pageAccountProcess);

    mainLayout->addWidget(sideMenuWidget);
    mainLayout->addWidget(stackedWidget);

    // 메뉴 버튼 → 페이지 전환 슬롯 연결
    connect(btnHome,           SIGNAL(clicked()), this, SLOT(goHome()));
    connect(btnNewAccount,     SIGNAL(clicked()), this, SLOT(goNewAccount()));
    connect(btnMyInfo,         SIGNAL(clicked()), this, SLOT(goMyInfo()));
    connect(btnAccountProcess, SIGNAL(clicked()), this, SLOT(goAccountProcess()));
    connect(btnLogout, SIGNAL(clicked()), this, SLOT(onLogoutClicked()));

    // 초기 페이지: 홈
    stackedWidget->setCurrentWidget(pageHome);
    updateMenuStyle(btnHome);
}

MainWindow::~MainWindow()
{
}

/**
 * 메뉴 버튼 목록을 순회하며 selectedButton만 SIDEBAR_BTN_ACTIVE 스타일을 적용하고
 * 나머지는 SIDEBAR_BTN 기본 스타일로 되돌린다.
 */
void MainWindow::updateMenuStyle(QPushButton *selectedButton)
{
    QList<QPushButton*> menuButtons;
    menuButtons << btnHome << btnNewAccount << btnMyInfo << btnAccountProcess;

    for (QPushButton *btn : menuButtons) {
        const bool active = (btn == selectedButton);
        btn->setStyleSheet(active ? AppStyle::SIDEBAR_BTN_ACTIVE : AppStyle::SIDEBAR_BTN);
    }
}

/** HomePage로 전환하고 홈 버튼을 활성 스타일로 강조한다. */
void MainWindow::goHome()
{
    stackedWidget->setCurrentWidget(pageHome);
    updateMenuStyle(btnHome);
}

/** NewAccountPage로 전환하고 신규계좌 버튼을 활성 스타일로 강조한다. */
void MainWindow::goNewAccount()
{
    stackedWidget->setCurrentWidget(pageNewAccount);
    updateMenuStyle(btnNewAccount);
}

/** MyInfoPage의 사용자 정보를 새로 로드한 뒤 페이지로 전환한다. */
void MainWindow::goMyInfo()
{
    pageMyInfo->loadUserInfo();
    stackedWidget->setCurrentWidget(pageMyInfo);
    updateMenuStyle(btnMyInfo);
}

/** AccountProcessPage로 전환하고 계좌처리 버튼을 활성 스타일로 강조한다. */
void MainWindow::goAccountProcess()
{
    stackedWidget->setCurrentWidget(pageAccountProcess);
    updateMenuStyle(btnAccountProcess);
}

/**
 * 서버에 "logout" 요청을 전송하고, SessionContext를 초기화한 뒤
 * 새 LoginDialog를 표시하고 현재 MainWindow를 닫는다.
 */
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
