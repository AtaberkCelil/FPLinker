#include "MarketCard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QNetworkRequest>
#include <QUrl>
#include <QTimer>

MarketCard::MarketCard(const MarketplaceApp& app, QWidget* parent)
    : QWidget(parent), m_app(app), m_isInstalled(app.isInstalled) {
    setProperty("appId", m_app.appId);
    setProperty("appName", m_app.name);
    setProperty("appSummary", m_app.summary);
    setupUI();
    updateInstalledState();
    loadIcon();
}

void MarketCard::setupUI() {
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

    QString summaryText = m_app.summary.isEmpty() ? m_app.appId : m_app.summary;
    m_summaryLabel = new QLabel(summaryText);
    m_summaryLabel->setStyleSheet(
        "QLabel {"
        "    color: #888888;"
        "    font-size: 11px;"
        "}"
    );
    m_summaryLabel->setWordWrap(true);
    textLayout->addWidget(m_summaryLabel);

    QString devText = m_app.developerName.isEmpty() ? m_app.appId : m_app.developerName;
    m_devLabel = new QLabel(devText);
    m_devLabel->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 10px;"
        "}"
    );
    textLayout->addWidget(m_devLabel);

    layout->addLayout(textLayout, 1);

    m_actionBtn = new QPushButton();
    m_actionBtn->setFixedSize(80, 30);
    m_actionBtn->setCursor(Qt::PointingHandCursor);
    updateInstalledState();
    layout->addWidget(m_actionBtn);

    setStyleSheet(
        "MarketCard {"
        "    background-color: #1e1e1e;"
        "    border-radius: 10px;"
        "    border: 1px solid #2a2a2a;"
        "}"
        "MarketCard:hover {"
        "    background-color: #252525;"
        "    border: 1px solid #3a3a3a;"
        "}"
    );
}

void MarketCard::updateInstalledState() {
    if (m_isInstalled) {
        m_actionBtn->setText("Remove");
        m_actionBtn->setStyleSheet(
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
        disconnect(m_actionBtn, &QPushButton::clicked, nullptr, nullptr);
        connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
            m_actionBtn->setText("...");
            m_actionBtn->setEnabled(false);
            emit removeRequested(m_app.appId);
            QTimer::singleShot(1000, this, [this]() {
                m_actionBtn->setEnabled(true);
            });
        });
    } else {
        m_actionBtn->setText("Install");
        m_actionBtn->setStyleSheet(
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
            "QPushButton:disabled {"
            "    background-color: #3a3a3a;"
            "    color: #666666;"
            "}"
        );
        disconnect(m_actionBtn, &QPushButton::clicked, nullptr, nullptr);
        connect(m_actionBtn, &QPushButton::clicked, this, [this]() {
            m_actionBtn->setText("...");
            m_actionBtn->setEnabled(false);
            emit installRequested(m_app.appId);
        });
    }
}

void MarketCard::loadIcon() {
    if (!m_app.iconUrl.isEmpty()) {
        QNetworkRequest request(QUrl(m_app.iconUrl));
        QNetworkReply* reply = m_iconNetwork.get(request);
        connect(reply, &QNetworkReply::finished, this, [this, reply]() {
            onIconDownloaded(reply);
        });
        return;
    }

    QPixmap placeholder(48, 48);
    placeholder.fill(Qt::transparent);
    m_iconLabel->setPixmap(placeholder);
}

void MarketCard::onIconDownloaded(QNetworkReply* reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        QPixmap pixmap;
        if (pixmap.loadFromData(data)) {
            m_iconLabel->setPixmap(pixmap.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
    reply->deleteLater();
}

void MarketCard::setMatched(bool matched) {
    m_matched = matched;
    setVisible(matched);
}

void MarketCard::setInstalled(bool installed) {
    m_isInstalled = installed;
    updateInstalledState();
}
