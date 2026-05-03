#include "AppCard.h"
#include "FlatpakManager.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QFileInfo>
#include <QTimer>

AppCard::AppCard(const FlatpakApp& app, QWidget* parent)
    : QWidget(parent), m_app(app) {
    setProperty("appId", m_app.appId);
    setProperty("appName", m_app.name);
    setProperty("appDesc", m_app.description);
    setupUI();
    loadIcon();
}

void AppCard::setupUI() {
    setMinimumHeight(70);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(12, 10, 12, 10);
    layout->setSpacing(12);

    m_iconLabel = new QLabel();
    m_iconLabel->setFixedSize(48, 48);
    m_iconLabel->setScaledContents(false);
    m_iconLabel->setAlignment(Qt::AlignCenter);
    m_iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    layout->addWidget(m_iconLabel);

    auto* textLayout = new QVBoxLayout();
    textLayout->setSpacing(2);

    m_nameLabel = new QLabel(m_app.name);
    m_nameLabel->setStyleSheet(
        "QLabel {"
        "    color: #ffffff;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
    );
    textLayout->addWidget(m_nameLabel);

    QString descText = m_app.description.isEmpty() ? m_app.appId : m_app.description;
    m_descLabel = new QLabel(descText);
    m_descLabel->setStyleSheet(
        "QLabel {"
        "    color: #888888;"
        "    font-size: 11px;"
        "}"
    );
    m_descLabel->setWordWrap(true);
    textLayout->addWidget(m_descLabel);

    QString metaText = m_app.origin;
    if (!m_app.version.isEmpty()) {
        metaText += " \u2022 " + m_app.version;
    }
    m_metaLabel = new QLabel(metaText);
    m_metaLabel->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 10px;"
        "}"
    );
    textLayout->addWidget(m_metaLabel);

    layout->addLayout(textLayout, 1);

    auto* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(6);

    m_removeBtn = new QPushButton("Remove");
    m_removeBtn->setFixedSize(70, 30);
    m_removeBtn->setCursor(Qt::PointingHandCursor);
    m_removeBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: transparent;"
        "    color: #ff6b6b;"
        "    border: 1px solid #555555;"
        "    border-radius: 6px;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #ff6b6b;"
        "    color: #ffffff;"
        "    border-color: #ff6b6b;"
        "}"
    );
    connect(m_removeBtn, &QPushButton::clicked, this, [this]() {
        emit removeRequested(m_app.appId);
    });
    btnLayout->addWidget(m_removeBtn);

    m_launchBtn = new QPushButton("Launch");
    m_launchBtn->setFixedSize(70, 30);
    m_launchBtn->setCursor(Qt::PointingHandCursor);
    m_launchBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #4a9eff;"
        "    color: #ffffff;"
        "    border: none;"
        "    border-radius: 6px;"
        "    font-size: 11px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #5aadff;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3a8eef;"
        "}"
    );
    connect(m_launchBtn, &QPushButton::clicked, this, [this]() {
        m_launchBtn->setText("...");
        m_launchBtn->setEnabled(false);
        emit launchRequested(m_app.appId);
        QTimer::singleShot(1000, this, [this]() {
            m_launchBtn->setText("Launch");
            m_launchBtn->setEnabled(true);
        });
    });
    btnLayout->addWidget(m_launchBtn);

    layout->addLayout(btnLayout);

    setStyleSheet(
        "AppCard {"
        "    background-color: #1e1e1e;"
        "    border-radius: 10px;"
        "    border: 1px solid #2a2a2a;"
        "}"
        "AppCard:hover {"
        "    background-color: #252525;"
        "    border: 1px solid #3a3a3a;"
        "}"
    );
}

void AppCard::loadIcon() {
    QString iconPath = FlatpakManager::findIconPath(m_app.iconName);

    if (!iconPath.isEmpty()) {
        QPixmap pixmap(iconPath);
        if (!pixmap.isNull()) {
            m_iconLabel->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            return;
        }
    }

    QIcon themeIcon = QIcon::fromTheme(m_app.iconName);
    if (!themeIcon.isNull()) {
        QPixmap pixmap = themeIcon.pixmap(48, 48);
        if (!pixmap.isNull()) {
            m_iconLabel->setPixmap(pixmap);
            return;
        }
    }

    QPixmap placeholder(48, 48);
    placeholder.fill(Qt::transparent);
    m_iconLabel->setPixmap(placeholder);
}

void AppCard::setMatched(bool matched) {
    m_matched = matched;
    setVisible(matched);
}
