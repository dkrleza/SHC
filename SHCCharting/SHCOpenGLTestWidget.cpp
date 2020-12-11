#include "SHCOpenGLTestWidget.hpp"
QT_CHARTS_USE_NAMESPACE
using namespace Eigen;
using namespace std;

OpenGLTestWidget::OpenGLTestWidget(QChart *ch,Ui::openGLparams *params):QChartView(ch) {
    this->params=params;
    setFixedSize(800, 800);
    setRenderHint(QPainter::Antialiasing);
    setWindowModality(Qt::ApplicationModal);
    chart=ch;
    connect(chart->scene(),&QGraphicsScene::changed,this,&OpenGLTestWidget::handle_scene_changed);
    axisX = new QValueAxis;
    axisX->setRange(0, 100);
    axisY = new QValueAxis;
    axisY->setRange(0, 100);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->setTitle("Dynamic generator results");
    chart->legend()->hide();
    series=new QScatterSeries();
    series->setUseOpenGL(true);
    series->setColor(QColor("red"));
    series->setMarkerSize(6);
    series->setPen(QPen("black"));
    chart->addSeries(series);
    series->attachAxis(axisX);
    series->attachAxis(axisY);
    timer=new QTimer();
    timer->setInterval(0);
    timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, this, &OpenGLTestWidget::regenerate);
    AgglomerationType at=NormalAgglomeration;
    switch (params->aggloTemplate->currentIndex()) {
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
    switch (params->driftTemplate->currentIndex()) {
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
    shc_classifier=new SHC(2,at,dt,params->decayPace->value());
    if(params->sigmaIndexUsage->isChecked()) shc_classifier->useSigmaIndex(params->sigmaIndexNeighborhood->value());
}

void OpenGLTestWidget::setSeries(QtCharts::QScatterSeries *series) {
    this->series=series;
}

QList<QPointF> *OpenGLTestWidget::generateStep() {
    double radi1=(2*M_PI*angle)/360,radi2=(2*M_PI*(angle+180))/360;
    angle+=params->angleInc->value();
    if(angle>360) angle-=360;

    vector<GDS_Element*> *elements=new vector<GDS_Element*>();
    VectorXd *mean1=new VectorXd(2);
    double x=(double)30*cos(radi1)+50,y=(double)30*sin(radi1)+50;
    *mean1 << x, y;
    MatrixXd *covariance1=new MatrixXd(2,2);
    *covariance1 << params->variance->value(), 0.0, 0.0, params->variance->value();
    VectorXd *vv=new VectorXd(2);
    *vv << DEFAULT_VIRTUAL_VARIANCE, DEFAULT_VIRTUAL_VARIANCE;
    elements->push_back(new GDS_Element(mean1,covariance1,GDS_Cluster,vv,(long)params->elementsPerStep->value()));
    
    VectorXd *mean2=new VectorXd(2);
    x=(double)30*cos(radi2)+50;
    y=(double)30*sin(radi2)+50;
    *mean2 << x, y;
    MatrixXd *covariance2=new MatrixXd(2,2);
    *covariance2 << params->variance->value(), 0.0, 0.0, params->variance->value();
    vv=new VectorXd(2);
    *vv << DEFAULT_VIRTUAL_VARIANCE, DEFAULT_VIRTUAL_VARIANCE;
    elements->push_back(new GDS_Element(mean2,covariance2,GDS_Cluster,vv,(long)params->elementsPerStep->value()));
    
    GaussDataSource gds(elements);
    GDS_Generated *values=gds.generate((long)params->elementsPerStep->value());
    shc_classifier->process(values->combined);
    shc_classifier->pseudoOffline(true);
    clusterMapping();
    
    set<string> *clusters=shc_classifier->getTopContainers();
    for(string clus_id:*clusters) {
        vector<SHC_Component_Details *> *cds=shc_classifier->getClassificationDetails(&clus_id, 3.5, 0, 1, true);
        for(SHC_Component_Details *comp_det:*cds) {
            QList<QPointF> *ql=new QList<QPointF>();
            if(pm_display.find(clus_id)!=pm_display.end()) delete pm_display[clus_id]->pointList;
            else pm_display[clus_id]=new OGLLineSeries();
            pm_display[clus_id]->pointList=ql;
            for(int row_index=0;row_index<comp_det->single->rows();row_index++) {
                VectorXd row=comp_det->single->row(row_index);
                ql->append(QPointF(row(0),row(1)));
            }
            if(params->displayBaseline->isChecked() && comp_det->baselineDetails!=NULL) {
                QList<QPointF> *ql_bline=new QList<QPointF>();
                if(pm_display[clus_id]->baselinePointList!=NULL) delete pm_display[clus_id]->baselinePointList;
                pm_display[clus_id]->baselinePointList=ql_bline;
                for(int row_index=0;row_index<comp_det->baselineDetails->single->rows();row_index++) {
                    VectorXd row=comp_det->baselineDetails->single->row(row_index);
                    ql_bline->append(QPointF(row(0),row(1)));
                }
            }
            if(params->displayDriftingFront->isChecked() && comp_det->childDetails.size()>0) {
                OGLLineSeries *ogl_ser=pm_display[clus_id];
                ogl_ser->move();
                if(ogl_ser->dfPL==NULL) ogl_ser->dfPL=new vector<QList<QPointF>*>();
                if(ogl_ser->dfSeries==NULL) ogl_ser->dfSeries=new vector<QLineSeries*>();
                for(SHC_Component_Details *df_compdet:comp_det->childDetails) {
                    QList<QPointF> *ql_detline=new QList<QPointF>();
                    ogl_ser->dfPL->push_back(ql_detline);
                    for(int row_index=0;row_index<df_compdet->single->rows();row_index++) {
                        VectorXd row=df_compdet->single->row(row_index);
                        ql_detline->append(QPointF(row(0),row(1)));
                    }
                }
            }
            delete comp_det;
        }
        delete cds;
    }
    pm_clear.clear();
    for(auto pm_display_it:pm_display) {
        if(clusters->find(pm_display_it.first)==clusters->end())
            pm_clear[pm_display_it.first]=pm_display[pm_display_it.first];
    }
    for(auto pm_clear_it:pm_clear) pm_display.erase(pm_clear_it.first);
    delete clusters;
    
    QList<QPointF> *points=new QList<QPointF>();
    for(int row_index=0;row_index<values->combined->rows();row_index++) {
        VectorXd row=values->combined->row(row_index);
        points->append(QPointF(row(0),row(1)));
    }
    delete values;
    return points;
}

void OpenGLTestWidget::regenerate() {
    QList<QPointF> *points=generateStep();
    series->replace(*points);
    process_clusters();
    delete points;
}

void OpenGLTestWidget::first_generate() {
    QList<QPointF> *points=generateStep();
    series->append(*points);
    process_clusters();
    delete points;
}

void OpenGLTestWidget::process_clusters() {
    for(auto it1:pm_display) {
        if(it1.second->series==NULL) {
            QLineSeries *qls=new QLineSeries();
            qls->setUseOpenGL(true);
            QPen p(Qt::black,3);
            if(cluster_color_mapping.find(it1.first)!=cluster_color_mapping.end()) p.setColor(cluster_color_mapping[it1.first]);
            qls->setPen(p);
            qls->append(*it1.second->pointList);
            chart->addSeries(qls);
            qls->attachAxis(axisX);
            qls->attachAxis(axisY);
            pm_display[it1.first]->series=qls;
        } else {
            it1.second->series->replace(*it1.second->pointList);
        }
        if(it1.second->baselinePointList!=NULL) {
            if(it1.second->baselineSeries==NULL) {
                QLineSeries *qls=new QLineSeries();
                qls->setUseOpenGL(true);
                QPen p(Qt::black,1);
                qls->setPen(p);
                qls->append(*it1.second->baselinePointList);
                chart->addSeries(qls);
                qls->attachAxis(axisX);
                qls->attachAxis(axisY);
                pm_display[it1.first]->baselineSeries=qls;
            } else {
                it1.second->baselineSeries->replace(*it1.second->baselinePointList);
            }
        }
        if(it1.second->dfPL!=NULL)
            for(QList<QPointF> *pl:*it1.second->dfPL) {
                QLineSeries *qls=new QLineSeries();
                QPen p(Qt::red,1);
                p.setStyle(Qt::DotLine);
                qls->setPen(p);
                qls->append(*pl);
                chart->addSeries(qls);
                qls->attachAxis(axisX);
                qls->attachAxis(axisY);
                it1.second->dfSeries->push_back(qls);
            }
        if(it1.second->dfSeries_r!=NULL)
            for(QLineSeries *ser:*it1.second->dfSeries_r) chart->removeSeries(ser);
        it1.second->remove();
    }
    for(auto it_c:pm_clear) {
        if(it_c.second->series!=NULL) chart->removeSeries(it_c.second->series);
        if(it_c.second->baselineSeries!=NULL) chart->removeSeries(it_c.second->baselineSeries);
        if(it_c.second->dfSeries!=NULL)
            for(QLineSeries *ser:*it_c.second->dfSeries) chart->removeSeries(ser);
        if(it_c.second->dfSeries_r!=NULL)
            for(QLineSeries *ser:*it_c.second->dfSeries_r) chart->removeSeries(ser);
        delete it_c.second;
    }
    pm_clear.clear();
}

OpenGLTestWidget::~OpenGLTestWidget() {
    delete timer;
    delete chart;
    delete shc_classifier;
}

void OpenGLTestWidget::clusterMapping() {
    set<string> *clusts=shc_classifier->getTopContainers();
    for(string clus_id:*clusts)
        if(cluster_color_mapping.find(clus_id)==cluster_color_mapping.end()) {
            QColor clus_color=QColor::fromRgb(QRandomGenerator::global()->generate());
            clus_color.setAlpha(200);
            cluster_color_mapping[clus_id]=clus_color;
        }
    delete clusts;
}


void OpenGLTestWidget::closeEvent(QCloseEvent *event) {
    timer->stop();
    closed();
    event->accept();
}

void OpenGLTestWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
    if(!fg) {
        fg=true;
        first_generate();
    } else timer->start();
}

void OpenGLTestWidget::handle_scene_changed() {
    timer->start();
}
