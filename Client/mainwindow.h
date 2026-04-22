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

/**
 * @class MainWindow
 * @brief 로그인 후 표시되는 메인 창.
 *
 * 왼쪽에 고정 너비(240px)의 사이드바 메뉴, 오른쪽에 QStackedWidget으로
 * 홈·신규계좌·내 정보·계좌처리 페이지를 전환하여 표시한다.
 *
 * 사이드바 버튼 클릭 → 해당 페이지로 전환 + 버튼 강조 스타일 업데이트.
 * 로그아웃 버튼 클릭 → 서버에 "logout" 요청 → 세션 초기화 → LoginDialog 재표시.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    /** 홈 버튼 클릭 시 HomePage로 전환하고 버튼 스타일을 갱신한다. */
    void goHome();

    /** 신규계좌 버튼 클릭 시 NewAccountPage로 전환하고 버튼 스타일을 갱신한다. */
    void goNewAccount();

    /** 내 정보 버튼 클릭 시 사용자 정보를 새로 로드하고 MyInfoPage로 전환한다. */
    void goMyInfo();

    /** 계좌 처리 버튼 클릭 시 AccountProcessPage로 전환하고 버튼 스타일을 갱신한다. */
    void goAccountProcess();

    /**
     * @brief 로그아웃 버튼 클릭 시 서버에 "logout" 요청을 전송하고 로그인 화면으로 돌아간다.
     *
     * SessionContext를 초기화한 뒤 새 LoginDialog를 열고 현재 MainWindow를 닫는다.
     */
    void onLogoutClicked();

private:
    /**
     * @brief 선택된 버튼에만 활성 스타일을 적용하고 나머지는 기본 스타일로 되돌린다.
     * @param selectedButton 현재 선택된 메뉴 버튼 포인터
     */
    void updateMenuStyle(QPushButton *selectedButton);

    QStackedWidget *stackedWidget;          ///< 오른쪽 페이지 영역 (페이지 전환 컨테이너)

    HomePage           *pageHome;           ///< 홈 페이지 (총 자산·파이차트)
    NewAccountPage     *pageNewAccount;     ///< 신규 계좌 개설 페이지
    MyInfoPage         *pageMyInfo;         ///< 내 정보·계좌 목록 페이지
    AccountProcessPage *pageAccountProcess; ///< 계좌 조회·입출금 처리 페이지

    QPushButton *btnHome;           ///< 홈 메뉴 버튼
    QPushButton *btnNewAccount;     ///< 신규계좌 메뉴 버튼
    QPushButton *btnMyInfo;         ///< 내 정보 메뉴 버튼
    QPushButton *btnAccountProcess; ///< 계좌 처리 메뉴 버튼
    QPushButton *btnLogout;         ///< 로그아웃 버튼 (사이드바 하단)
};

#endif // MAINWINDOW_H
