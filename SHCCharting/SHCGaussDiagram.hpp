#ifndef SHCGaussDiagram_hpp
#define SHCGaussDiagram_hpp

#include <QtCharts/QChartView>
#include "SHCComponentAreaSeries.hpp"
#include <QtCharts/QScatterSeries>
#include <Eigen/Dense>
#include "ui_gaussTest.h"
#include "SHC.hpp"
#include "SHCComponentTooltip.hpp"
#include <memory>
using namespace std;
QT_CHARTS_USE_NAMESPACE

class SHCGaussDiagramChart;

class SHCGaussChartView : public QChartView {
Q_OBJECT
private:
    void mouseReleaseEvent(QMouseEvent *e);
    QString chartInfo;
public:
    SHCGaussChartView(SHC *shc, vector<MatrixXd*> *slice, Ui::gaussTest *params_widget, unordered_map<string, QColor> cluster_mapping,
                      vector<QColor> series_mapping, QString chartInfo, bool drawSI);
    SHCGaussChartView(SHC *shc, MatrixXd *slice, Ui::gaussTest *params_widget, unordered_map<string, QColor> cluster_mapping,
                      QString chartInfo, bool drawSI, const int current_slice_pos=-1);
    SHCGaussDiagramChart *getChart();
    void setChartInfo(QString info);
    QString getChartInfo();
};

class SHCGaussDiagramChart : public QChart {
Q_OBJECT
private:
    SHCComponentAreaSeries *transform(SHC_Component *comp,MatrixXd *upper,MatrixXd *lower);
    vector<SHCComponentAreaSeries*> cluster_series;
    vector<QScatterSeries*> original_series;
    vector<QLineSeries*> si_series;
    QScatterSeries *si_root=NULL;
    SHCComponentTooltip *tt=NULL;
    shared_ptr<SHC> processor=nullptr;
    void m1(QScatterSeries *bcenters, QScatterSeries *centers,unordered_map<std::string, QColor> &cluster_mapping, Ui::gaussTest *params_widget);
    void m2(QScatterSeries *bcenters, QScatterSeries *centers, Ui::gaussTest *params_widget, bool drawSI);
    void _drawSI(SigmaIndexNode<SHC_Component*> *node, Ui::gaussTest *params_widget,set<string> *visited);
    void drawArrow(QPointF start,QPointF end,int size=2,QColor color=QColor("black"));
public:
    SHCGaussDiagramChart(SHC *shc, vector<MatrixXd*> *slice, Ui::gaussTest *params_widget, unordered_map<string, QColor> cluster_mapping,
                         vector<QColor> series_mapping, bool drawSI);
    SHCGaussDiagramChart(SHC *shc, MatrixXd *slice, Ui::gaussTest *params_widget, unordered_map<string, QColor> cluster_mapping,
                         bool drawSI, const int current_slice_pos=-1);
    ~SHCGaussDiagramChart();
    void setSHCModelsVisibility(bool visible);
    void setOriginalSeriesVisibility(bool visible);
    void setSIVisibility(bool visible);
public slots:
    void showComponentTooltip(SHC_Component *comp);
};

#endif /* SHCGaussDiagram_hpp */
