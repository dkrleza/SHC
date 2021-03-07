#ifndef GaussDataSource_hpp
#define GaussDataSource_hpp

#include <Eigen/Dense>
#include <string>
#include "SHCDataSource.hpp"
#include "SHC/SHC_Component.hpp"
#include <memory>
using namespace Eigen;
using namespace std;

struct GaussRandomParameters {
    double minCovariance=0.5,maxCovariance=7.0;
    double minCorrelation=0.01,maxCorrelation=0.95;
    int dimensions=2;
    double minSpace=10,maxSpace=90;
    double virtualCovariance=DEFAULT_VIRTUAL_VARIANCE;
    GaussRandomParameters() {}
    GaussRandomParameters(double minCovariance,double maxCovariance,double minCorrelation,double maxCorrelation,
                          int dimensions,double minSpace,double maxSpace,double virtualCovariance)
            : minCovariance(minCovariance),maxCovariance(maxCovariance),minCorrelation(minCorrelation),maxCorrelation(maxCorrelation),
              dimensions(dimensions),minSpace(minSpace),maxSpace(maxSpace),virtualCovariance(virtualCovariance) {}
    
};

enum GDS_Type {GDS_Cluster, GDS_Outlier, GDS_LinearMovingCluster, GDS_CircularMovingCluster};

class AbstractMovementCalculator {
private:
    VectorXd *pos=NULL;
public:
    AbstractMovementCalculator(VectorXd *pos);
    virtual VectorXd *calculatePosition(int step);
    AbstractMovementCalculator()=default;
    virtual ~AbstractMovementCalculator()=default;
};
class LinearMovementCalculator : public AbstractMovementCalculator {
private:
    VectorXd *startPos=NULL,*finishPos=NULL,*delta=NULL;
public:
    LinearMovementCalculator(VectorXd *startPos,VectorXd *finishPos,int steps);
    ~LinearMovementCalculator();
    VectorXd *calculatePosition(int step);
};
class CircularMovementCalculator : public AbstractMovementCalculator {
private:
    VectorXd *center=NULL;
    int startAngle=0,radius=10;
    double deltaAngle=10.0;
public:
    CircularMovementCalculator(VectorXd *center,int startAngle,int finishAngle,int radius,int steps);
    ~CircularMovementCalculator();
    VectorXd *calculatePosition(int step);
};

struct GDS_Element {
    VectorXd *mean=NULL;
    MatrixXd *cov=NULL;
    GDS_Type type=GDS_Cluster;
    vector<SHC_Component *> *mockup=NULL;
    AbstractMovementCalculator *movCalc=NULL;
    SHC_Containable_Set *set=NULL;
    GDS_Element(VectorXd *mean,MatrixXd *cov,GDS_Type type,VectorXd *virtualVariance=NULL,long elements=1) {
        this->mean=mean;
        this->cov=cov;
        this->type=type;
        if(type==GDS_Cluster) elements=1000;
        set=new SHC_Containable_Set();
        mockup=new vector<SHC_Component *>({new SHC_Component(set,mean,cov,false,elements,virtualVariance)});
        if(virtualVariance!=NULL) delete virtualVariance;
    }
    GDS_Element(VectorXd *startPos,VectorXd *finishPos,MatrixXd *cov,int steps,VectorXd *virtualVariance=NULL) {
        this->cov=cov;
        this->type=GDS_LinearMovingCluster;
        this->movCalc=new LinearMovementCalculator(startPos,finishPos,steps);
        mockup=new vector<SHC_Component *>();
        set=new SHC_Containable_Set();
        for(int i=0;i<steps;i++) {
            VectorXd *nmean=movCalc->calculatePosition(i);
            mockup->push_back(new SHC_Component(set,nmean,cov,false,1000,virtualVariance));
            delete nmean;
        }
        delete startPos;delete finishPos;
        if(virtualVariance!=NULL) delete virtualVariance;
    }
    GDS_Element(VectorXd *center,int startAngle,int finishAngle,int radius,MatrixXd *cov,int steps,VectorXd *virtualVariance=NULL) {
        this->cov=cov;
        this->type=GDS_CircularMovingCluster;
        this->movCalc=new CircularMovementCalculator(center,startAngle,finishAngle,radius,steps);
        mockup=new vector<SHC_Component *>();
        set=new SHC_Containable_Set();
        for(int i=0;i<steps;i++) {
            VectorXd *nmean=movCalc->calculatePosition(i);
            mockup->push_back(new SHC_Component(set,nmean,cov,false,1000,virtualVariance));
            delete nmean;
        }
        delete center;
        if(virtualVariance!=NULL) delete virtualVariance;
    }
    ~GDS_Element() {
        if(mean!=NULL) delete mean;
        delete cov;
        for(SHC_Component *comp:*mockup) delete comp;
        delete mockup;
        if(movCalc!=NULL) delete movCalc;
        if(set!=NULL) delete set;
    }
};

struct GDS_Generated : public SHC_Generated {
    MatrixXd *combined=NULL;
    GDS_Generated(vector<MatrixXd*> *slice) {
        this->slice=slice;
        combined=new MatrixXd(0,2);
        for(unsigned j=0;j<slice->size();j++) {
            MatrixXd *ts2=new MatrixXd(combined->rows()+slice->at(j)->rows(),2);
            *ts2 << *combined, *(slice->at(j));
            delete combined;
            combined = ts2;
        }
        VectorXi indices = VectorXi::LinSpaced(combined->rows(), 0, combined->rows());
        mt19937 eng(chrono::high_resolution_clock::now().time_since_epoch().count());
        shuffle(indices.data(), indices.data()+combined->rows(), eng);
        *combined = indices.asPermutation()*(*combined);
    }
    ~GDS_Generated() {
        for(MatrixXd *m:*slice) delete m;
        delete slice;
        delete combined;
    }
};

class GaussDataSource : public SHCDataSource {
private:
    MatrixXd *generateCovariance(bool makeOutlier=false);
    VectorXd *generateMean(),*generateVirtualVariance();
    double random(double min, double max);
    GaussRandomParameters *params=NULL;
    vector<GDS_Element*> *elements=NULL;
    int clusters,outliers;
    bool addElement(GDS_Element *el, double mahalanobisThreshold=3.0),allowOverlapping=false;
public:
    GaussDataSource(int clusters=5, int outliers=0, double mahalanobisThreshold=3.0, bool allowOverlapping=false, GaussRandomParameters *params=NULL);
    GaussDataSource(vector<GDS_Element*> *elements);
    ~GaussDataSource();
    GDS_Generated *generate(long elements,int step=0);
};

#endif /* GaussDataSource_hpp */
