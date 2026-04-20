#include "logindialog.h"
#include "registerdialog.h"
#include "mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QGraphicsDropShadowEffect>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent), m_mainWindow(nullptr)
{
    setupUI();
    applyStyles();
    setWindowTitle(tr("로그인"));
    setFixedSize(460, 520);
}

LoginDialog::~LoginDialog() = default;

void LoginDialog::setupUI()
{
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setAlignment(Qt::AlignCenter);
    rootLayout->setContentsMargins(40, 40, 40, 40);

    m_loginCard = new QFrame(this);
    m_loginCard->setObjectName("loginCard");
    m_loginCard->setFixedWidth(360);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_loginCard);
    shadow->setBlurRadius(32);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0, 0, 0, 70));
    m_loginCard->setGraphicsEffect(shadow);

    QVBoxLayout *cardLayout = new QVBoxLayout(m_loginCard);
    cardLayout->setSpacing(12);
    cardLayout->setContentsMargins(32, 36, 32, 32);

    m_titleLabel = new QLabel(tr("환영합니다"), m_loginCard);
    m_titleLabel->setObjectName("titleLabel");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_subtitleLabel = new QLabel(tr("계정에 로그인하세요"), m_loginCard);
    m_subtitleLabel->setObjectName("subtitleLabel");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);

    QFrame *divider = new QFrame(m_loginCard);
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("divider");

    m_idLabel = new QLabel(tr("아이디"), m_loginCard);
    m_idLabel->setObjectName("fieldLabel");

    m_idEdit = new QLineEdit(m_loginCard);
    m_idEdit->setObjectName("inputField");
    m_idEdit->setPlaceholderText(tr("아이디를 입력하세요"));
    m_idEdit->setFixedHeight(44);

    m_pwLabel = new QLabel(tr("비밀번호"), m_loginCard);
    m_pwLabel->setObjectName("fieldLabel");

    m_pwEdit = new QLineEdit(m_loginCard);
    m_pwEdit->setObjectName("inputField");
    m_pwEdit->setPlaceholderText(tr("비밀번호를 입력하세요"));
    m_pwEdit->setEchoMode(QLineEdit::Password);
    m_pwEdit->setFixedHeight(44);

    m_registerBtn = new QPushButton(tr("회원가입"), m_loginCard);
    m_registerBtn->setObjectName("registerBtn");
    m_registerBtn->setFixedHeight(44);

    m_confirmBtn = new QPushButton(tr("O  확인"), m_loginCard);
    m_confirmBtn->setObjectName("confirmBtn");
    m_confirmBtn->setFixedHeight(48);

    m_cancelBtn = new QPushButton(tr("X  취소"), m_loginCard);
    m_cancelBtn->setObjectName("cancelBtn");
    m_cancelBtn->setFixedHeight(48);

    QHBoxLayout *actionRow = new QHBoxLayout();
    actionRow->setSpacing(10);
    actionRow->addWidget(m_confirmBtn);
    actionRow->addWidget(m_cancelBtn);

    cardLayout->addWidget(m_titleLabel);
    cardLayout->addWidget(m_subtitleLabel);
    cardLayout->addSpacing(2);
    cardLayout->addWidget(divider);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_idLabel);
    cardLayout->addWidget(m_idEdit);
    cardLayout->addWidget(m_pwLabel);
    cardLayout->addWidget(m_pwEdit);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_registerBtn);
    cardLayout->addSpacing(4);
    cardLayout->addLayout(actionRow);

    rootLayout->addWidget(m_loginCard);

    connect(m_registerBtn, SIGNAL(clicked()), this, SLOT(onRegisterClicked()));
    connect(m_confirmBtn, SIGNAL(clicked()), this, SLOT(onConfirmClicked()));
    connect(m_cancelBtn, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
}

void LoginDialog::applyStyles()
{
    setStyleSheet(R"(
        QDialog {
            background-color: #0f172a;
            font-family: 'Malgun Gothic', 'Apple SD Gothic Neo', sans-serif;
        }
    )");

    m_loginCard->setStyleSheet(R"(
        #loginCard {
            background-color: #1e293b;
            border-radius: 18px;
            border: 1px solid #334155;
        }
        #titleLabel {
            color: #f8fafc;
            font-size: 24px;
            font-weight: 700;
        }
        #subtitleLabel {
            color: #94a3b8;
            font-size: 13px;
        }
        QFrame#divider {
            color: #334155;
        }
        QLabel#fieldLabel {
            color: #cbd5e1;
            font-size: 12px;
            font-weight: 600;
        }
        QLineEdit#inputField {
            background-color: #0f172a;
            border: 1.5px solid #334155;
            border-radius: 9px;
            color: #f1f5f9;
            font-size: 13px;
            padding: 0 12px;
        }
    )");
}

void LoginDialog::onRegisterClicked()
{
    RegisterDialog dlg(this);
    dlg.exec();
}

void LoginDialog::onConfirmClicked()
{
    QString id = m_idEdit->text().trimmed();
    QString pw = m_pwEdit->text().trimmed();

    if (id.isEmpty() || pw.isEmpty()) {
        QMessageBox::warning(this, tr("입력 오류"),
                             tr("아이디와 비밀번호를 입력해주세요."));
        return;
    }

    // 지금은 임시로 입력만 하면 로그인 성공 처리
    m_mainWindow = new MainWindow();
    m_mainWindow->show();

    accept();   // 로그인창 닫기
}

void LoginDialog::onCancelClicked()
{
    reject();
}