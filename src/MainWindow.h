#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QLabel>
#include "FlatpakManager.h"
#include "AppCard.h"
#include "MarketCard.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private:
    void setupWindow();
    void setupUI();
    QWidget* createHeader(const QString& title);
    void loadInstalledApps();
    void filterInstalledApps(const QString& query);
    void loadMarketplace(const QString& query);
    void clearLayout(QLayout* layout);
    void showMarketplacePlaceholder(const QString& text);

    QLineEdit* m_installedSearch;
    QLineEdit* m_marketSearch;
    QScrollArea* m_installedScroll;
    QScrollArea* m_marketScroll;
    QWidget* m_installedContainer;
    QWidget* m_marketContainer;
    QVBoxLayout* m_installedLayout;
    QVBoxLayout* m_marketLayout;
    QLabel* m_installedStatus;
    QLabel* m_marketStatus;
    QList<AppCard*> m_installedCards;
    QList<MarketCard*> m_marketCards;

    FlatpakManager* m_manager;
};

#endif
