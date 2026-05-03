#ifndef FLATPAKMANAGER_H
#define FLATPAKMANAGER_H

#include <QString>
#include <QList>
#include <QProcess>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QPixmap>

struct FlatpakApp {
    QString appId;
    QString name;
    QString description;
    QString version;
    QString origin;
    QString branch;
    QString iconName;
    QString desktopFile;
    bool isInstalled = true;
};

struct MarketplaceApp {
    QString appId;
    QString name;
    QString summary;
    QString description;
    QString developerName;
    QString iconUrl;
    QString license;
    int installsLastMonth = 0;
    bool isInstalled = false;
};

class FlatpakManager : public QObject {
    Q_OBJECT

public:
    static QList<FlatpakApp> listInstalledApps();
    static bool launchApp(const QString& appId);
    static QString findIconPath(const QString& iconName);
    static bool installApp(const QString& appId);
    static bool removeApp(const QString& appId);

    FlatpakManager(QObject* parent = nullptr);
    void searchMarketplace(const QString& query);

signals:
    void marketplaceResults(const QList<MarketplaceApp>& apps);
    void marketplaceError(const QString& error);

private:
    static QString findDesktopFile(const QString& appId);
    static QString getIconName(const QString& appId, const QString& desktopFile);

    QNetworkAccessManager* m_network;
    QString m_currentSearch;
    void onSearchReply(QNetworkReply* reply);
    bool isAppInstalled(const QString& appId);
};

#endif
