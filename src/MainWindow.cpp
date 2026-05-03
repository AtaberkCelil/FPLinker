#include "MainWindow.h"
#include "FlatpakManager.h"
#include "AppCard.h"
#include "MarketCard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QScrollBar>
#include <QMessageBox>
#include <QProgressDialog>
#include <QTimer>

MainWindow::MainWindow() {
    setupWindow();
    setupUI();

    m_manager = new FlatpakManager(this);

    connect(m_manager, &FlatpakManager::marketplaceResults, this, [this](const QList<MarketplaceApp>& apps) {
        clearLayout(m_marketLayout);
        m_marketCards.clear();

        if (apps.isEmpty()) {
            showMarketplacePlaceholder("No applications found on Flathub");
            m_marketStatus->setText("No results found");
            return;
        }

        for (const MarketplaceApp& app : apps) {
            auto* card = new MarketCard(app);

            connect(card, &MarketCard::installRequested, this, [this, card](const QString& appId) {
                auto* dlg = new QProgressDialog("Installing " + appId + "...", "Cancel", 0, 0, this);
                dlg->setWindowModality(Qt::WindowModal);
                dlg->setMinimumDuration(0);
                dlg->show();

                bool success = FlatpakManager::installApp(appId);
                dlg->close();

                if (success) {
                    QMessageBox::information(this, "Success", appId + " installed successfully!");
                    card->setInstalled(true);
                    loadInstalledApps();
                } else {
                    QMessageBox::critical(this, "Error", "Failed to install " + appId);
                }
            });

            connect(card, &MarketCard::removeRequested, this, [this, card](const QString& appId) {
                auto* dlg = new QProgressDialog("Removing " + appId + "...", "Cancel", 0, 0, this);
                dlg->setWindowModality(Qt::WindowModal);
                dlg->setMinimumDuration(0);
                dlg->show();

                bool success = FlatpakManager::removeApp(appId);
                dlg->close();

                if (success) {
                    QMessageBox::information(this, "Success", appId + " removed successfully!");
                    card->setInstalled(false);
                    loadInstalledApps();
                } else {
                    QMessageBox::critical(this, "Error", "Failed to remove " + appId);
                }
            });

            m_marketCards.append(card);
            m_marketLayout->insertWidget(m_marketLayout->count(), card);
        }

        m_marketLayout->addStretch();
        m_marketStatus->setText(QString("%1 applications found").arg(apps.size()));
    });

    connect(m_manager, &FlatpakManager::marketplaceError, this, [this](const QString& error) {
        clearLayout(m_marketLayout);
        showMarketplacePlaceholder("Error: " + error);
        m_marketStatus->setText("Search failed");
    });

    loadInstalledApps();
}

void MainWindow::setupWindow() {
    setWindowTitle("FPLinker");
    setMinimumSize(900, 700);
    resize(1000, 750);
    setStyleSheet("QMainWindow { background-color: #121212; }");
}

