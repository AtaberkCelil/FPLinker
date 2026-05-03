#include "FlatpakManager.h"

#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QPixmap>

QList<FlatpakApp> FlatpakManager::listInstalledApps() {
    QList<FlatpakApp> apps;

    QProcess process;
    process.start("flatpak", {"list", "--app", "--columns=application,name,description,version,origin,branch"});
    process.waitForFinished(30000);

    if (process.exitCode() != 0) {
        return apps;
    }

    QString output = QString::fromUtf8(process.readAllStandardOutput()).trimmed();
    if (output.isEmpty()) {
        return apps;
    }

    QStringList lines = output.split('\n');
    for (const QString& line : lines) {
        if (line.trimmed().isEmpty()) continue;

        QStringList parts = line.split('\t');
        if (parts.size() < 6) continue;

        FlatpakApp app;
        app.appId = parts[0].trimmed();
        app.name = parts[1].trimmed().isEmpty() ? app.appId : parts[1].trimmed();
        app.description = parts[2].trimmed();
        app.version = parts[3].trimmed();
        app.origin = parts[4].trimmed();
        app.branch = parts[5].trimmed();
        app.desktopFile = findDesktopFile(app.appId);
        app.iconName = getIconName(app.appId, app.desktopFile);
        app.isInstalled = true;

        apps.append(app);
    }

    return apps;
}

bool FlatpakManager::launchApp(const QString& appId) {
    return QProcess::startDetached("flatpak", {"run", appId});
}

bool FlatpakManager::installApp(const QString& appId) {
    QProcess process;
    process.start("flatpak", {"install", "-y", "--noninteractive", appId});
    process.waitForFinished(-1);
    return process.exitCode() == 0;
}

bool FlatpakManager::removeApp(const QString& appId) {
    QProcess process;
    process.start("flatpak", {"uninstall", "-y", "--noninteractive", appId});
    process.waitForFinished(-1);
    return process.exitCode() == 0;
}

QString FlatpakManager::findDesktopFile(const QString& appId) {
    QString home = QDir::homePath();
    QStringList paths = {
        "/var/lib/flatpak/exports/share/applications/" + appId + ".desktop",
        home + "/.local/share/flatpak/exports/share/applications/" + appId + ".desktop"
    };

    for (const QString& path : paths) {
        if (QFile::exists(path)) {
            return path;
        }
    }
    return QString();
}

QString FlatpakManager::getIconName(const QString& appId, const QString& desktopFile) {
    if (!desktopFile.isEmpty() && QFile::exists(desktopFile)) {
        QFile file(desktopFile);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                if (line.startsWith("Icon=")) {
                    return line.mid(5).trimmed();
                }
            }
        }
    }
    return appId;
}

QString FlatpakManager::findIconPath(const QString& iconName) {
    if (iconName.isEmpty()) return QString();

    if (iconName.startsWith('/')) {
        return QFileInfo::exists(iconName) ? iconName : QString();
    }

    QString home = QDir::homePath();
    QStringList basePaths = {
        "/var/lib/flatpak/exports/share/icons",
        home + "/.local/share/flatpak/exports/share/icons",
        "/usr/share/icons"
    };

    QStringList sizes = {"256x256", "128x128", "96x96", "64x64", "48x48", "32x32"};
    QStringList dirs = {"apps", ""};

    for (const QString& base : basePaths) {
        for (const QString& size : sizes) {
            for (const QString& dir : dirs) {
                QString path = base + "/hicolor/" + size;
                if (!dir.isEmpty()) path += "/" + dir;

                QString fullPath = path + "/" + iconName + ".png";
                if (QFile::exists(fullPath)) {
                    return fullPath;
                }
            }
        }
    }

    return QString();
}

bool FlatpakManager::isAppInstalled(const QString& appId) {
    QProcess process;
    process.start("flatpak", {"list", "--app", "--columns=application"});
    process.waitForFinished(10000);
    QString output = QString::fromUtf8(process.readAllStandardOutput());
    return output.contains(appId);
}

FlatpakManager::FlatpakManager(QObject* parent) : QObject(parent) {
    m_network = new QNetworkAccessManager(this);
}

void FlatpakManager::searchMarketplace(const QString& query) {
    m_currentSearch = query;

    QUrl url("https://flathub.org/api/v2/search");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "FPLinker/1.0");

    QJsonObject body;
    body["query"] = query;
    body["filters"] = QJsonArray();

    QJsonDocument doc(body);
    QNetworkReply* reply = m_network->post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onSearchReply(reply);
    });
}

void FlatpakManager::onSearchReply(QNetworkReply* reply) {
    QList<MarketplaceApp> results;

    if (reply->error() != QNetworkReply::NoError) {
        emit marketplaceError(reply->errorString());
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        emit marketplaceError("Invalid response from Flathub");
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    if (root.contains("hits") && root["hits"].isArray()) {
        QJsonArray hits = root["hits"].toArray();
        for (const QJsonValue& val : hits) {
            if (val.isObject()) {
                QJsonObject obj = val.toObject();
                MarketplaceApp app;
                app.appId = obj.value("app_id").toString();
                app.name = obj.value("name").toString();
                app.summary = obj.value("summary").toString();
                app.description = obj.value("description").toString();
                app.developerName = obj.value("developer_name").toString();
                app.iconUrl = obj.value("icon").toString();
                app.license = obj.value("project_license").toString();
                app.installsLastMonth = obj.value("installs_last_month").toInt();
                app.isInstalled = isAppInstalled(app.appId);

                results.append(app);
            }
        }
    }

    emit marketplaceResults(results);
    reply->deleteLater();
}
