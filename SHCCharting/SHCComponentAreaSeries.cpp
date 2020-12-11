#include "SHCComponentAreaSeries.hpp"
#include <iostream>

SHCComponentAreaSeries::SHCComponentAreaSeries(SHC_Component *comp,QLineSeries *lower,QLineSeries *upper):QAreaSeries(upper,lower) {
    this->comp=comp;
    this->u=upper;
    this->l=lower;
    connect(this,SIGNAL(clicked(const QPointF&)),this,SLOT(seriesClicked(const QPointF&)));
}

SHCComponentAreaSeries::~SHCComponentAreaSeries() {
    //delete comp;
    delete u;
    delete l;
}

void SHCComponentAreaSeries::seriesClicked(const QPointF &point) {
    componentClicked(comp,point);
}

