#ifndef SHCChartingUtils_hpp
#define SHCChartingUtils_hpp

#include <Eigen/Dense>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <iostream>
#include <random>
#include "SHCDataSource.hpp"
using namespace Eigen;
using namespace std;

class SHCChartingException : public exception {
private:
    const char *msg=NULL;
public:
    const char *what() const throw();
    SHCChartingException(const char *msg);
};

pair<MatrixXd*,pair<VectorXd*,VectorXd*>> *readDataset(QString filename,char separator,int fields,bool randomize=true,bool normalize=false,VectorXd *minNorm=NULL,
                                                       VectorXd *maxNorm=NULL);
MatrixXd *generateSimplePopulation(double x,double y,double variance,long elements);

#endif /* SHCChartingUtils_hpp */
