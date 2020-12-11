#include "SHCDiagramWidget.hpp"
#include <QtWidgets/QWidget>
#include <QtCore/QString>
#include <QtCore/QDataStream>
#include "ui_diagram.h"
#include "SHC.hpp"
#include "SHCGaussDiagram.hpp"
#include <iostream>
#include <QtCore/QRandomGenerator>
#include "SHCOutlierDefWidget.hpp"
#include "SHCClusterDefWidget.hpp"
#include "SHCLinMovingClusterDefWidget.hpp"
#include "SHCCircMovingClusterDefWidget.hpp"
#include "GaussDataSource.hpp"
#include <memory>
#include <string>
using namespace std;
using namespace Qt;

void SHCDiagramWidget::clusterMapping(unordered_map<string, QColor> &cluster_color_mapping, SHC *shc_classifier) {
    set<string> *clusts=shc_classifier->getTopContainers();
    for(string clus_id:*clusts)
        if(cluster_color_mapping.find(clus_id)==cluster_color_mapping.end()) {
            QColor clus_color=QColor::fromRgb(QRandomGenerator::global()->generate());
            clus_color.setAlpha(200);
            cluster_color_mapping[clus_id]=clus_color;
        }
    delete clusts;
}

void SHCDiagramWidget::mergeSlices(vector<MatrixXd *> *all_slices, vector<MatrixXd *> *slice) {
    if(all_slices->size()==0) {
        for(unsigned j=0;j<slice->size();j++)
            all_slices->push_back(new MatrixXd(*slice->at(j)));
    } else {
        for(unsigned j=0;j<slice->size();j++) {
            MatrixXd *nm=new MatrixXd(all_slices->at(j)->rows()+slice->at(j)->rows(), all_slices->at(j)->cols());
            *nm << *(all_slices->at(j)), *(slice->at(j));
            delete all_slices->at(j);
            all_slices->at(j) = nm;
        }
    }
}

void SHCDiagramWidget::createChart(const unordered_map<string, QColor> &cluster_color_mapping, Ui::gaussTest *params_widget,
                                   SHC *shc_classifier, MatrixXd *slice, const string info, bool drawSI, const int current_slice_pos) {
    SHCGaussChartView *gdw=new SHCGaussChartView(shc_classifier, slice, params_widget, cluster_color_mapping, QString::fromStdString(info),
                                                 drawSI, current_slice_pos);
    ui.diagramHolder->addWidget(gdw);
}

void SHCDiagramWidget::createChart(const unordered_map<string, QColor> &cluster_color_mapping, Ui::gaussTest *params_widget,
                                   const vector<QColor> &series_color_mapping, SHC *shc_classifier, vector<MatrixXd *> *slice,
                                   const string info, bool drawSI) {
    SHCGaussChartView *gdw=new SHCGaussChartView(shc_classifier, slice, params_widget, cluster_color_mapping, series_color_mapping,
                                                 QString::fromStdString(info), drawSI);
    ui.diagramHolder->addWidget(gdw);
}

