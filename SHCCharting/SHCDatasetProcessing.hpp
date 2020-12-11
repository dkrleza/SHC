#ifndef SHCDatasetProcessing_hpp
#define SHCDatasetProcessing_hpp

#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCore/QRandomGenerator>
#include "SHC.hpp"
#include <Eigen/Dense>
#include "SHCChartingUtils.hpp"
#include "SHCComponentAreaSeries.hpp"
#include "SHCComponentTooltip.hpp"
QT_CHARTS_USE_NAMESPACE
using namespace std;

class SHCDatasetProcessing : public QChartView {
Q_OBJECT
private:
    SHCComponentTooltip *tt=NULL;
    SHC *shc_classifier=NULL;
    unordered_map<string, QBrush> cluster_color_mapping;
    void clusterMapping();
    void closeEvent(QCloseEvent *event);
    SHCComponentAreaSeries *transform(SHC_Component *comp, MatrixXd *upper, MatrixXd *lower);
signals:
    void closed();
protected:
    QChart *chart=NULL;
    QScatterSeries *selected=NULL;
    QValueAxis *axisX=NULL, *axisY=NULL;
public:
    SHCDatasetProcessing(QChart *chart,int dimensions,string datasetfile,string datasetname,char separator='\t');
    ~SHCDatasetProcessing();
public slots:
    void showComponentTooltip(SHC_Component *comp);
    void showPoint(const QPointF &p);
};

/*class SHCHFADataset : public QChartView {
    Q_OBJECT
    private:
        SHC *shc_classifier=NULL;
        void closeEvent(QCloseEvent *event);
        SHCComponentAreaSeries *transform(SHC_Component *comp, MatrixXd *upper, MatrixXd *lower);
    signals:
        void closed();
    protected:
        QChart *chart=NULL;
    public:
        SHCHFADataset(QChart *chart,string hfa_x,string hfa_y);
        ~SHCHFADataset();
};*/

#endif /* SHCDatasetProcessing_hpp */
