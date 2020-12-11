#include "SHCGaussDiagram.hpp"
#include <QtWidgets/QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include "SHCComponentAreaSeries.hpp"
#include <QtCharts/QScatterSeries>
#include <QtCore/QCoreApplication>
#include <QtCore/QRandomGenerator>
#include <cmath>
#include <iostream>
#include <SHC/SHC_Component>
#include <SHC/SHC>
#include "GaussDataSource.hpp"
#include "ui_gaussTest.h"
#include "SHCComponentTooltip.hpp"
#include "SigmaIndex.hpp"
using namespace Qt;
QT_CHARTS_USE_NAMESPACE

SHCComponentAreaSeries *SHCGaussDiagramChart::transform(SHC_Component *comp, MatrixXd *upper, MatrixXd *lower) {
    QLineSeries *ser_upper=new QLineSeries();
    for(int i(0);i<upper->rows();i++)
        ser_upper->append((*upper)(i,0), (*upper)(i,1));
    QLineSeries *ser_lower=new QLineSeries();
    for(int i(0);i<lower->rows();i++)
        ser_lower->append((*lower)(i,0), (*lower)(i,1));
    SHCComponentAreaSeries *res=new SHCComponentAreaSeries(comp, ser_upper, ser_lower);
    connect(res,SIGNAL(componentClicked(SHC_Component*,const QPointF&)),this,SLOT(showComponentTooltip(SHC_Component*)));
    return res;
}

void SHCGaussDiagramChart::m1(QScatterSeries *bcenters, QScatterSeries *centers, unordered_map<std::string, QColor> &cluster_mapping,
                              Ui::gaussTest *params_widget) {
    set<string> *clusters=processor->getTopContainers();
    if(params_widget->displayCenters->isChecked()) {
        centers = new QScatterSeries();
        centers->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        centers->setMarkerSize(8);
        centers->setColor(QColor("black"));
        bcenters = new QScatterSeries();
        bcenters->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
        bcenters->setMarkerSize(8);
        bcenters->setColor(QColor("violet"));
    }
    for(string clus_id:*clusters) {
        vector<SHC_Component_Details *> *cds=processor->getClassificationDetails(&clus_id, params_widget->mdTheta->value(), 0, 1);
        for(SHC_Component_Details *comp_det:*cds) {
            SHCComponentAreaSeries *qas=transform(comp_det->parent, comp_det->upper, comp_det->lower);
            if(!comp_det->obsolete || !params_widget->displayObsolete->isChecked()) qas->setPen(Qt::NoPen);
            else {
                qas->setPen(QColor("black"));
                qas->pen().setStyle(Qt::SolidLine);
            }
            if(params_widget->displayBaseline->isChecked() && (comp_det->covThreshold || comp_det->nThreshold)) {
                if(comp_det->covThreshold && !comp_det->nThreshold) qas->setBrush(Qt::BDiagPattern);
                else if(!comp_det->covThreshold && comp_det->nThreshold) qas->setBrush(Qt::FDiagPattern);
                else qas->setBrush(Qt::DiagCrossPattern);
            }
            qas->setColor(cluster_mapping[clus_id]);
            addSeries(qas);
            cluster_series.push_back(qas);
            if(params_widget->displayDriftingFront->isChecked()) {
                for(SHC_Component_Details *chdet:comp_det->childDetails) {
                    SHCComponentAreaSeries *qas1=transform(chdet->parent, chdet->upper, chdet->lower);
                    qas1->setColor(black);
                    addSeries(qas1);
                }
            }
            if(params_widget->displayCenters->isChecked()) {
                centers->append((*comp_det->mean)(0), (*comp_det->mean)(1));
                if(comp_det->baseline!=NULL)
                    bcenters->append((*comp_det->baseline)(0), (*comp_det->baseline)(1));
            }
            delete comp_det;
        }
        delete cds;
    }
    delete clusters;
}

