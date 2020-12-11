#ifndef SHCShellWindow_hpp
#define SHCShellWindow_hpp

#include <QtWidgets/QMainWindow>
#include "ui_shcShell.h"
#include "SHCGaussGenerator.hpp"
#include "SHCOpenGLParams.hpp"
#include "SHCDatasetProcessing.hpp"

class SHCShellWindow : public QMainWindow {
Q_OBJECT
private:
    Ui::shcShell shell;
    SHCGaussGeneratorWindow *w2=NULL;
    SHCOpenGLParams *w3=NULL;
    SHCDatasetProcessing *w4=NULL;
public:
    SHCShellWindow();
    ~SHCShellWindow();
public slots:
    void show_diagram();
    void close_w4();
};

#endif /* SHCShellWindow_hpp */
