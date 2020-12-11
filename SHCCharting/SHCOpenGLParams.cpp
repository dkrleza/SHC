#include "SHCOpenGLParams.hpp"

SHCOpenGLParams::SHCOpenGLParams() {
    ui.setupUi(this);
    connect(ui.generateButton,SIGNAL(clicked()),this,SLOT(generate()));
}

void SHCOpenGLParams::generate() {
    oglWidget=new OpenGLTestWidget(new QChart(),&ui);
    connect(oglWidget,SIGNAL(closed()),this,SLOT(oglClosed()));
    oglWidget->show();
}

void SHCOpenGLParams::oglClosed() {
    oglWidget->deleteLater();
    oglWidget=NULL;
}

SHCOpenGLParams::~SHCOpenGLParams() {
    if(oglWidget!=NULL) delete oglWidget;
}