SHCDiagramWidget::SHCDiagramWidget(Ui::gaussTest *params_widget) {
    ui.setupUi(this);
    setWindowModality(ApplicationModal);
    GaussDataSource *gds=NULL;
    totalPages=params_widget->totalStepsValue->value();
    if(params_widget->generatorType->currentIndex()==0) {
        GaussRandomParameters *params=new GaussRandomParameters(params_widget->minCovariance->value(), params_widget->maxCovariance->value(),
            params_widget->minCorrelation->value(), params_widget->maxCorrelation->value(), 2,(double)params_widget->minSpace->value(),
            (double)params_widget->maxSpace->value(), params_widget->virtualCovariance->value());
        gds=new GaussDataSource(params_widget->clustersValue->value(), params_widget->outliersValue->value(),
                                params_widget->mdOverlapTheta->value(), params_widget->allowOverlappingValue->isChecked(), params);
    } else if(params_widget->generatorType->currentIndex()==1) {
        vector<GDS_Element*> *elements=new vector<GDS_Element*>();
        for(int i=0;i<params_widget->listWidget->count();i++) {
            QListWidgetItem *qlwi=params_widget->listWidget->item(i);
            QWidget *wi=params_widget->listWidget->itemWidget(qlwi);
            if(typeid(*wi)==typeid(SHCClusterDefWidget)) {
                SHCClusterDefWidget *cdwi=dynamic_cast<SHCClusterDefWidget*>(wi);
                VectorXd *mean=new VectorXd(2);
                *mean << cdwi->getUi()->meanX->value(), cdwi->getUi()->meanY->value();
                MatrixXd *covariance=new MatrixXd(2,2);
                *covariance << cdwi->getUi()->covX->value(), cdwi->getUi()->corrXY->value(), cdwi->getUi()->corrXY->value(), cdwi->getUi()->covY->value();
                VectorXd *vv=new VectorXd(2);
                *vv << params_widget->virtualCovariance->value(), params_widget->virtualCovariance->value();
                elements->push_back(new GDS_Element(mean,covariance,GDS_Cluster,vv,(long)100));
            } else if(typeid(*wi)==typeid(SHCOutlierDefWidget)) {
                SHCOutlierDefWidget *odwi=dynamic_cast<SHCOutlierDefWidget*>(wi);
                VectorXd *mean=new VectorXd(2);
                *mean << odwi->getUi()->meanX->value(), odwi->getUi()->meanY->value();
                MatrixXd *covariance=new MatrixXd(2,2);
                covariance->setZero();
                VectorXd *vv=new VectorXd(2);
                *vv << params_widget->virtualCovariance->value(), params_widget->virtualCovariance->value();
                elements->push_back(new GDS_Element(mean,covariance,GDS_Outlier,vv,(long)1));
            } else if(typeid(*wi)==typeid(SHCLinMovingClusterDefWidget)) {
                SHCLinMovingClusterDefWidget *lmcdwi=dynamic_cast<SHCLinMovingClusterDefWidget*>(wi);
                VectorXd *startPos=new VectorXd(2);
                *startPos << lmcdwi->getUi()->startX->value(), lmcdwi->getUi()->startY->value();
                VectorXd *finishPos=new VectorXd(2);
                *finishPos << lmcdwi->getUi()->finishX->value(), lmcdwi->getUi()->finishY->value();
                MatrixXd *covariance=new MatrixXd(2,2);
                *covariance << lmcdwi->getUi()->covX->value(), lmcdwi->getUi()->corrXY->value(), lmcdwi->getUi()->corrXY->value(), lmcdwi->getUi()->covY->value();
                VectorXd *vv=new VectorXd(2);
                *vv << params_widget->virtualCovariance->value(), params_widget->virtualCovariance->value();
                elements->push_back(new GDS_Element(startPos,finishPos,covariance,totalPages,vv));
            } else if(typeid(*wi)==typeid(SHCCircMovingClusterDefWidget)) {
                SHCCircMovingClusterDefWidget *cmcdwi=dynamic_cast<SHCCircMovingClusterDefWidget*>(wi);
                VectorXd *center=new VectorXd(2);
                *center << cmcdwi->getUi()->centerX->value(), cmcdwi->getUi()->centerY->value();
                int startAngle=cmcdwi->getUi()->fromAng->value();
                int finishAngle=cmcdwi->getUi()->toAng->value();
                int radius=cmcdwi->getUi()->radius->value();
                MatrixXd *covariance=new MatrixXd(2,2);
                *covariance << cmcdwi->getUi()->covX->value(), cmcdwi->getUi()->corrXY->value(), cmcdwi->getUi()->corrXY->value(), cmcdwi->getUi()->covY->value();
                VectorXd *vv=new VectorXd(2);
                *vv << params_widget->virtualCovariance->value(), params_widget->virtualCovariance->value();
                elements->push_back(new GDS_Element(center,startAngle,finishAngle,radius,covariance,totalPages,vv));
            }
        }
        gds=new GaussDataSource(elements);
    }
    connect(ui.nextButton, SIGNAL(clicked()), this, SLOT(nextPage()));
    connect(ui.previousButton, SIGNAL(clicked()), this, SLOT(previousPage()));
    connect(ui.showOriginalSeries, SIGNAL(toggled(bool)), this, SLOT(toggleOriginalSeries(bool)));
    connect(ui.showSHCModels, SIGNAL(toggled(bool)), this, SLOT(toggleSHCModels(bool)));
    connect(ui.showSI, SIGNAL(toggled(bool)), this, SLOT(toggleSI(bool)));
    for(int i=ui.diagramHolder->count()-1;i>=0;i--) {
        QWidget* widget = ui.diagramHolder->widget(i);
        ui.diagramHolder->removeWidget(widget);
        widget->deleteLater();
    }
    SHC *shc_classifier=NULL;
    if(params_widget->shc_behavioral->isChecked()) {
        AgglomerationType at=NormalAgglomeration;
        switch (params_widget->aggloTemplate->currentIndex()) {
            case 1:
                at=AggresiveAgglomeration;
                break;
            case 2:
                at=RelaxedAgglomeration;
                break;
            default:
                break;
        }
        DriftType dt=NormalDrift;
        switch (params_widget->driftTemplate->currentIndex()) {
            case 1:
                dt=FastDrift;
                break;
            case 2:
                dt=SlowDrift;
                break;
            case 3:
                dt=NoDrift;
                break;
            case 4:
                dt=UltraFastDrift;
                break;
            default:
                break;
        }
        shc_classifier=new SHC(2,at,dt,params_widget->decayPace->value());
        shc_classifier->setSharedAgglomerationThreshold(params_widget->sha_agglo_theta->value());
    }
    if(params_widget->shc_specific->isChecked()) {
        shc_classifier=new SHC(params_widget->mdTheta->value(), true, true, new VectorXd(params_widget->virtualCovariance->value()*VectorXd::Ones(2)),
                               params_widget->aggloCounter->value(),10,40,(float)params_widget->removeCompSizeRatio->value(),
                               (float)params_widget->driftCheckCompSizeRatio->value(),(float)params_widget->driftMovementMDRatio->value(),
                               params_widget->decayPace_manual->value());
        shc_classifier->setSharedAgglomerationThreshold(params_widget->sha_agglo_theta_man->value());
    }
    if(params_widget->sigmaIndexUsage->isChecked()) shc_classifier->useSigmaIndex(params_widget->sigmaIndexNeighborhood->value(),params_widget->balancedSigmaIndex->isChecked());
    vector<MatrixXd*> *all_slices=new vector<MatrixXd*>();
    unordered_map<string, QColor> cluster_color_mapping;
    vector<QColor> series_color_mapping;
    int tmp_totalPages=totalPages,current_processing_slice_pos=-1;
    for(int i=0;i<tmp_totalPages;i++) {
        //vector<MatrixXd*> *slice=gds->generate((int)(params_widget->totalElementsValue->value()/totalPages),i);
        GDS_Generated *gg=gds->generate((int)(params_widget->totalElementsValue->value()/totalPages),i);
        if(series_color_mapping.size()==0) {
            if(gg->slice->size()==1)
                series_color_mapping.push_back(QColorConstants::Black);
            else {
                for(unsigned j=0;j<gg->slice->size();j++)
                    series_color_mapping.push_back(QColor::fromRgb(QRandomGenerator::global()->generate()));
            }
        }
        
        shc_classifier->setEventCallback([&](SHCEvent *event) {
            if(event->eventType==DriftFrontTrigger || event->eventType==BeforeObsoleteComponentDeleted ||
               event->eventType==AfterObsoleteComponentDeleted) {
                stringstream ss;
                if(params_widget->displayFrontTriggerEvent->isChecked() && event->eventType==DriftFrontTrigger) {
                    ss << "Triggered by the component " << *event->component_id << " - drift front threshold reached";
                    MatrixXd *tslice=event->shc->getCache();
                    unordered_map<string, QColor> tcluster_color_mapping;
                    clusterMapping(tcluster_color_mapping, event->shc);
                    createChart(tcluster_color_mapping, params_widget, event->shc, tslice, ss.str(),
                                params_widget->displaySI->isChecked(), -1);
                    totalPages++;
                    delete tslice;
                } else if((params_widget->displayBeforeDeleteObsoleteEvent->isChecked() && event->eventType==BeforeObsoleteComponentDeleted) ||
                          (params_widget->displayAfterDeleteObsoleteEvent->isChecked() && event->eventType==AfterObsoleteComponentDeleted)) {
                    clusterMapping(cluster_color_mapping, shc_classifier);
                    if(event->eventType==BeforeObsoleteComponentDeleted) ss << "Component " << *event->component_id << " is about to be deleted as obsolete";
                    if(event->eventType==AfterObsoleteComponentDeleted) ss << "Component " << *event->component_id << " was deleted as obsolete";
                    createChart(cluster_color_mapping, params_widget, shc_classifier, gg->combined, ss.str(),
                                params_widget->displaySI->isChecked(), current_processing_slice_pos);
                    totalPages++;
                }
            }
            if(event->eventType==SlicePositionChange) current_processing_slice_pos=*event->slice_pos;
            delete event;
        });
        shc_classifier->process(gg->combined);
        //cout << "Query time:" << shc_classifier->getTimes()[0] << " ms" << endl;
        //shc_classifier->printSigmaIndex();
        
        clusterMapping(cluster_color_mapping, shc_classifier);
        mergeSlices(all_slices, gg->slice);
        stringstream ss;
        ss << "Normal view for page " << (i+1);
        createChart(cluster_color_mapping, params_widget, series_color_mapping, shc_classifier, gg->slice, ss.str(),
                    params_widget->displaySI->isChecked());
        shc_classifier->pseudoOffline(true); // let us see
        delete gg;
    }
    delete gds;
    if(totalPages>1) {
        totalPages++;
        SHCGaussChartView *gdw=new SHCGaussChartView(shc_classifier, all_slices, params_widget, cluster_color_mapping, series_color_mapping,
                                                     QString::fromStdString("Full view"), params_widget->displaySI->isChecked());
        ui.diagramHolder->addWidget(gdw);
    }
    ui.diagramHolder->setCurrentIndex(page-1);
    adjustButtons();
    printCurrentPage();
    for(unsigned i=0;i<all_slices->size();i++)
        delete all_slices->at(i);
    delete all_slices;
    delete shc_classifier;
}

