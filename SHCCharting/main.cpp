#include <QtWidgets/QApplication>
#include "SHCShellWindow.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SHCShellWindow window;
    window.show();
    return app.exec();
}
