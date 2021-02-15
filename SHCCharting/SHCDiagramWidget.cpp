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

void SHCDiagramWidget::clusterMapping(unordered_map<string, QColor> *cluster_color_mapping, SHC *shc_classifier) {
    set<string> *clusts=shc_classifier->getTopContainers();
    for(string clus_id:*clusts)
        if(cluster_color_mapping->find(clus_id)==cluster_color_mapping->end()) {
            QColor clus_color=QColor::fromRgb(QRandomGenerator::global()->generate());
            clus_color.setAlpha(200);
            (*cluster_color_mapping)[clus_id]=clus_color;
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

void SHCDiagramWidget::createChart(unordered_map<string, QColor> *cluster_color_mapping, Ui::gaussTest *params_widget,
                                   SHC *shc_classifier, MatrixXd *slice, const string info, bool drawSI, const int current_slice_pos) {
    SHCGaussChartView *gdw=new SHCGaussChartView(shc_classifier, slice, params_widget, cluster_color_mapping, QString::fromStdString(info),
                                                 drawSI, current_slice_pos);
    ui.diagramHolder->addWidget(gdw);
}

void SHCDiagramWidget::createChart(unordered_map<string, QColor> *cluster_color_mapping, Ui::gaussTest *params_widget,
                                   vector<QColor> *series_color_mapping, SHC *shc_classifier, vector<MatrixXd *> *slice,
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
    int parallel_instances=params_widget->parallelInstances->value();
    vector<SHC*> shc_classifiers(parallel_instances);
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
        for(int p=0;p<parallel_instances;p++) {
            SHC *shcc=new SHC(2,at,dt,params_widget->decayPace->value(),false,parallel_instances>1);
            shcc->setSharedAgglomerationThreshold(params_widget->sha_agglo_theta->value());
            shc_classifiers[p]=shcc;
        }
    }
    if(params_widget->shc_specific->isChecked()) {
        for(int p=0;p<parallel_instances;p++) {
            SHC *shcc=new SHC(params_widget->mdTheta->value(), parallel_instances>1, true, new VectorXd(params_widget->virtualCovariance->value()*VectorXd::Ones(2)),
                              params_widget->aggloCounter->value(),10,40,(float)params_widget->removeCompSizeRatio->value(),
                              (float)params_widget->driftCheckCompSizeRatio->value(),(float)params_widget->driftMovementMDRatio->value(),
                              params_widget->decayPace_manual->value());
            shcc->setSharedAgglomerationThreshold(params_widget->sha_agglo_theta_man->value());
            shc_classifiers[p]=shcc;
        }
    }
    if(parallel_instances>1) {
        for(int p=1;p<parallel_instances;p++) {
            shc_classifiers[p]->setDeltaLoggingSourceName("node"+to_string(p));
        }
    }
    if(params_widget->sigmaIndexUsage->isChecked()) {
        for(SHC *shcc:shc_classifiers)
            shcc->useSigmaIndex(params_widget->sigmaIndexNeighborhood->value(),params_widget->balancedSigmaIndex->isChecked());
    }
    vector<vector<MatrixXd*>*> all_slices(parallel_instances);
    for(int p=0;p<parallel_instances;p++) all_slices[p]=new vector<MatrixXd*>();
    vector<unordered_map<string, QColor>*> cluster_color_mapping(parallel_instances);
    for(int p=0;p<parallel_instances;p++) cluster_color_mapping[p]=new unordered_map<string, QColor>();
    vector<vector<QColor>*> series_color_mapping(parallel_instances);
    for(int p=0;p<parallel_instances;p++) series_color_mapping[p]=new vector<QColor>();
    int tmp_totalPages=totalPages,current_processing_slice_pos=-1;
    totalPages*=parallel_instances;
    for(int i=0;i<tmp_totalPages;i++) {
        vector<shared_ptr<DeltaLogger>> dl(parallel_instances);
        for(int p=0;p<parallel_instances;p++) {
            GDS_Generated *gg=gds->generate((int)(params_widget->totalElementsValue->value()/totalPages),i);
            // Pull out all the instance values
            SHC *shcc=shc_classifiers[p];
            vector<MatrixXd*> *as=all_slices[p];
            unordered_map<string, QColor> *ccm=cluster_color_mapping[p];
            vector<QColor> *scm=series_color_mapping[p];
            
            if(scm->size()==0) {
                if(gg->slice->size()==1)
                    scm->push_back(QColorConstants::Black);
                else {
                    for(unsigned j=0;j<gg->slice->size();j++)
                    scm->push_back(QColor::fromRgb(QRandomGenerator::global()->generate()));
                }
            }
            
            shcc->setEventCallback([&](SHCEvent *event) {
                if(event->eventType==DriftFrontTrigger || event->eventType==BeforeObsoleteComponentDeleted ||
                   event->eventType==AfterObsoleteComponentDeleted) {
                    stringstream ss;
                    if(params_widget->displayFrontTriggerEvent->isChecked() && event->eventType==DriftFrontTrigger) {
                        ss << "Triggered by the component " << *event->component_id << " - drift front threshold reached";
                        MatrixXd *tslice=event->shc->getCache();
                        unordered_map<string, QColor> *tcluster_color_mapping=new unordered_map<string, QColor>();
                        clusterMapping(tcluster_color_mapping, event->shc);
                        createChart(tcluster_color_mapping, params_widget, event->shc, tslice, ss.str(),
                                    params_widget->displaySI->isChecked(), -1);
                        totalPages++;
                        delete tslice;delete tcluster_color_mapping;
                    } else if((params_widget->displayBeforeDeleteObsoleteEvent->isChecked() && event->eventType==BeforeObsoleteComponentDeleted) ||
                              (params_widget->displayAfterDeleteObsoleteEvent->isChecked() && event->eventType==AfterObsoleteComponentDeleted)) {
                        clusterMapping(ccm, shcc);
                        if(event->eventType==BeforeObsoleteComponentDeleted) ss << "Component " << *event->component_id << " is about to be deleted as obsolete";
                        if(event->eventType==AfterObsoleteComponentDeleted) ss << "Component " << *event->component_id << " was deleted as obsolete";
                        createChart(ccm, params_widget, shcc, gg->combined, ss.str(), params_widget->displaySI->isChecked(), current_processing_slice_pos);
                        totalPages++;
                    }
                }
                if(event->eventType==SlicePositionChange) current_processing_slice_pos=*event->slice_pos;
                delete event;
            });
            pair<shared_ptr<vector<shared_ptr<ClassificationResult>>>,shared_ptr<DeltaLogger>> res=shcc->process(gg->combined);
            dl[p]=res.second;
            
            clusterMapping(ccm, shcc);
            mergeSlices(as, gg->slice);
            stringstream ss;
            ss << "Normal view for page " << (i+1) << " and SHC " << shcc->getDeltaLoggingSourceName();
            createChart(ccm, params_widget, scm, shcc, gg->slice, ss.str(), params_widget->displaySI->isChecked());
            shcc->pseudoOffline(true); // let us see
            delete gg;
        }
        if(parallel_instances>1) {
            SHC *master=shc_classifiers[0];
            shared_ptr<DeltaLogger> master_dl=dl[0];
            master_dl->print(cout,"master");
            for(int p1=1;p1<parallel_instances;p1++) {
                string node_id=shc_classifiers[p1]->getDeltaLoggingSourceName();
                dl[p1]->print(cout,node_id);
                master->consumeDeltaLog(dl[p1],&node_id,master_dl,true,&cout);
                master_dl->print(cout,"master");
            }
            for(int p1=1;p1<parallel_instances;p1++) {
                shc_classifiers[p1]->consumeDeltaLog(master_dl,NULL,nullptr,true,&cout);
            }
            vector<MatrixXd*> *as=all_slices[0];
            unordered_map<string, QColor> *ccm=cluster_color_mapping[0];
            clusterMapping(ccm, master);
            vector<QColor> *scm=series_color_mapping[0];
            stringstream ss;
            ss << "After master merge";
            totalPages++;
            createChart(ccm, params_widget, scm, master, as, ss.str(), params_widget->displaySI->isChecked());
            for(int p1=1;p1<parallel_instances;p1++) {
                vector<MatrixXd*> *as=all_slices[p1];
                unordered_map<string, QColor> *ccm=cluster_color_mapping[p1];
                clusterMapping(ccm, shc_classifiers[p1]);
                vector<QColor> *scm=series_color_mapping[p1];
                stringstream ss;
                ss << "After " << shc_classifiers[p1]->getDeltaLoggingSourceName() << " merge";
                totalPages++;
                createChart(ccm, params_widget, scm, shc_classifiers[p1], as, ss.str(), params_widget->displaySI->isChecked());
            }
        }
    }
    delete gds;
    if(totalPages>1) {
        for(int p=0;p<parallel_instances;p++) {
            totalPages++;
            
            SHC *shcc=shc_classifiers[p];
            vector<MatrixXd*> *as=all_slices[p];
            unordered_map<string, QColor> *ccm=cluster_color_mapping[p];
            vector<QColor> *scm=series_color_mapping[p];
            
            stringstream ss;
            ss << "Full view for SHC instance " << (p+1);
            SHCGaussChartView *gdw=new SHCGaussChartView(shcc, as, params_widget, ccm, scm, QString::fromStdString(ss.str()), params_widget->displaySI->isChecked());
            ui.diagramHolder->addWidget(gdw);
        }
    }
    ui.diagramHolder->setCurrentIndex(page-1);
    adjustButtons();
    printCurrentPage();
    for(int p=0;p<parallel_instances;p++) {
        for(unsigned i=0;i<all_slices[p]->size();i++)
            delete all_slices[p]->at(i);
        delete all_slices[p];
        delete cluster_color_mapping[p];
        delete series_color_mapping[p];
        delete shc_classifiers[p];
    }
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
