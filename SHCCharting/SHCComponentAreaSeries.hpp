#ifndef SHCComponentAreaSeries_hpp
#define SHCComponentAreaSeries_hpp

#include <QtCharts/QAreaSeries>
#include <QtCharts/QLineSeries>
#include "SHC/SHC_Component.hpp"
QT_CHARTS_USE_NAMESPACE

class SHCComponentAreaSeries : public QAreaSeries {
Q_OBJECT
private:
    SHC_Component *comp=NULL;
    QLineSeries *u=NULL,*l=NULL;
signals:
    void componentClicked(SHC_Component *comp,const QPointF &point);
public:
    SHCComponentAreaSeries(SHC_Component *comp,QLineSeries *lower,QLineSeries *upper);
    ~SHCComponentAreaSeries();
public slots:
    void seriesClicked(const QPointF &point);
};

#endif /* SHCComponentAreaSeries_hpp */