SHCDiagramWidget::~SHCDiagramWidget() {
}

void SHCDiagramWidget::closeEvent(QCloseEvent *event) {
    closed();
    event->accept();
}

void SHCDiagramWidget::nextPage() {
    if(page<totalPages) {
        page++;
        ui.diagramHolder->setCurrentIndex(page-1);
    }
    adjustButtons();
    printCurrentPage();
}

void SHCDiagramWidget::previousPage() {
    if(page>1) {
        page--;
        ui.diagramHolder->setCurrentIndex(page-1);
    }
    adjustButtons();
    printCurrentPage();
}

void SHCDiagramWidget::adjustButtons() {
    if(page>1) ui.previousButton->setDisabled(false);
    else ui.previousButton->setDisabled(true);
    if(page<totalPages) ui.nextButton->setDisabled(false);
    else ui.nextButton->setDisabled(true);
    
}

void SHCDiagramWidget::printCurrentPage() {
    stringstream ss;
    ss << "Chart " << page << "/" << totalPages;
    ui.currChartText->setText(QString::fromStdString(ss.str()));
    SHCGaussChartView *gcw=dynamic_cast<SHCGaussChartView*>(ui.diagramHolder->currentWidget());
    ui.chartInfo->setPlainText(gcw->getChartInfo());
}

void SHCDiagramWidget::toggleSHCModels(bool toggled) {
    for(int i=0;i<ui.diagramHolder->count();i++) {
        SHCGaussChartView *cv=(SHCGaussChartView*)ui.diagramHolder->widget(i);
        cv->getChart()->setSHCModelsVisibility(toggled);
        cv->getChart()->update();
    }
}

void SHCDiagramWidget::toggleOriginalSeries(bool toggled) {
    for(int i=0;i<ui.diagramHolder->count();i++) {
        QWidget *wi=ui.diagramHolder->widget(i);
        if(wi!=NULL) {
            SHCGaussChartView *cv=static_cast<SHCGaussChartView*>(wi);
            cv->getChart()->setOriginalSeriesVisibility(toggled);
            cv->getChart()->update();
        }
    }
}

void SHCDiagramWidget::toggleSI(bool toggled) {
    for(int i=0;i<ui.diagramHolder->count();i++) {
        QWidget *wi=ui.diagramHolder->widget(i);
        if(wi!=NULL) {
            SHCGaussChartView *cv=static_cast<SHCGaussChartView*>(wi);
            cv->getChart()->setSIVisibility(toggled);
            cv->getChart()->update();
        }
    }
}
