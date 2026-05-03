#ifndef APPCARD_H
#define APPCARD_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include "FlatpakManager.h"

class AppCard : public QWidget {
    Q_OBJECT

public:
    explicit AppCard(const FlatpakApp& app, QWidget* parent = nullptr);
    void setMatched(bool matched);

signals:
    void launchRequested(const QString& appId);
    void removeRequested(const QString& appId);

private:
    void setupUI();
    void loadIcon();

    FlatpakApp m_app;
    QLabel* m_iconLabel;
    QLabel* m_nameLabel;
    QLabel* m_descLabel;
    QLabel* m_metaLabel;
    QPushButton* m_launchBtn;
    QPushButton* m_removeBtn;
    bool m_matched = true;
};

#endif
