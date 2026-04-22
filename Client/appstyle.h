#pragma once
#include <QString>
#include <QColor>
#include <QWidget>
#include <QGraphicsDropShadowEffect>

/**
 * @namespace AppStyle
 * @brief 앱 전역 디자인 시스템 (Toss / KakaoBank 스타일).
 *
 * 모든 색상 토큰, 위젯 스타일시트(QSS), 그림자 헬퍼를 한 곳에서 관리한다.
 *
 * 사용 예:
 *   widget->setStyleSheet(AppStyle::CARD);
 *   AppStyle::applyCardShadow(card);
 *   qApp->setStyleSheet(AppStyle::APP);  // main.cpp에서 전역 적용
 */
namespace AppStyle {

// ── Color Tokens ───────────────────────────────────────────────────────────
/**
 * @namespace Color
 * @brief 디자인 시스템 색상 토큰 모음.
 *
 * 위젯에서 색상을 직접 하드코딩하지 않고 이 토큰을 참조해
 * 테마 변경 시 한 곳만 수정하면 되도록 한다.
 */
namespace Color {
inline const QString Primary       = "#3182F6";   ///< 브랜드 메인 블루
inline const QString PrimaryHover  = "#1B64DA";   ///< 버튼 호버 색상
inline const QString PrimaryPress  = "#1B56BC";   ///< 버튼 클릭 색상
inline const QString PrimaryLight  = "#E8F2FE";   ///< 선택 배경 / 하이라이트용 연한 블루

inline const QString Success       = "#00C896";   ///< 성공/긍정 (초록)
inline const QString Danger        = "#F04452";   ///< 위험/삭제 (빨강)
inline const QString DangerHover   = "#D93D4A";   ///< 위험 버튼 호버
inline const QString Warning       = "#FFA726";   ///< 경고 (주황)

inline const QString Bg            = "#F2F4F6";   ///< 페이지 배경색
inline const QString Surface       = "#FFFFFF";   ///< 카드·다이얼로그 표면색
inline const QString SurfaceAlt    = "#F9FAFB";   ///< 입력 필드 배경
inline const QString Border        = "#E5E8EB";   ///< 테두리·구분선
inline const QString Divider       = "#F2F4F6";   ///< 구분선 (배경과 동색)

inline const QString TextPrimary   = "#191F28";   ///< 본문 텍스트
inline const QString TextSecondary = "#4E5968";   ///< 보조 텍스트
inline const QString TextMuted     = "#8B95A1";   ///< 설명·힌트 텍스트
inline const QString TextDisabled  = "#B0B8C1";   ///< 비활성 텍스트

// 사이드바 전용 어두운 톤
inline const QString SidebarBg     = "#0B1220";   ///< 사이드바 배경
inline const QString SidebarHover  = "#1E293B";   ///< 사이드바 항목 호버
inline const QString SidebarText   = "#CBD5E1";   ///< 사이드바 메뉴 텍스트
inline const QString SidebarMuted  = "#64748B";   ///< 사이드바 보조 텍스트
} // namespace Color

// ── Typography ─────────────────────────────────────────────────────────────
/// 크로스플랫폼 한글 폰트 스택 (Mac → Windows → fallback 순서)
inline const QString FontStack =
    "'Apple SD Gothic Neo', 'Malgun Gothic', 'Segoe UI', -apple-system, sans-serif";

// ── Shadow Helpers ─────────────────────────────────────────────────────────

/**
 * @brief 위젯에 카드 그림자(elevation) 효과를 적용한다.
 * @param w     그림자를 적용할 위젯
 * @param blur  블러 반경 (px, 기본 24)
 * @param dy    수직 오프셋 (px, 기본 4)
 * @param alpha 그림자 불투명도 0-255 (기본 24)
 *
 * Qt QSS는 box-shadow를 지원하지 않으므로
 * QGraphicsDropShadowEffect로 대체한다.
 */
inline void applyCardShadow(QWidget *w, int blur = 24, int dy = 4, int alpha = 24)
{
    auto *eff = new QGraphicsDropShadowEffect(w);
    eff->setBlurRadius(blur);
    eff->setOffset(0, dy);
    eff->setColor(QColor(17, 24, 39, alpha));
    w->setGraphicsEffect(eff);
}

/**
 * @brief 히어로 카드(잔액 표시 등 주요 배너)에 강조 그림자를 적용한다.
 *
 * applyCardShadow보다 블러와 오프셋이 크다.
 */
inline void applyHeroShadow(QWidget *w)
{
    applyCardShadow(w, 40, 10, 30);
}

// ── Global App Stylesheet ──────────────────────────────────────────────────
/// 전체 위젯에 기본 폰트·툴팁·스크롤바 스타일을 적용하는 QSS.
/// main.cpp에서 qApp->setStyleSheet(AppStyle::APP)로 한 번만 호출한다.
inline const QString APP =
    "* {"
    "  font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', 'Segoe UI', sans-serif;"
    "  font-size: 14px;"
    "}"
    "QScrollBar:vertical {"
    "  background: transparent; width: 10px; margin: 2px;"
    "}"
    "QScrollBar::handle:vertical {"
    "  background: #D1D6DB; border-radius: 4px; min-height: 30px;"
    "}"
    "QScrollBar::handle:vertical:hover { background: #8B95A1; }"
    "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }"
    "QScrollBar:horizontal {"
    "  background: transparent; height: 10px; margin: 2px;"
    "}"
    "QScrollBar::handle:horizontal {"
    "  background: #D1D6DB; border-radius: 4px; min-width: 30px;"
    "}"
    "QScrollBar::handle:horizontal:hover { background: #8B95A1; }"
    "QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { width: 0; }"
    "QToolTip {"
    "  background: #191F28; color: white; border: none;"
    "  padding: 6px 10px; border-radius: 6px;"
    "}";

// ── Page / Layout ──────────────────────────────────────────────────────────
inline const QString PAGE_BG =
    "background-color: #F2F4F6;";   ///< 일반 페이지 배경

inline const QString PAGE_BG_LABEL =
    "background-color: #F2F4F6;"
    "QLabel { color: #191F28; background: transparent; border: none; }";  ///< 라벨 포함 페이지 배경

// ── Cards ──────────────────────────────────────────────────────────────────
inline const QString CARD =
    "QFrame {"
    "  background-color: white;"
    "  border: 1px solid #E5E8EB;"
    "  border-radius: 16px;"
    "}";  ///< 기본 카드 (라운드 흰 배경)

inline const QString CARD_LARGE =
    "QFrame {"
    "  background-color: white;"
    "  border: 1px solid #E5E8EB;"
    "  border-radius: 20px;"
    "}";  ///< 큰 카드 (계좌처리 페이지 좌·우 패널용)

/// 히어로 카드 (잔액 표시용 — 블루 그라디언트)
inline const QString HERO_CARD =
    "QFrame {"
    "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
    "    stop:0 #3182F6, stop:1 #1B64DA);"
    "  border-radius: 20px;"
    "  border: none;"
    "}"
    "QLabel { color: white; background: transparent; border: none; }";

// ── Inputs ─────────────────────────────────────────────────────────────────
inline const QString INPUT =
    "QLineEdit {"
    "  background-color: #F9FAFB; border: 1.5px solid #E5E8EB;"
    "  border-radius: 10px; color: #191F28; font-size: 14px; padding: 0 14px;"
    "  selection-background-color: #3182F6; selection-color: white;"
    "}"
    "QLineEdit:focus { border-color: #3182F6; background-color: white; }"
    "QLineEdit:disabled { background-color: #F2F4F6; color: #8B95A1; }";  ///< 단일 QLineEdit 스타일

inline const QString COMBO =
    "QComboBox {"
    "  background-color: #F9FAFB; border: 1.5px solid #E5E8EB;"
    "  border-radius: 10px; color: #191F28; font-size: 14px; padding: 0 14px;"
    "}"
    "QComboBox:focus, QComboBox:on { border-color: #3182F6; background-color: white; }"
    "QComboBox::drop-down {"
    "  border: none; width: 32px; subcontrol-origin: padding;"
    "  subcontrol-position: right center;"
    "}"
    "QComboBox::down-arrow {"
    "  image: none; width: 0; height: 0;"
    "  border-left: 5px solid transparent;"
    "  border-right: 5px solid transparent;"
    "  border-top: 6px solid #4E5968;"
    "  margin-right: 12px;"
    "}"
    "QComboBox QAbstractItemView {"
    "  background-color: white; color: #191F28;"
    "  border: 1px solid #E5E8EB; border-radius: 8px;"
    "  selection-background-color: #E8F2FE; selection-color: #191F28;"
    "  outline: none; padding: 4px;"
    "}"
    "QComboBox QAbstractItemView::item { padding: 8px 10px; border-radius: 6px; }";  ///< QComboBox 스타일

/// QLineEdit + QDoubleSpinBox + QComboBox를 한 번에 통일하는 스타일 (신규계좌 페이지용)
inline const QString INPUT_ALL =
    "QLineEdit, QDoubleSpinBox, QComboBox {"
    "  background-color: #F9FAFB; border: 1.5px solid #E5E8EB;"
    "  border-radius: 10px; color: #191F28; font-size: 14px; padding: 0 14px;"
    "}"
    "QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus, QComboBox:on {"
    "  border-color: #3182F6; background-color: white;"
    "}"
    "QComboBox::drop-down { border: none; width: 32px; }"
    "QComboBox::down-arrow {"
    "  image: none; width: 0; height: 0;"
    "  border-left: 5px solid transparent;"
    "  border-right: 5px solid transparent;"
    "  border-top: 6px solid #4E5968;"
    "  margin-right: 12px;"
    "}"
    "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width: 0; border: none; }";

/// 읽기 전용 표시 필드 (이자율·마이너스 한도처럼 고정값 표시용)
inline const QString FIXED_VAL =
    "color: #4E5968; font-size: 14px; border: 1.5px solid #E5E8EB;"
    "border-radius: 10px; background-color: #F2F4F6; padding: 0 14px;";

// ── Buttons ────────────────────────────────────────────────────────────────
inline const QString BTN_PRIMARY =
    "QPushButton {"
    "  background-color: #3182F6; color: white; border: none;"
    "  border-radius: 12px; font-size: 15px; font-weight: 700;"
    "  padding: 0 20px;"
    "}"
    "QPushButton:hover    { background-color: #1B64DA; }"
    "QPushButton:pressed  { background-color: #1B56BC; }"
    "QPushButton:disabled { background-color: #B0C8F0; }";  ///< 기본 파란색 버튼

inline const QString BTN_BLUE = BTN_PRIMARY;  ///< BTN_PRIMARY 의 호환용 별칭

inline const QString BTN_DANGER =
    "QPushButton {"
    "  background-color: #F04452; color: white; border: none;"
    "  border-radius: 12px; font-size: 15px; font-weight: 700;"
    "  padding: 0 20px;"
    "}"
    "QPushButton:hover    { background-color: #D93D4A; }"
    "QPushButton:pressed  { background-color: #BF3542; }"
    "QPushButton:disabled { background-color: #F7A5AB; }";  ///< 위험/삭제용 빨간 버튼

inline const QString BTN_RED = BTN_DANGER;  ///< BTN_DANGER 의 호환용 별칭

inline const QString BTN_SECONDARY =
    "QPushButton {"
    "  background-color: #F2F4F6; color: #4E5968; border: 1px solid #E5E8EB;"
    "  border-radius: 12px; font-size: 15px; font-weight: 600;"
    "  padding: 0 20px;"
    "}"
    "QPushButton:hover  { background-color: #E5E8EB; color: #191F28; }"
    "QPushButton:pressed { background-color: #D1D6DB; }";  ///< 보조 회색 버튼 (취소 등)

inline const QString BTN_GRAY = BTN_SECONDARY;  ///< BTN_SECONDARY 의 호환용 별칭

inline const QString BTN_GHOST =
    "QPushButton {"
    "  background-color: transparent; color: #3182F6; border: none;"
    "  font-size: 14px; font-weight: 600;"
    "}"
    "QPushButton:hover { color: #1B64DA; text-decoration: underline; }";  ///< 텍스트형 고스트 버튼

// ── Labels ─────────────────────────────────────────────────────────────────
inline const QString LABEL_DISPLAY  = "font-size: 28px; font-weight: 800; color: #191F28; border: none;";   ///< 대형 숫자 표시용
inline const QString LABEL_TITLE    = "font-size: 22px; font-weight: 700; color: #191F28; border: none;";   ///< 페이지·카드 제목
inline const QString LABEL_SUBTITLE = "font-size: 16px; font-weight: 600; color: #4E5968; border: none;";   ///< 섹션 부제목
inline const QString LABEL_FIELD    = "color: #4E5968; font-size: 13px; font-weight: 600; border: none; min-width: 110px;";  ///< 폼 필드 라벨
inline const QString LABEL_MUTED    = "font-size: 13px; font-weight: 500; color: #8B95A1; border: none;";   ///< 흐린 설명 텍스트
inline const QString LABEL_CAPTION  = "font-size: 12px; color: #8B95A1; border: none;";                     ///< 캡션/보조 텍스트

// ── Message Box ────────────────────────────────────────────────────────────
/// QMessageBox 전용 스타일 (MyInfoPage::showMsg에서 사용)
inline const QString MSGBOX =
    "QMessageBox { background-color: white; }"
    "QMessageBox QLabel {"
    "  color: #191F28; font-size: 14px; background-color: transparent;"
    "  min-width: 240px;"
    "}"
    "QMessageBox QPushButton {"
    "  background-color: #3182F6; color: white; border: none;"
    "  border-radius: 8px; padding: 8px 22px; font-size: 13px; font-weight: 700;"
    "  min-width: 72px;"
    "}"
    "QMessageBox QPushButton:hover { background-color: #1B64DA; }"
    "QMessageBox QPushButton:pressed { background-color: #1B56BC; }";

// ── Misc ───────────────────────────────────────────────────────────────────
/// QRadioButton 스타일 (입출금 선택 등)
inline const QString RADIO =
    "QRadioButton {"
    "  color: #4E5968; background: transparent; font-size: 14px; spacing: 8px;"
    "}"
    "QRadioButton::indicator {"
    "  width: 18px; height: 18px; border-radius: 9px;"
    "  border: 2px solid #D1D6DB; background: white;"
    "}"
    "QRadioButton::indicator:checked {"
    "  border: 6px solid #3182F6; background: white;"
    "}"
    "QRadioButton:disabled { color: #B0B8C1; }"
    "QRadioButton::indicator:disabled { border-color: #E5E8EB; }";

/// QListWidget 스타일 (계좌 검색 결과 드롭다운용)
inline const QString LIST =
    "QListWidget {"
    "  background-color: white; border: 1px solid #E5E8EB;"
    "  border-radius: 10px; font-size: 14px; color: #191F28;"
    "  outline: none; padding: 4px;"
    "}"
    "QListWidget::item { padding: 10px 12px; border-radius: 6px; }"
    "QListWidget::item:hover { background-color: #F2F4F6; }"
    "QListWidget::item:selected { background-color: #E8F2FE; color: #191F28; }";

/// 투명 배경 QScrollArea (스크롤 트랙이 보이지 않아야 하는 영역용)
inline const QString SCROLL_TRANSPARENT =
    "QScrollArea { background: transparent; border: none; }";

// ── Sidebar (MainWindow) ───────────────────────────────────────────────────
inline const QString SIDEBAR_CONTAINER =
    "QWidget#sidebar {"
    "  background-color: #0B1220;"
    "  border-right: 1px solid #1E293B;"
    "}";  ///< 사이드바 컨테이너 (objectName 셀렉터)

inline const QString SIDEBAR_LOGO =
    "color: white; font-size: 22px; font-weight: 800;"
    "letter-spacing: -0.5px; border: none;";  ///< 로고 텍스트

inline const QString SIDEBAR_LOGO_SUB =
    "color: #64748B; font-size: 11px; font-weight: 600;"
    "letter-spacing: 1px; border: none;";  ///< 로고 서브타이틀 ("PERSONAL BANKING")

inline const QString SIDEBAR_USER_CARD =
    "QWidget#userCard {"
    "  background-color: #111B2E; border: 1px solid #1E293B;"
    "  border-radius: 12px;"
    "}"
    "QLabel { background: transparent; border: none; }";  ///< 사용자 카드 위젯

inline const QString SIDEBAR_USER_HELLO =
    "color: #8B95A1; font-size: 11px; font-weight: 600;"
    "letter-spacing: 1px;";  ///< "WELCOME BACK" 텍스트

inline const QString SIDEBAR_USER_NAME =
    "color: white; font-size: 15px; font-weight: 700;";  ///< 사용자 이름 텍스트

inline const QString SIDEBAR_SECTION =
    "color: #64748B; font-size: 12px; font-weight: 700;"
    "letter-spacing: 2px; border: none;";  ///< "MENU" 섹션 라벨

inline const QString SIDEBAR_BTN =
    "QPushButton {"
    "  background-color: transparent; color: #CBD5E1;"
    "  text-align: left; padding: 12px 16px 12px 18px; border: none;"
    "  border-radius: 10px; font-size: 14px; font-weight: 600;"
    "}"
    "QPushButton:hover { background-color: #1E293B; color: white; }";  ///< 비활성 메뉴 버튼

inline const QString SIDEBAR_BTN_ACTIVE =
    "QPushButton {"
    "  background-color: #1B3358; color: #F1F5F9;"
    "  text-align: left; padding: 12px 16px 12px 15px;"
    "  border-left: 3px solid #60A5FA;"
    "  border-top: none; border-right: none; border-bottom: none;"
    "  border-radius: 10px; font-size: 14px; font-weight: 700;"
    "}";  ///< 활성(선택된) 메뉴 버튼 — 왼쪽 강조 바 포함

inline const QString SIDEBAR_LOGOUT =
    "QPushButton {"
    "  background-color: transparent; color: #94A3B8;"
    "  border: 1px solid #1E293B;"
    "  border-radius: 10px; font-size: 13px; font-weight: 600;"
    "  padding: 10px 16px;"
    "}"
    "QPushButton:hover { background-color: #1E293B; color: #F04452; border-color: #F04452; }";  ///< 로그아웃 버튼

// ── Dark Dialog Theme (기존 코드 호환용) ─────────────────────────────────
inline const QString DARK_DIALOG_BG   = "QDialog { background-color: #F2F4F6; }";  ///< 다이얼로그 배경
inline const QString DARK_CARD_BODY   = "background-color: white; border-radius: 20px; border: 1px solid #E5E8EB;";  ///< 다이얼로그 내부 카드
inline const QString DARK_FIELD_LABEL =
    "QLabel#fieldLabel {"
    "  color: #4E5968; font-size: 13px; font-weight: 600;"
    "}";  ///< 다이얼로그 필드 라벨
inline const QString DARK_INPUT =
    "QLineEdit#inputField {"
    "  background-color: #F9FAFB; border: 1.5px solid #E5E8EB;"
    "  border-radius: 10px; color: #191F28; font-size: 14px; padding: 0 14px;"
    "}"
    "QLineEdit#inputField:focus { border: 1.5px solid #3182F6; background-color: white; }";  ///< 다이얼로그 입력 필드

} // namespace AppStyle
