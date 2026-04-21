#pragma once
#include <QString>
#include <QColor>
#include <QWidget>
#include <QGraphicsDropShadowEffect>

// ─────────────────────────────────────────────────────────────────────────────
//  디자인 시스템 (Toss / KakaoBank 스타일)
//
//  사용법:
//    widget->setStyleSheet(AppStyle::CARD);
//    AppStyle::applyCardShadow(card);           // 카드에 부드러운 그림자
//    qApp->setStyleSheet(AppStyle::APP);        // main.cpp에서 전역 기본값
// ─────────────────────────────────────────────────────────────────────────────

namespace AppStyle {

// ── Color Tokens ───────────────────────────────────────────────────────────
namespace Color {
inline const QString Primary       = "#3182F6";   // 브랜드 블루
inline const QString PrimaryHover  = "#1B64DA";
inline const QString PrimaryPress  = "#1B56BC";
inline const QString PrimaryLight  = "#E8F2FE";   // 배경/하이라이트용

inline const QString Success       = "#00C896";
inline const QString Danger        = "#F04452";
inline const QString DangerHover   = "#D93D4A";
inline const QString Warning       = "#FFA726";

inline const QString Bg            = "#F2F4F6";   // 페이지 배경
inline const QString Surface       = "#FFFFFF";   // 카드/표면
inline const QString SurfaceAlt    = "#F9FAFB";   // 입력 필드
inline const QString Border        = "#E5E8EB";
inline const QString Divider       = "#F2F4F6";

inline const QString TextPrimary   = "#191F28";
inline const QString TextSecondary = "#4E5968";
inline const QString TextMuted     = "#8B95A1";
inline const QString TextDisabled  = "#B0B8C1";

// Sidebar (어두운 톤 유지)
inline const QString SidebarBg     = "#0B1220";
inline const QString SidebarHover  = "#1E293B";
inline const QString SidebarText   = "#CBD5E1";
inline const QString SidebarMuted  = "#64748B";
} // namespace Color

// ── Typography ─────────────────────────────────────────────────────────────
// 크로스플랫폼 한글 폰트 스택 (Mac: Apple SD, Windows: Malgun, fallback: system)
inline const QString FontStack =
    "'Apple SD Gothic Neo', 'Malgun Gothic', 'Segoe UI', -apple-system, sans-serif";

// ── Shadow Helper ──────────────────────────────────────────────────────────
// 카드에 부드러운 elevation 효과. QSS의 box-shadow 미지원 대응.
inline void applyCardShadow(QWidget *w, int blur = 24, int dy = 4, int alpha = 24)
{
    auto *eff = new QGraphicsDropShadowEffect(w);
    eff->setBlurRadius(blur);
    eff->setOffset(0, dy);
    eff->setColor(QColor(17, 24, 39, alpha));
    w->setGraphicsEffect(eff);
}

inline void applyHeroShadow(QWidget *w)
{
    applyCardShadow(w, 40, 10, 30);
}

// ── Global App Stylesheet (main.cpp에서 qApp->setStyleSheet로 사용) ───────
// 전체 위젯에 기본 폰트/툴팁/스크롤바 스타일 적용
inline const QString APP =
    // 기본 폰트 — 모든 위젯에 상속
    "* {"
    "  font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', 'Segoe UI', sans-serif;"
    "  font-size: 14px;"
    "}"
    // 스크롤바 (얇고 심플하게)
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
    // 툴팁
    "QToolTip {"
    "  background: #191F28; color: white; border: none;"
    "  padding: 6px 10px; border-radius: 6px;"
    "}";

// ── Page / Layout ──────────────────────────────────────────────────────────
inline const QString PAGE_BG =
    "background-color: #F2F4F6;";

inline const QString PAGE_BG_LABEL =
    "background-color: #F2F4F6;"
    "QLabel { color: #191F28; background: transparent; border: none; }";

// ── Cards ──────────────────────────────────────────────────────────────────
inline const QString CARD =
    "QFrame {"
    "  background-color: white;"
    "  border: 1px solid #E5E8EB;"
    "  border-radius: 16px;"
    "}";

inline const QString CARD_LARGE =
    "QFrame {"
    "  background-color: white;"
    "  border: 1px solid #E5E8EB;"
    "  border-radius: 20px;"
    "}";

// 히어로 카드 (잔액 표시용 — 그라디언트 블루)
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
    "QLineEdit:disabled { background-color: #F2F4F6; color: #8B95A1; }";

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
    "QComboBox QAbstractItemView::item { padding: 8px 10px; border-radius: 6px; }";

// QLineEdit + QDoubleSpinBox + QComboBox 한번에
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

// 읽기 전용 표시 필드
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
    "QPushButton:disabled { background-color: #B0C8F0; }";

// 기존 호환용 (예전 코드가 BTN_BLUE 사용)
inline const QString BTN_BLUE = BTN_PRIMARY;

inline const QString BTN_DANGER =
    "QPushButton {"
    "  background-color: #F04452; color: white; border: none;"
    "  border-radius: 12px; font-size: 15px; font-weight: 700;"
    "  padding: 0 20px;"
    "}"
    "QPushButton:hover    { background-color: #D93D4A; }"
    "QPushButton:pressed  { background-color: #BF3542; }"
    "QPushButton:disabled { background-color: #F7A5AB; }";

inline const QString BTN_RED = BTN_DANGER;

inline const QString BTN_SECONDARY =
    "QPushButton {"
    "  background-color: #F2F4F6; color: #4E5968; border: 1px solid #E5E8EB;"
    "  border-radius: 12px; font-size: 15px; font-weight: 600;"
    "  padding: 0 20px;"
    "}"
    "QPushButton:hover  { background-color: #E5E8EB; color: #191F28; }"
    "QPushButton:pressed { background-color: #D1D6DB; }";

inline const QString BTN_GRAY = BTN_SECONDARY;

inline const QString BTN_GHOST =
    "QPushButton {"
    "  background-color: transparent; color: #3182F6; border: none;"
    "  font-size: 14px; font-weight: 600;"
    "}"
    "QPushButton:hover { color: #1B64DA; text-decoration: underline; }";

// ── Labels ─────────────────────────────────────────────────────────────────
inline const QString LABEL_DISPLAY =
    "font-size: 28px; font-weight: 800; color: #191F28; border: none;";

inline const QString LABEL_TITLE =
    "font-size: 22px; font-weight: 700; color: #191F28; border: none;";

inline const QString LABEL_SUBTITLE =
    "font-size: 16px; font-weight: 600; color: #4E5968; border: none;";

inline const QString LABEL_FIELD =
    "color: #4E5968; font-size: 13px; font-weight: 600; border: none; min-width: 110px;";

inline const QString LABEL_MUTED =
    "font-size: 13px; font-weight: 500; color: #8B95A1; border: none;";

inline const QString LABEL_CAPTION =
    "font-size: 12px; color: #8B95A1; border: none;";

// ── Message Box ────────────────────────────────────────────────────────────
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

inline const QString LIST =
    "QListWidget {"
    "  background-color: white; border: 1px solid #E5E8EB;"
    "  border-radius: 10px; font-size: 14px; color: #191F28;"
    "  outline: none; padding: 4px;"
    "}"
    "QListWidget::item { padding: 10px 12px; border-radius: 6px; }"
    "QListWidget::item:hover { background-color: #F2F4F6; }"
    "QListWidget::item:selected { background-color: #E8F2FE; color: #191F28; }";

inline const QString SCROLL_TRANSPARENT =
    "QScrollArea { background: transparent; border: none; }";

// ── Sidebar (MainWindow) ───────────────────────────────────────────────────
inline const QString SIDEBAR_CONTAINER =
    "QWidget#sidebar {"
    "  background-color: #0B1220;"
    "  border-right: 1px solid #1E293B;"
    "}";

inline const QString SIDEBAR_LOGO =
    "color: white; font-size: 22px; font-weight: 800;"
    "letter-spacing: -0.5px; border: none;";

inline const QString SIDEBAR_LOGO_SUB =
    "color: #64748B; font-size: 11px; font-weight: 600;"
    "letter-spacing: 1px; border: none;";

inline const QString SIDEBAR_USER_CARD =
    "QWidget#userCard {"
    "  background-color: #111B2E; border: 1px solid #1E293B;"
    "  border-radius: 12px;"
    "}"
    "QLabel { background: transparent; border: none; }";

inline const QString SIDEBAR_USER_HELLO =
    "color: #8B95A1; font-size: 11px; font-weight: 600;"
    "letter-spacing: 1px;";

inline const QString SIDEBAR_USER_NAME =
    "color: white; font-size: 15px; font-weight: 700;";

inline const QString SIDEBAR_SECTION =
    "color: #64748B; font-size: 12px; font-weight: 700;"
    "letter-spacing: 2px; border: none;";

inline const QString SIDEBAR_BTN =
    "QPushButton {"
    "  background-color: transparent; color: #CBD5E1;"
    "  text-align: left; padding: 12px 16px 12px 18px; border: none;"
    "  border-radius: 10px; font-size: 14px; font-weight: 600;"
    "}"
    "QPushButton:hover { background-color: #1E293B; color: white; }";

inline const QString SIDEBAR_BTN_ACTIVE =
    "QPushButton {"
    "  background-color: #1B3358; color: #F1F5F9;"
    "  text-align: left; padding: 12px 16px 12px 15px;"
    "  border-left: 3px solid #60A5FA;"
    "  border-top: none; border-right: none; border-bottom: none;"
    "  border-radius: 10px; font-size: 14px; font-weight: 700;"
    "}";

inline const QString SIDEBAR_LOGOUT =
    "QPushButton {"
    "  background-color: transparent; color: #94A3B8;"
    "  border: 1px solid #1E293B;"
    "  border-radius: 10px; font-size: 13px; font-weight: 600;"
    "  padding: 10px 16px;"
    "}"
    "QPushButton:hover { background-color: #1E293B; color: #F04452; border-color: #F04452; }";

// ── Dark Dialog Theme (기존 코드 호환용) ─────────────────────────────────
inline const QString DARK_DIALOG_BG =
    "QDialog { background-color: #F2F4F6; }";

inline const QString DARK_CARD_BODY =
    "background-color: white; border-radius: 20px; border: 1px solid #E5E8EB;";

inline const QString DARK_FIELD_LABEL =
    "QLabel#fieldLabel {"
    "  color: #4E5968; font-size: 13px; font-weight: 600;"
    "}";

inline const QString DARK_INPUT =
    "QLineEdit#inputField {"
    "  background-color: #F9FAFB; border: 1.5px solid #E5E8EB;"
    "  border-radius: 10px; color: #191F28; font-size: 14px; padding: 0 14px;"
    "}"
    "QLineEdit#inputField:focus { border: 1.5px solid #3182F6; background-color: white; }";

} // namespace AppStyle
