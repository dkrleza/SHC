#include "GaussDataSource.hpp"
#include "SHCDataSource.hpp"
#include "SHC/SHC_Component.hpp"
#include <Eigen/Dense>
#include <random>
#include <cmath>
#include <iostream>
using namespace Eigen;

GaussDataSource::GaussDataSource(int clusters, int outliers, double mahalanobisThreshold, bool allowOverlapping, GaussRandomParameters *params) : SHCDataSource() {
    if(params==NULL) params=new GaussRandomParameters();
    this->elements=new vector<GDS_Element*>();
    this->params=params;
    this->clusters=clusters;
    this->outliers=outliers;
    this->allowOverlapping=allowOverlapping;
    for(int i=0;i<clusters;i++) {
        int count=0;
        while(!addElement(new GDS_Element(generateMean(),generateCovariance(),GDS_Cluster,generateVirtualVariance(),(long)100),mahalanobisThreshold) && count<100) count++;
    }
    for(int i=0;i<outliers;i++) {
        int count=0;
        while(!addElement(new GDS_Element(generateMean(),generateCovariance(true),GDS_Outlier,generateVirtualVariance(),(long)1),mahalanobisThreshold) && count<100) count++;
    }
}

GaussDataSource::GaussDataSource(vector<GDS_Element*> *elements) : SHCDataSource() {
    if(elements==NULL || elements->size()==0) throw SHCDataSourceException("There must be at least one GDS element defined");
    if(params==NULL) params=new GaussRandomParameters();
    this->elements=elements;
    if(elements->at(0)->type==GDS_Cluster) this->params->dimensions=elements->at(0)->mean->size();
    else if(elements->at(0)->type==GDS_LinearMovingCluster || elements->at(0)->type==GDS_CircularMovingCluster) {
        VectorXd *pos=elements->at(0)->movCalc->calculatePosition(0);
        this->params->dimensions=pos->size();
        delete pos;
    }
    this->clusters=0;
    this->outliers=0;
    for(unsigned i=0;i<this->elements->size();i++) {
        GDS_Element *element=this->elements->at(i);
        if(element->type==GDS_Cluster || element->type==GDS_LinearMovingCluster || element->type==GDS_CircularMovingCluster) this->clusters++;
        else this->outliers++;
    }
}

bool GaussDataSource::addElement(GDS_Element *el, double mahalanobisThreshold) {
    if(!allowOverlapping)
        for(auto el_v1:*elements)
            for(auto mckp1:*el_v1->mockup)
                for(auto mckp2:*el->mockup)
                    if(mckp1->connectionMeasure(mckp2, mahalanobisThreshold)<1) {
                        delete el;
                        return false;
                    }
    this->elements->push_back(el);
    return true;
}

GaussDataSource::~GaussDataSource() {
    for(GDS_Element *it:*elements) delete it;
    delete elements;
    if(params!=NULL) delete params;
}

VectorXd *GaussDataSource::generateVirtualVariance() {
    VectorXd *vv=new VectorXd(params->dimensions);
    for(int i(0);i<params->dimensions;i++) (*vv)(i)=params->virtualCovariance;
    return vv;
}

MatrixXd *GaussDataSource::generateCovariance(bool makeOutlier) {
    if(params==NULL) return NULL;
    MatrixXd *cov=new MatrixXd(MatrixXd::Zero(params->dimensions, params->dimensions));
    if(!makeOutlier) {
        for(int i=0;i<params->dimensions;i++)
            (*cov)(i,i)=random(params->minCovariance, params->maxCovariance);
        for(int i=0;i<params->dimensions;i++) {
            for(int j=0;j<params->dimensions;j++) {
                if(i!=j) {
                    double vi=sqrt((*cov)(i,i));
                    double vj=sqrt((*cov)(j,j));
                    double corr=random(params->minCorrelation, params->maxCorrelation);
                    (*cov)(i,j)=(*cov)(j,i)=corr*vi*vj;
                }
            }
        }
    }
    return cov;
}