void MainWindow::setupUI() {
    auto* central = new QWidget();
    setCentralWidget(central);
    auto* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    auto* tabs = new QTabWidget();
    tabs->setStyleSheet(
        "QTabWidget::pane {"
        "    border: none;"
        "    background-color: #121212;"
        "}"
        "QTabBar::tab {"
        "    background-color: #1a1a1a;"
        "    color: #888888;"
        "    padding: 12px 24px;"
        "    border: none;"
        "    border-bottom: 2px solid #2a2a2a;"
        "    font-size: 13px;"
        "    font-weight: bold;"
        "}"
        "QTabBar::tab:selected {"
        "    color: #ffffff;"
        "    border-bottom: 2px solid #4a9eff;"
        "}"
        "QTabBar::tab:hover {"
        "    color: #ffffff;"
        "    background-color: #222222;"
        "}"
    );

    auto* installedTab = new QWidget();
    auto* installedLayout = new QVBoxLayout(installedTab);
    installedLayout->setContentsMargins(0, 0, 0, 0);
    installedLayout->setSpacing(0);

    installedLayout->addWidget(createHeader("Installed Apps"));

    m_installedScroll = new QScrollArea();
    m_installedScroll->setWidgetResizable(true);
    m_installedScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_installedScroll->setStyleSheet(
        "QScrollArea {"
        "    background-color: #121212;"
        "    border: none;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #1a1a1a;"
        "    width: 10px;"
        "    border: none;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #3a3a3a;"
        "    border-radius: 5px;"
        "    min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #4a4a4a;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
    );

    m_installedContainer = new QWidget();
    m_installedLayout = new QVBoxLayout(m_installedContainer);
    m_installedLayout->setContentsMargins(16, 8, 16, 16);
    m_installedLayout->setSpacing(6);
    m_installedLayout->setAlignment(Qt::AlignTop);

    m_installedScroll->setWidget(m_installedContainer);
    installedLayout->addWidget(m_installedScroll);

    m_installedStatus = new QLabel();
    m_installedStatus->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 11px;"
        "    padding: 8px 16px;"
        "    background-color: #1a1a1a;"
        "    border-top: 1px solid #2a2a2a;"
        "}"
    );
    installedLayout->addWidget(m_installedStatus);

    tabs->addTab(installedTab, "Installed");

    auto* marketTab = new QWidget();
    auto* marketTabLayout = new QVBoxLayout(marketTab);
    marketTabLayout->setContentsMargins(0, 0, 0, 0);
    marketTabLayout->setSpacing(0);

    marketTabLayout->addWidget(createHeader("Marketplace"));

    m_marketScroll = new QScrollArea();
    m_marketScroll->setWidgetResizable(true);
    m_marketScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_marketScroll->setStyleSheet(
        "QScrollArea {"
        "    background-color: #121212;"
        "    border: none;"
        "}"
        "QScrollBar:vertical {"
        "    background-color: #1a1a1a;"
        "    width: 10px;"
        "    border: none;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background-color: #3a3a3a;"
        "    border-radius: 5px;"
        "    min-height: 30px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "    background-color: #4a4a4a;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    height: 0px;"
        "}"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
        "    background: none;"
        "}"
    );

    m_marketContainer = new QWidget();
    m_marketLayout = new QVBoxLayout(m_marketContainer);
    m_marketLayout->setContentsMargins(16, 8, 16, 16);
    m_marketLayout->setSpacing(6);
    m_marketLayout->setAlignment(Qt::AlignTop);

    showMarketplacePlaceholder("Search Flathub to browse and install apps");

    m_marketScroll->setWidget(m_marketContainer);
    marketTabLayout->addWidget(m_marketScroll);

    m_marketStatus = new QLabel("Search Flathub to browse apps");
    m_marketStatus->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 11px;"
        "    padding: 8px 16px;"
        "    background-color: #1a1a1a;"
        "    border-top: 1px solid #2a2a2a;"
        "}"
    );
    marketTabLayout->addWidget(m_marketStatus);

    tabs->addTab(marketTab, "Marketplace");

    mainLayout->addWidget(tabs);
}

QWidget* MainWindow::createHeader(const QString& title) {
    auto* header = new QFrame();
    header->setStyleSheet(
        "QFrame {"
        "    background-color: #1a1a1a;"
        "    border-bottom: 1px solid #2a2a2a;"
        "}"
    );
    header->setFixedHeight(80);

    auto* layout = new QHBoxLayout(header);
    layout->setContentsMargins(24, 12, 24, 12);

    auto* titleLabel = new QLabel(title);
    titleLabel->setStyleSheet(
        "QLabel {"
        "    color: #ffffff;"
        "    font-size: 22px;"
        "    font-weight: bold;"
        "}"
    );
    layout->addWidget(titleLabel, 1);

    QLineEdit* searchBox = new QLineEdit();

    if (title == "Installed Apps") {
        searchBox->setPlaceholderText("  Search installed apps...");
        m_installedSearch = searchBox;
        connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::filterInstalledApps);
    } else {
        searchBox->setPlaceholderText("  Search Flathub...");
        m_marketSearch = searchBox;

        auto* searchTimer = new QTimer(this);
        searchTimer->setSingleShot(true);
        searchTimer->setInterval(500);

        connect(searchBox, &QLineEdit::textChanged, this, [searchTimer]() {
            searchTimer->start();
        });

        connect(searchTimer, &QTimer::timeout, this, [this, searchBox]() {
            QString text = searchBox->text().trimmed();
            if (text.length() >= 2) {
                loadMarketplace(text);
            } else if (text.isEmpty()) {
                clearLayout(m_marketLayout);
                showMarketplacePlaceholder("Search Flathub to browse and install apps");
                m_marketStatus->setText("Search Flathub to browse apps");
            }
        });
    }

    searchBox->setFixedWidth(320);
    searchBox->setStyleSheet(
        "QLineEdit {"
        "    background-color: #2a2a2a;"
        "    color: #ffffff;"
        "    border: 1px solid #3a3a3a;"
        "    border-radius: 8px;"
        "    padding: 8px 12px;"
        "    font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "    border: 1px solid #4a9eff;"
        "}"
    );

    layout->addWidget(searchBox, 0, Qt::AlignRight);

    return header;
}

