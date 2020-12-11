#include "SHCDatasetProcessing.hpp"

SHCDatasetProcessing::SHCDatasetProcessing(QChart *ch,int dimensions,string datasetfile,string datasetname,char separator):QChartView(ch) {
    setFixedSize(800, 800);
    setRenderHint(QPainter::Antialiasing);
    setWindowModality(Qt::ApplicationModal);
    pair<MatrixXd*,pair<VectorXd*,VectorXd*>> *dataset_res=NULL;
    if(datasetname=="Aggregation") dataset_res=readDataset(QString::fromStdString(datasetfile), separator, dimensions, true);
    else if(datasetname=="Chameleon") {
        VectorXd min(2);
        min << 0, 0;
        VectorXd max(2);
        max << 100, 50;
        dataset_res=readDataset(QString::fromStdString(datasetfile), separator, dimensions, false, true, &min, &max);
    } else if(datasetname!="JoensuuGPS") dataset_res=readDataset(QString::fromStdString(datasetfile), separator, dimensions, true);
    else dataset_res=readDataset(QString::fromStdString(datasetfile), separator, dimensions, false, true);
    chart=ch;
    chart->setTitle(QString::fromStdString(datasetname));
    chart->legend()->hide();
    axisX = new QValueAxis;
    axisX->setGridLineVisible(false);
    axisY = new QValueAxis;
    axisY->setGridLineVisible(false);
    if(datasetname!="JoensuuGPS" && datasetname!="Chameleon" && datasetname!="Generated 1") {
        double min=std::min((*dataset_res->second.first)(0),(*dataset_res->second.first)(1))-5;
        double max=std::max((*dataset_res->second.second)(0),(*dataset_res->second.second)(1))+5;
        axisX->setRange(min, max);
        axisY->setRange(min, max);
    } else {
        axisX->setRange((*dataset_res->second.first)(0)-0.1, (*dataset_res->second.second)(0)+0.1);
        axisY->setRange((*dataset_res->second.first)(1)-0.1, (*dataset_res->second.second)(1)+0.1);
    }
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    if(datasetname=="Aggregation") shc_classifier=new SHC(3.0, false, true, new VectorXd(0.4*VectorXd::Ones(dimensions)), 1, 0, 0, 0, 0, 0, 0, 0.1, 0);
    if(datasetname=="Chameleon") {
        shc_classifier=new SHC(2.84, false, true, new VectorXd(0.4*VectorXd::Ones(dimensions)), 500, 0, 0, 0, 0, 0, 0, 0.1, 6.5, true);
        shc_classifier->setSharedAgglomerationThreshold(1);
        setFixedSize(1600, 800);
    }
    if(datasetname=="Spiral") shc_classifier=new SHC(3.0, true, true, new VectorXd(0.3*VectorXd::Ones(dimensions)), 1, 0, 0, 0.5, 1.0, 1.0, 0);
    if(datasetname=="JoensuuGPS") shc_classifier=new SHC(3.2, true, true, new VectorXd(1.0*VectorXd::Ones(dimensions)), 10, 0, 0, 0, 0, 0, 0);
    if(datasetname=="Generated 1") {
        shc_classifier=new SHC(2,NormalAgglomeration,NoDrift,0);
        shc_classifier->useSigmaIndex(3);
        shc_classifier->setAgglomerationCount(1);
    }
    if(dataset_res==NULL) throw new SHCChartingException("Dataset cannot be read!");
    shc_classifier->process(dataset_res->first);
    shc_classifier->pseudoOffline(true);
    if(datasetname=="Generated 1") {
        shc_classifier->printSigmaIndex(cout);
        VectorXd test(2);
        test << 16,8;
        shc_classifier->process(&test);
    }
    if(datasetname=="Chameleon") {
        shc_classifier->cleanOutliers();
        shc_classifier->removeSmallComponents(7);
    }
    clusterMapping();
        
    set<string> *clusters=shc_classifier->getTopContainers();
    for(string clus_id:*clusters) {
        vector<SHC_Component_Details *> *cds=shc_classifier->getClassificationDetails(&clus_id, shc_classifier->getTheta(), 0, 1, false);
        for(SHC_Component_Details *comp_det:*cds) {
            SHCComponentAreaSeries *cas=transform(comp_det->parent, comp_det->upper, comp_det->lower);
            //cas->setColor(cluster_color_mapping[clus_id]);
            cas->setBrush(cluster_color_mapping[clus_id]);
            /*if(comp_det->parent->isBlocked()) {
                QPen qp(QColor("black"));
                qp.setWidth(3);
                cas->setPen(qp);
            } else
             cas->setPen(QPen(Qt::blue));*/
            cas->setPen(QPen(Qt::NoPen));
            connect(cas,SIGNAL(componentClicked(SHC_Component*,const QPointF&)),this,SLOT(showComponentTooltip(SHC_Component*)));
            chart->addSeries(cas);
            cas->attachAxis(axisX);
            cas->attachAxis(axisY);
            delete comp_det;
        }
        delete cds;
    }
    
    QList<QPointF> *points=new QList<QPointF>();
    for(int row_index=0;row_index<dataset_res->first->rows();row_index++) {
        VectorXd row=dataset_res->first->row(row_index);
        points->append(QPointF(row(0),row(1)));
    }
    QScatterSeries *ds_pts=new QScatterSeries();
    ds_pts->setColor(Qt::black);
    ds_pts->setMarkerSize(6);
    //ds_pts->setPen(QPen(Qt::NoPen));
    ds_pts->append(*points);
    chart->addSeries(ds_pts);
    ds_pts->attachAxis(axisX);
    ds_pts->attachAxis(axisY);
    delete points;
    
    delete clusters;delete dataset_res->second.first;
    delete dataset_res->second.second;delete dataset_res;
}

