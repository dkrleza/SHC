#include "SHCGaussGenerator.hpp"
#include "SHCDiagramWidget.hpp"
#include "SHCClusterDefWidget.hpp"
#include "SHCOutlierDefWidget.hpp"
#include "SHCLinMovingClusterDefWidget.hpp"
#include "SHCCircMovingClusterDefWidget.hpp"
#include <QtWidgets/QMessageBox>
#include <iostream>
using namespace std;

SHCGaussGeneratorWindow::SHCGaussGeneratorWindow() {
    ui.setupUi(this);
    ui.placeholder->setCurrentIndex(ui.generatorType->currentIndex());
    ui.optionsTabs->setCurrentIndex(0);
    connect(ui.generateButton,SIGNAL(clicked()),this,SLOT(generate()));
    connect(ui.addDefinitionButton,SIGNAL(clicked()),this,SLOT(addDefinitionWidget()));
}

void SHCGaussGeneratorWindow::generate() {
    if(ui.generatorType->currentIndex()==1 && ui.listWidget->count()==0) {
        QMessageBox msgBox;
        msgBox.setText("You must have at least one cluster or outlier definition in the list.");
        msgBox.exec();
        return;
    }
    if(ui.totalStepsValue->value()<2 && isHavingMovingDefinition()) {
        QMessageBox msgBox;
        msgBox.setText("Having a moving cluster definition requires multiple steps.");
        msgBox.exec();
        return;
    }
    int cindex=ui.generatorType->currentIndex();
    if(cindex==0 || cindex==1) {
        if(chartView==NULL) {
            chartView=new SHCDiagramWidget(&ui);
            connect(chartView,SIGNAL(closed()),this,SLOT(closeChart()));
        }
        chartView->show();
    }
}

void SHCGaussGeneratorWindow::closeChart() {
    if(chartView!=NULL) {
        chartView->deleteLater();
        chartView=NULL;
    }
}

void SHCGaussGeneratorWindow::addDefinitionWidget() {
    QListWidgetItem *qlwi=new QListWidgetItem(ui.listWidget);
    ui.listWidget->addItem(qlwi);
    stringstream ss;
    if(ui.definitionType->currentIndex()==0) ss << "Cluster definition " << definition++;
    else if(ui.definitionType->currentIndex()==1) ss << "Outlier definition " << definition++;
    else if(ui.definitionType->currentIndex()==2) ss << "Linear mov. cluster definition " << definition++;
    else if(ui.definitionType->currentIndex()==3) ss << "Circular mov. cluster definition " << definition++;
    string *name=new string(ss.str());
    if(ui.definitionType->currentIndex()==0) {
        SHCClusterDefWidget *cdwi=new SHCClusterDefWidget(name);
        connect(cdwi,SIGNAL(removeListItem(string*)),this,SLOT(removeDefinitionWidget(string*)));
        qlwi->setSizeHint(cdwi->geometry().size());
        ui.listWidget->setItemWidget(qlwi, cdwi);
    } else if(ui.definitionType->currentIndex()==1) {
        SHCOutlierDefWidget *odwi=new SHCOutlierDefWidget(name);
        connect(odwi,SIGNAL(removeListItem(string*)),this,SLOT(removeDefinitionWidget(string*)));
        qlwi->setSizeHint(odwi->geometry().size());
        ui.listWidget->setItemWidget(qlwi, odwi);
    } else if(ui.definitionType->currentIndex()==2) {
        SHCLinMovingClusterDefWidget *lmcdwi=new SHCLinMovingClusterDefWidget(name);
        connect(lmcdwi,SIGNAL(removeListItem(string*)),this,SLOT(removeDefinitionWidget(string*)));
        qlwi->setSizeHint(lmcdwi->geometry().size());
        ui.listWidget->setItemWidget(qlwi, lmcdwi);
    } else if(ui.definitionType->currentIndex()==3) {
        SHCCircMovingClusterDefWidget *cmcdwi=new SHCCircMovingClusterDefWidget(name);
        connect(cmcdwi,SIGNAL(removeListItem(string*)),this,SLOT(removeDefinitionWidget(string*)));
        qlwi->setSizeHint(cmcdwi->geometry().size());
        ui.listWidget->setItemWidget(qlwi, cmcdwi);
    }
}

void SHCGaussGeneratorWindow::removeDefinitionWidget(string *name) {
    for(int i=0;i<ui.listWidget->count();i++) {
        QListWidgetItem *qlwi=ui.listWidget->item(i);
        QWidget *wi=ui.listWidget->itemWidget(qlwi);
        bool remove=false;
        if(typeid(*wi)==typeid(SHCClusterDefWidget)) {
            SHCClusterDefWidget *cdwi=dynamic_cast<SHCClusterDefWidget *>(wi);
            if(*cdwi->getName()==*name) remove=true;
        } else if(typeid(*wi)==typeid(SHCOutlierDefWidget)) {
            SHCOutlierDefWidget *odwi=dynamic_cast<SHCOutlierDefWidget *>(wi);
            if(*odwi->getName()==*name) remove=true;
        } else if(typeid(*wi)==typeid(SHCLinMovingClusterDefWidget)) {
            SHCLinMovingClusterDefWidget *lmcdwi=dynamic_cast<SHCLinMovingClusterDefWidget *>(wi);
            if(*lmcdwi->getName()==*name) remove=true;
        } else if(typeid(*wi)==typeid(SHCCircMovingClusterDefWidget)) {
            SHCCircMovingClusterDefWidget *cmcdwi=dynamic_cast<SHCCircMovingClusterDefWidget *>(wi);
            if(*cmcdwi->getName()==*name) remove=true;
        }
        if(remove) {
            ui.listWidget->removeItemWidget(qlwi);
            ui.listWidget->takeItem(i);
            ui.listWidget->update();
            return;
        }
    }
}

bool SHCGaussGeneratorWindow::isHavingMovingDefinition() {
    for(int i=0;i<ui.listWidget->count();i++) {
        QListWidgetItem *qlwi=ui.listWidget->item(i);
        QWidget *wi=ui.listWidget->itemWidget(qlwi);
        if(typeid(*wi)==typeid(SHCLinMovingClusterDefWidget) ||
           typeid(*wi)==typeid(SHCCircMovingClusterDefWidget)) return true;
    }
    return false;
}
