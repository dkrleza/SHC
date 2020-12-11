#include "SHCShellWindow.hpp"
#include <iostream>
using namespace std;

SHCShellWindow::SHCShellWindow() {
    shell.setupUi(this);
    connect(shell.execButton,SIGNAL(clicked()),this,SLOT(show_diagram()));
}

SHCShellWindow::~SHCShellWindow() {
    if(w2!=NULL) delete w2;
    if(w3!=NULL) delete w3;
}

void SHCShellWindow::show_diagram() {
    if(shell.testType->currentIndex()==0) {
        if(w2==NULL) w2=new SHCGaussGeneratorWindow();
        w2->show();
    } else if(shell.testType->currentIndex()==1) {
        if(w3==NULL) w3=new SHCOpenGLParams();
        w3->show();
    } else if(shell.testType->currentIndex()==2) {
        if(w4==NULL) w4=new SHCDatasetProcessing(new QChart(),2,"generated1","Generated 1");
        connect(w4, SIGNAL(closed()), this, SLOT(close_w4()));
        w4->show();
    } else if(shell.testType->currentIndex()==3) {
        if(w4==NULL) w4=new SHCDatasetProcessing(new QChart(),2,":/datasets/Aggregation.txt","Aggregation");
        connect(w4, SIGNAL(closed()), this, SLOT(close_w4()));
        w4->show();
    } else if(shell.testType->currentIndex()==4) {
        if(w4==NULL) w4=new SHCDatasetProcessing(new QChart(),2,":/datasets/Spiral.txt","Spiral");
        connect(w4, SIGNAL(closed()), this, SLOT(close_w4()));
        w4->show();
    } else if(shell.testType->currentIndex()==5) {
        if(w4==NULL) w4=new SHCDatasetProcessing(new QChart(),2,":/datasets/MopsiLocations2012-Joensuu.txt","JoensuuGPS",' ');
        connect(w4, SIGNAL(closed()), this, SLOT(close_w4()));
        w4->show();
    } else if(shell.testType->currentIndex()==6) {
        if(w4==NULL) w4=new SHCDatasetProcessing(new QChart(),2,":/datasets/chameleon.csv","Chameleon",',');
        connect(w4, SIGNAL(closed()), this, SLOT(close_w4()));
        w4->show();
    }
}

void SHCShellWindow::close_w4() {
    if(w4!=NULL) {
        w4->deleteLater();
        w4=NULL;
    }
}
