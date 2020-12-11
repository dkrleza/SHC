#ifndef SHCOpenGLParams_hpp
#define SHCOpenGLParams_hpp

#include <QtWidgets/QWidget>
#include "ui_openGLparams.h"
#include "SHCOpenGLTestWidget.hpp"

class SHCOpenGLParams : public QWidget {
Q_OBJECT
private:
    Ui::openGLparams ui;
    OpenGLTestWidget *oglWidget=NULL;
public:
    SHCOpenGLParams();
    ~SHCOpenGLParams();
public slots:
    void generate();
    void oglClosed();
};

#endif /* SHCOpenGLParams_hpp */