void SHCDatasetProcessing::clusterMapping() {
    set<string> *clusts=shc_classifier->getTopContainers();
    vector<Qt::BrushStyle> v={Qt::BrushStyle::SolidPattern,Qt::BrushStyle::Dense1Pattern,Qt::BrushStyle::Dense2Pattern,
        Qt::BrushStyle::Dense3Pattern,Qt::BrushStyle::Dense4Pattern,Qt::BrushStyle::Dense5Pattern};
    int i=0;
    for(string clus_id:*clusts)
        if(cluster_color_mapping.find(clus_id)==cluster_color_mapping.end()) {
            /*QColor clus_color=QColor::fromRgb(QRandomGenerator::global()->generate());
            clus_color.setAlpha(200);
            cluster_color_mapping[clus_id]=clus_color;*/
            QBrush brush(v.at(i++));
            if(i>5) i=0;
            //int gs=rand() % 200;
            //QColor cc=QColor::fromRgb(gs, gs, gs);
            QColor cc=QColor::fromRgb(QRandomGenerator::global()->generate());
            cc.setAlpha(200);
            brush.setColor(cc);
            cluster_color_mapping[clus_id]=brush;
        }
    delete clusts;
}

SHCDatasetProcessing::~SHCDatasetProcessing() {
    if(shc_classifier!=NULL) delete shc_classifier;
    if(chart!=NULL) delete chart;
}

void SHCDatasetProcessing::closeEvent(QCloseEvent *event) {
    closed();
    event->accept();
}

SHCComponentAreaSeries *SHCDatasetProcessing::transform(SHC_Component *comp, MatrixXd *upper, MatrixXd *lower) {
    QLineSeries *ser_upper=new QLineSeries();
    for(int i(0);i<upper->rows();i++)
        ser_upper->append((*upper)(i,0), (*upper)(i,1));
    QLineSeries *ser_lower=new QLineSeries();
    for(int i(0);i<lower->rows();i++)
        ser_lower->append((*lower)(i,0), (*lower)(i,1));
    SHCComponentAreaSeries *res=new SHCComponentAreaSeries(comp, ser_upper, ser_lower);
    return res;
}

void SHCDatasetProcessing::showComponentTooltip(SHC_Component *comp) {
    if(tt!=NULL) delete tt;
    tt=new SHCComponentTooltip(comp);
    tt->show();
    if(selected!=NULL) {
        chart->removeSeries(selected);
        delete selected;
        selected=NULL;
    }
    QList<QPointF> *points=new QList<QPointF>();
    string *idz=new string(comp->getId());
    MatrixXd *ptz=shc_classifier->getCache(idz);
    delete idz;
    for(int row_index=0;row_index<ptz->rows();row_index++) {
        VectorXd row=ptz->row(row_index);
        points->append(QPointF(row(0),row(1)));
    }
    selected=new QScatterSeries();
    selected->setColor(QColor("violet"));
    selected->setMarkerSize(7);
    //ds_pts->setPen(QPen(Qt::NoPen));
    connect(selected,SIGNAL(clicked(const QPointF&)),this,SLOT(showPoint(const QPointF&)));
    selected->append(*points);
    chart->addSeries(selected);
    selected->attachAxis(axisX);
    selected->attachAxis(axisY);
    delete points;delete ptz;
}

void SHCDatasetProcessing::showPoint(const QPointF &p) {
    cout << "clicked point:" << p.x() << "," << p.y() << endl;
}