void SHCGaussDiagramChart::_drawSI(SigmaIndexNode<SHC_Component*> *node, Ui::gaussTest *params_widget,set<string> *visited) {
    if(!visited) visited=new set<string>();
    if(visited->find(node->getId())!=visited->end()) return;
    visited->insert(node->getId());
    for(pair<string, SigmaIndexNode<SHC_Component*>*> it:*node->getChildren()) {
        QPointF *start=NULL;
        if(node->getId()==ROOT) start=new QPointF((float)params_widget->maxSpace->value()+8, (float)params_widget->maxSpace->value()+8);
        else start=new QPointF((*node->getPopulation()->getMean())(0),(*node->getPopulation()->getMean())(1));
        QColor *c=new QColor("black");
        drawArrow(*start, QPointF((*it.second->getPopulation()->getMean())(0),(*it.second->getPopulation()->getMean())(1)),2,*c);
        _drawSI(it.second, params_widget, visited);
        delete start;delete c;
    }
}

void SHCGaussDiagramChart::drawArrow(QPointF start,QPointF end,int size,QColor color) {
    double angle=atan2(end.y()-start.y(),end.x()-start.x())*180/M_PI;
    double angle1=angle+185,angle2=angle+175;
    double radi1=(2*M_PI*angle1)/360,radi2=(2*M_PI*angle2)/360;
    QLineSeries *qls=new QLineSeries();
    qls->append(start); qls->append(end);
    qls->append(size*cos(radi1)+end.x(),size*sin(radi1)+end.y());
    qls->append(size*cos(radi2)+end.x(),size*sin(radi2)+end.y());
    qls->append(end);
    QPen p(color);
    p.setWidth(2);
    qls->setPen(p);
    addSeries(qls);
    si_series.push_back(qls);
}

void SHCGaussDiagramChart::m2(QtCharts::QScatterSeries *bcenters, QtCharts::QScatterSeries *centers, Ui::gaussTest *params_widget,
                              bool drawSI) {
    if(params_widget->displayCenters->isChecked()) {
        addSeries(centers);
        addSeries(bcenters);
    }
    if(drawSI && processor->getSigmaIndex()) {
        SigmaIndex<SHC_Component*> *si=processor->getSigmaIndex();
        si_root=new QScatterSeries();
        si_root->append((float)params_widget->maxSpace->value()+8, (float)params_widget->maxSpace->value()+8);
        si_root->setColor(QColor("white"));
        si_root->setPen(QColor("black"));
        si_root->pen().setStyle(Qt::SolidLine);
        si_root->setMarkerSize(13);
        addSeries(si_root);
        set<string> visited;
        _drawSI(si->getRoot(), params_widget, &visited);
    }
    
    createDefaultAxes();
    axes(Qt::Horizontal).first()->setRange(params_widget->minSpace->value()-10, params_widget->maxSpace->value()+10);
    axes(Qt::Horizontal).first()->setGridLineVisible(false);
    axes(Qt::Vertical).first()->setRange(params_widget->minSpace->value()-10, params_widget->maxSpace->value()+10);
    axes(Qt::Vertical).first()->setGridLineVisible(false);
    legend()->hide();
    setTitle("Static normal generator results");
}

SHCGaussDiagramChart::SHCGaussDiagramChart(SHC *shc, vector<MatrixXd*> *slice, Ui::gaussTest *params_widget,
                                           unordered_map<string, QColor> cluster_mapping, vector<QColor> series_mapping,
                                           bool drawSI) {
    processor=make_shared<SHC>(shc);
    QtCharts::QScatterSeries *centers=NULL;
    QtCharts::QScatterSeries *bcenters=NULL;
    m1(bcenters, centers, cluster_mapping, params_widget);
    int i=0;
    for(MatrixXd *_t1:*slice) {
        QScatterSeries *ss=new QScatterSeries();
        for(int row_index=0;row_index<_t1->rows();row_index++) {
            VectorXd row=_t1->row(row_index);
            ss->append(row(0), row(1));
        }
        ss->setColor(series_mapping[i++]);
        ss->setMarkerSize(7);
        addSeries(ss);
        original_series.push_back(ss);
    }
    m2(bcenters, centers, params_widget, drawSI);
}