void MainWindow::clearLayout(QLayout* layout) {
    while (QLayoutItem* item = layout->takeAt(0)) {
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }
}

void MainWindow::showMarketplacePlaceholder(const QString& text) {
    auto* placeholder = new QLabel(text);
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 14px;"
        "    padding: 40px;"
        "}"
    );
    m_marketLayout->addWidget(placeholder);
    m_marketLayout->addStretch();
}

void MainWindow::loadInstalledApps() {
    m_installedStatus->setText("Loading Flatpak applications...");

    clearLayout(m_installedLayout);
    m_installedCards.clear();

    QList<FlatpakApp> apps = FlatpakManager::listInstalledApps();

    if (apps.isEmpty()) {
        auto* empty = new QLabel("No Flatpak applications found.\n\nInstall some apps from the Marketplace tab or via terminal.");
        empty->setAlignment(Qt::AlignCenter);
        empty->setStyleSheet(
            "QLabel {"
            "    color: #666666;"
            "    font-size: 14px;"
            "    padding: 40px;"
            "}"
        );
        m_installedLayout->addWidget(empty);
        m_installedLayout->addStretch();
        m_installedStatus->setText("No applications found");
        return;
    }

    std::sort(apps.begin(), apps.end(), [](const FlatpakApp& a, const FlatpakApp& b) {
        return a.name.toLower() < b.name.toLower();
    });

    for (const FlatpakApp& app : apps) {
        auto* card = new AppCard(app);
        connect(card, &AppCard::launchRequested, this, [](const QString& appId) {
            FlatpakManager::launchApp(appId);
        });
        connect(card, &AppCard::removeRequested, this, [this](const QString& appId) {
            int ret = QMessageBox::question(this, "Remove App",
                "Are you sure you want to remove " + appId + "?");
            if (ret == QMessageBox::Yes) {
                auto* dlg = new QProgressDialog("Removing " + appId + "...", "Cancel", 0, 0, this);
                dlg->setWindowModality(Qt::WindowModal);
                dlg->setMinimumDuration(0);
                dlg->show();

                bool success = FlatpakManager::removeApp(appId);
                dlg->close();

                if (success) {
                    loadInstalledApps();
                } else {
                    QMessageBox::critical(this, "Error", "Failed to remove " + appId);
                }
            }
        });
        m_installedCards.append(card);
        m_installedLayout->addWidget(card);
    }

    m_installedLayout->addStretch();
    m_installedStatus->setText(QString("%1 applications loaded").arg(apps.size()));
}

void MainWindow::filterInstalledApps(const QString& query) {
    QString q = query.toLower().trimmed();
    int visibleCount = 0;

    for (AppCard* card : m_installedCards) {
        bool match = q.isEmpty() ||
                     card->property("appId").toString().toLower().contains(q) ||
                     card->property("appName").toString().toLower().contains(q) ||
                     card->property("appDesc").toString().toLower().contains(q);

        card->setMatched(match);
        if (match) visibleCount++;
    }

    m_installedStatus->setText(QString("%1 of %2 applications shown").arg(visibleCount).arg(m_installedCards.size()));
}

void MainWindow::loadMarketplace(const QString& query) {
    clearLayout(m_marketLayout);

    auto* loading = new QLabel("Searching Flathub...");
    loading->setAlignment(Qt::AlignCenter);
    loading->setStyleSheet(
        "QLabel {"
        "    color: #666666;"
        "    font-size: 14px;"
        "    padding: 40px;"
        "}"
    );
    m_marketLayout->addWidget(loading);
    m_marketLayout->addStretch();

    m_marketStatus->setText("Searching Flathub...");

    m_manager->searchMarketplace(query);
}
