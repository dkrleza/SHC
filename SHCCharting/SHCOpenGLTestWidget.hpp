#ifndef SHCOpenGLTestWidget_hpp
#define SHCOpenGLTestWidget_hpp

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QTimer>
#include "SHC/SHC.hpp"
#include <QtCharts/QValueAxis>
#include <Eigen/Dense>
#include "DataSources/GaussDataSource.hpp"
#include <QtGlobal>
#include <QList>
#include <QTimer>
#include <iostream>
#include <QtCore/QRandomGenerator>
#include "ui_openGLparams.h"
QT_CHARTS_USE_NAMESPACE
using namespace std;

struct OGLLineSeries {
    QList<QPointF> *pointList=NULL,*baselinePointList=NULL;
    vector<QList<QPointF>*> *dfPL=NULL;
    QLineSeries *series=NULL,*baselineSeries;
    vector<QLineSeries*> *dfSeries=NULL,*dfSeries_r=NULL;
    ~OGLLineSeries() {
        delete pointList;
        if(baselinePointList!=NULL) delete baselinePointList;
        if(series!=NULL) delete series;
        if(baselineSeries!=NULL) delete baselineSeries;
        if(dfPL!=NULL) {
            for(QList<QPointF> *it:*dfPL) delete it;
            delete dfPL;
        }
        if(dfSeries!=NULL) {
            for(QLineSeries *it:*dfSeries) delete it;
            delete dfSeries;
        }
        remove();
    }
    void remove() {
        if(dfSeries_r!=NULL) {
            for(QLineSeries *it:*dfSeries_r) delete it;
            delete dfSeries_r;
            dfSeries_r=NULL;
        }
    }
    void move() {
        remove();
        if(dfPL!=NULL) {
            for(QList<QPointF> *it:*dfPL) delete it;
            dfPL->clear();
        }
        dfSeries_r=dfSeries;
        dfSeries=NULL;
    }
};

class OpenGLTestWidget : public QChartView {
Q_OBJECT
private:
    Ui::openGLparams *params;
    SHC *shc_classifier=NULL;
    unordered_map<string, QColor> cluster_color_mapping;
    void clusterMapping();
    QValueAxis *axisX=NULL,*axisY=NULL;
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
    bool fg=false;
signals:
    void closed();
protected:
    QScatterSeries *series=NULL;
    unordered_map<string, OGLLineSeries*> pm_display,pm_clear;
    QTimer *timer=NULL;
    QChart *chart=NULL;
    double angle=0;
    QList<QPointF> *generateStep();
    void process_clusters();
public slots:
    void regenerate();
    void first_generate();
    void handle_scene_changed();
public:
    OpenGLTestWidget(QChart *chart,Ui::openGLparams *params);
    ~OpenGLTestWidget();
    void setSeries(QScatterSeries *series);
};

#endif /* SHCOpenGLTestWidget_hpp */