SHCGaussDiagramChart::SHCGaussDiagramChart(SHC *shc, MatrixXd *slice, Ui::gaussTest *params_widget,
                                           unordered_map<string, QColor> cluster_mapping, bool drawSI, const int current_slice_pos) {
    processor=make_shared<SHC>(shc);
    QtCharts::QScatterSeries *centers=NULL;
    QtCharts::QScatterSeries *bcenters=NULL;
    m1(bcenters, centers, cluster_mapping, params_widget);

    QScatterSeries *ss=new QScatterSeries();
    int last=(current_slice_pos<0 ? slice->rows() : current_slice_pos+1);
    for(int row_index=0;row_index<last;row_index++) {
        VectorXd row=slice->row(row_index);
        ss->append(row(0), row(1));
    }
    ss->setColor(QColor("black"));
    ss->setMarkerSize(7);
    addSeries(ss);
    original_series.push_back(ss);
    
    m2(bcenters, centers, params_widget, drawSI);
}

SHCGaussChartView::SHCGaussChartView(SHC *shc, vector<MatrixXd*> *slice, Ui::gaussTest *params_widget,
                                     unordered_map<string, QColor> cluster_mapping, vector<QColor> series_mapping,
                                     QString chartInfo, bool drawSI) :
                                        QChartView(new SHCGaussDiagramChart(shc, slice, params_widget, cluster_mapping, series_mapping,
                                                                            drawSI)) {
    setFixedSize(800, 800);
    setRenderHint(QPainter::Antialiasing);
    setWindowModality(ApplicationModal);
    setRubberBand(RectangleRubberBand);
    this->chartInfo=chartInfo;
}

SHCGaussChartView::SHCGaussChartView(SHC *shc, MatrixXd *slice, Ui::gaussTest *params_widget, unordered_map<string, QColor> cluster_mapping,
                                     QString chartInfo, bool drawSI, const int current_slice_pos) :
        QChartView(new SHCGaussDiagramChart(shc, slice, params_widget, cluster_mapping, drawSI, current_slice_pos)) {
    setFixedSize(800, 800);
    setRenderHint(QPainter::Antialiasing);
    setWindowModality(ApplicationModal);
    setRubberBand(RectangleRubberBand);
    this->chartInfo=chartInfo;
}

void SHCGaussChartView::setChartInfo(QString info) {
    this->chartInfo=info;
}

QString SHCGaussChartView::getChartInfo() {
    return chartInfo;
}

void SHCGaussChartView::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::RightButton) {
        chart()->zoomReset();
        return;
    }
    QChartView::mouseReleaseEvent(e);
}

void SHCGaussDiagramChart::setSHCModelsVisibility(bool visible) {
    for(SHCComponentAreaSeries *qas:cluster_series)
        if(!visible) qas->hide();
        else qas->show();
}

void SHCGaussDiagramChart::setOriginalSeriesVisibility(bool visible) {
    for(QScatterSeries *qss:original_series)
        if(!visible) qss->hide();
        else qss->show();
}

void SHCGaussDiagramChart::setSIVisibility(bool visible) {
    if(si_root) {
        if(!visible) si_root->hide();
        else si_root->show();
    }
    for(QLineSeries *qls:si_series)
        if(!visible) qls->hide();
        else qls->show();
}

SHCGaussDiagramChart *SHCGaussChartView::getChart() {
    return (SHCGaussDiagramChart *)chart();
}


void SHCGaussDiagramChart::showComponentTooltip(SHC_Component *comp) {
    if(tt!=NULL) delete tt;
    tt=new SHCComponentTooltip(comp);
    tt->show();
}


SHCGaussDiagramChart::~SHCGaussDiagramChart() {
    if(tt!=NULL) delete tt;
}
