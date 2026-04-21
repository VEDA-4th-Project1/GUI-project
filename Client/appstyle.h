#pragma once
#include <QString>

namespace AppStyle {

// ── Page ───────────────────────────────────────────────────────────────────
inline const QString PAGE_BG =
    "background-color: #F5F7FB;";

inline const QString PAGE_BG_LABEL =
    "background-color: #F5F7FB;"
    "QLabel { color: #111827; background: transparent; border: none; }";

// ── Cards ──────────────────────────────────────────────────────────────────
inline const QString CARD =
    "QFrame { background-color: white; border: 1px solid #E5E7EB; border-radius: 16px; }";

inline const QString CARD_LARGE =
    "QFrame { background-color: white; border: 1px solid #E5E7EB; border-radius: 18px; }";

// ── Inputs ─────────────────────────────────────────────────────────────────
inline const QString INPUT =
    "QLineEdit {"
    "  background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
    "  border-radius: 8px; color: #111827; font-size: 13px; padding: 0 12px;"
    "}"
    "QLineEdit:focus { border-color: #2563EB; background-color: white; }";

inline const QString COMBO =
    "QComboBox {"
    "  background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
    "  border-radius: 8px; color: #111827; font-size: 13px; padding: 0 12px;"
    "}"
    "QComboBox::drop-down { border: none; width: 28px; }"
    "QComboBox QAbstractItemView { background-color: white; color: #111827; }";

// QLineEdit + QDoubleSpinBox + QComboBox 한번에
inline const QString INPUT_ALL =
    "QLineEdit, QDoubleSpinBox, QComboBox {"
    "  background-color: #F9FAFB; border: 1.5px solid #D1D5DB;"
    "  border-radius: 8px; color: #111827; font-size: 13px; padding: 0 12px;"
    "}"
    "QLineEdit:focus, QDoubleSpinBox:focus, QComboBox:focus {"
    "  border-color: #2563EB; background-color: white;"
    "}"
    "QComboBox::drop-down { border: none; width: 28px; }";

// 읽기 전용 표시 필드 (이자율, 한도 등)
inline const QString FIXED_VAL =
    "color: #6B7280; font-size: 13px; border: 1.5px solid #E5E7EB;"
    "border-radius: 8px; background-color: #F3F4F6; padding: 0 12px;";

// ── Buttons ────────────────────────────────────────────────────────────────
inline const QString BTN_BLUE =
    "QPushButton { background-color: #2563EB; color: white; border: none;"
    "  border-radius: 8px; font-size: 13px; font-weight: bold; }"
    "QPushButton:hover    { background-color: #1D4ED8; }"
    "QPushButton:pressed  { background-color: #1E40AF; }"
    "QPushButton:disabled { background-color: #93C5FD; }";

inline const QString BTN_RED =
    "QPushButton { background-color: #EF4444; color: white; border: none;"
    "  border-radius: 8px; font-size: 13px; font-weight: bold; }"
    "QPushButton:hover { background-color: #DC2626; }";

inline const QString BTN_GRAY =
    "QPushButton { background-color: #6B7280; color: white; border: none;"
    "  border-radius: 8px; font-size: 13px; font-weight: bold; }"
    "QPushButton:hover { background-color: #4B5563; }";

// ── Labels ─────────────────────────────────────────────────────────────────
// 카드 타이틀 (내 정보, 신규 계좌 개설 등)
inline const QString LABEL_TITLE =
    "font-size: 20px; font-weight: 700; color: #111827; border: none;";

// 입력 필드 앞 레이블 (이름, 금액 등)
inline const QString LABEL_FIELD =
    "color: #374151; font-size: 13px; font-weight: 600; border: none; min-width: 120px;";

// 보조 안내 텍스트
inline const QString LABEL_MUTED =
    "font-size: 12px; font-weight: 600; color: #6B7280;";

// ── Message Box ────────────────────────────────────────────────────────────
inline const QString MSGBOX =
    "QMessageBox { background-color: white; }"
    "QMessageBox QLabel { color: #111827; font-size: 14px; background-color: transparent; }"
    "QMessageBox QPushButton {"
    "  background-color: #2563EB; color: white; border: none;"
    "  border-radius: 6px; padding: 6px 18px; font-size: 13px; font-weight: bold;"
    "  min-width: 70px;"
    "}"
    "QMessageBox QPushButton:hover { background-color: #1D4ED8; }";

// ── Misc ───────────────────────────────────────────────────────────────────
inline const QString RADIO =
    "QRadioButton { color: #374151; background: transparent; font-size: 13px; }"
    "QRadioButton::indicator { width: 16px; height: 16px; }";

inline const QString LIST =
    "QListWidget {"
    "  background-color: white; border: 1px solid #D1D5DB;"
    "  border-radius: 8px; font-size: 13px; color: #111827;"
    "}"
    "QListWidget::item { padding: 8px; }"
    "QListWidget::item:selected { background-color: #DBEAFE; color: #111827; }";

inline const QString SCROLL_TRANSPARENT =
    "QScrollArea { background: transparent; border: none; }";

// ── Sidebar (MainWindow) ───────────────────────────────────────────────────
inline const QString SIDEBAR_BTN =
    "QPushButton {"
    "  background-color: transparent; color: #E5E7EB;"
    "  text-align: left; padding: 12px 16px; border: none;"
    "  border-radius: 12px; font-size: 15px; font-weight: 600;"
    "}"
    "QPushButton:hover { background-color: #1F2937; color: white; }";

inline const QString SIDEBAR_BTN_ACTIVE =
    "QPushButton {"
    "  background-color: #2563EB; color: white;"
    "  text-align: left; padding: 12px 16px; border: none;"
    "  border-radius: 12px; font-size: 15px; font-weight: 700;"
    "}";

// ── Dark Dialog Theme (LoginDialog / RegisterDialog 공통) ─────────────────
inline const QString DARK_DIALOG_BG =
    "QDialog { background-color: #0f172a; "
    "  font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif; }";

// 카드 공통 배경/테두리 (objectName 선택자는 각 파일에서 앞에 붙여 사용)
inline const QString DARK_CARD_BODY =
    "background-color: #1e293b; border-radius: 18px; border: 1px solid #334155;";

inline const QString DARK_FIELD_LABEL =
    "QLabel#fieldLabel {"
    "  color: #cbd5e1; font-size: 12px; font-weight: 600;"
    "  font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif; }";

inline const QString DARK_INPUT =
    "QLineEdit#inputField {"
    "  background-color: #0f172a; border: 1.5px solid #334155;"
    "  border-radius: 8px; color: #f1f5f9; font-size: 13px; padding: 0 12px;"
    "  font-family: 'Apple SD Gothic Neo', 'Malgun Gothic', sans-serif; }"
    "QLineEdit#inputField:focus { border: 1.5px solid #6366f1; background-color: #1a2540; }";

} // namespace AppStyle