double GaussDataSource::random(double min, double max) {
    mt19937 eng(chrono::high_resolution_clock::now().time_since_epoch().count());
    uniform_real_distribution<double> dist(min, max);
    return dist(eng);
}

VectorXd *GaussDataSource::generateMean() {
    VectorXd *mean=new VectorXd(params->dimensions);
    for(int i=0;i<params->dimensions;i++) {
        (*mean)(i)=random(params->minSpace, params->maxSpace);
    }
    return mean;
}

GDS_Generated *GaussDataSource::generate(long elements,int step) {
    long clus_elms=(long)(elements-outliers)/clusters;
    vector<MatrixXd*> *res=new vector<MatrixXd*>();
    for(unsigned i=0;i<this->elements->size();i++) {
        GDS_Element *element=this->elements->at(i);
        if(element->type==GDS_Outlier) {
            MatrixXd *res_m=new MatrixXd(MatrixXd::Zero(1, params->dimensions));
            res_m->row(0)=*element->mean;
            res->push_back(res_m);
        } else {
            MatrixXd *res_m=new MatrixXd(MatrixXd::Zero(clus_elms, params->dimensions));
            VectorXd *pos=NULL;
            EigenMultivariateNormal<double> *norm_solver=NULL;
            if(element->type==GDS_Cluster) norm_solver=new EigenMultivariateNormal<double>(*element->mean,*element->cov,false,chrono::high_resolution_clock::now().time_since_epoch().count());
            else if(element->type==GDS_LinearMovingCluster || element->type==GDS_CircularMovingCluster) {
                if(element->type==GDS_LinearMovingCluster) {
                    LinearMovementCalculator *lmc=dynamic_cast<LinearMovementCalculator*>(element->movCalc);
                    pos=lmc->calculatePosition(step);
                } else if(element->type==GDS_CircularMovingCluster) {
                    CircularMovementCalculator *cmc=dynamic_cast<CircularMovementCalculator*>(element->movCalc);
                    pos=cmc->calculatePosition(step);
                }
                norm_solver=new EigenMultivariateNormal<double>(*pos,*element->cov,false,chrono::high_resolution_clock::now().time_since_epoch().count());
            }
            *res_m << norm_solver->samples(clus_elms).transpose();
            res->push_back(res_m);
            delete norm_solver;
            if(pos!=NULL) delete pos;
        }
    }
    return new GDS_Generated(res);
}

LinearMovementCalculator::LinearMovementCalculator(VectorXd *startPos,VectorXd *finishPos,int steps) : AbstractMovementCalculator(startPos) {
    this->startPos=new VectorXd(*startPos);
    this->finishPos=new VectorXd(*finishPos);
    this->delta=new VectorXd(((*this->finishPos)-(*this->startPos))/steps);
}

LinearMovementCalculator::~LinearMovementCalculator() {
    delete startPos;
    delete finishPos;
    delete delta;
}

VectorXd *LinearMovementCalculator::calculatePosition(int step) {
    return new VectorXd(((*startPos)+(*delta)*step));
}

AbstractMovementCalculator::AbstractMovementCalculator(VectorXd *pos) {
    this->pos=pos;
}

VectorXd *AbstractMovementCalculator::calculatePosition(int step) {
    if(step>=0) return pos;
    else return NULL;
}

CircularMovementCalculator::CircularMovementCalculator(VectorXd *center,int startAngle,int finishAngle,int radius,int steps) : AbstractMovementCalculator(center) {
    this->center=new VectorXd(*center);
    this->startAngle=startAngle;
    this->deltaAngle=(finishAngle-startAngle)/steps;
    this->radius=radius;
}

CircularMovementCalculator::~CircularMovementCalculator() {
    delete center;
}

VectorXd *CircularMovementCalculator::calculatePosition(int step) {
    double angle=(double)startAngle+deltaAngle*(double)step;
    double radi=(2*M_PI*angle)/360;
    double x=(double)radius*cos(radi)+(*center)[0],y=(double)radius*sin(radi)+(*center)[1];
    VectorXd *res=new VectorXd(2);
    *res << x, y;
    return res;
}
