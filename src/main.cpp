#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    app.setStyleSheet(
        "* {"
        "    font-family: \"Inter\", \"Cantarell\", \"Segoe UI\", sans-serif;"
        "}"
    );

    MainWindow window;
    window.show();

    return app.exec();
}
