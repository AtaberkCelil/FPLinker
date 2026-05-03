#ifndef MARKETCHARD_H
#define MARKETCHARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPixmap>
#include "FlatpakManager.h"

class MarketCard : public QWidget {
    Q_OBJECT

public:
    explicit MarketCard(const MarketplaceApp& app, QWidget* parent = nullptr);
    void setMatched(bool matched);

signals:
    void installRequested(const QString& appId);
    void removeRequested(const QString& appId);

public slots:
    void setInstalled(bool installed);

private:
    void setupUI();
    void loadIcon();
    void updateInstalledState();
    void onIconDownloaded(QNetworkReply* reply);

    MarketplaceApp m_app;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QLabel* m_summaryLabel;
    QLabel* m_devLabel;
    QPushButton* m_actionBtn;
    QNetworkAccessManager m_iconNetwork;
    bool m_matched = true;
    bool m_isInstalled = false;
};

#endif
