#include "SHCChartingUtils.hpp"

SHCChartingException::SHCChartingException(const char *msg) {
    this->msg=msg;
}
const char *SHCChartingException::what() const throw() {
    return this->msg;
}

pair<MatrixXd*,pair<VectorXd*,VectorXd*>> *readDataset(QString filename,char separator,int fields,bool randomize,bool normalize,VectorXd *minNorm,VectorXd *maxNorm) {
    if(filename=="generated1") {
        vector<MatrixXd*> slices;
        slices.push_back(generateSimplePopulation(15, 10, 0, 1));
        slices.push_back(generateSimplePopulation(9, 18, .5, 40));
        slices.push_back(generateSimplePopulation(12, 14, .5, 35));
        slices.push_back(generateSimplePopulation(19, 4, 0, 1));
//        slices.push_back(generateSimplePopulation(18, 4, 0, 1));
        
        MatrixXd *combined=new MatrixXd(0,2);
        for(unsigned j=0;j<slices.size();j++) {
            MatrixXd *ts2=new MatrixXd(combined->rows()+slices.at(j)->rows(),2);
            *ts2 << *combined, *(slices.at(j));
            delete combined;
            combined = ts2;
        }
        for(MatrixXd* it:slices) delete it;
        /*VectorXi indices = VectorXi::LinSpaced(combined->rows(), 0, combined->rows());
        mt19937 eng(chrono::high_resolution_clock::now().time_since_epoch().count());
        shuffle(indices.data(), indices.data()+combined->rows(), eng);
        *combined = indices.asPermutation()*(*combined);*/
        VectorXd *min=new VectorXd(2),*max=new VectorXd(2);
        *min << 0,0;*max << 30,30;
        return new pair<MatrixXd*,pair<VectorXd*,VectorXd*>>(combined,make_pair(min,max));
    } else {
        QFile file(filename);
        vector<VectorXd> tmp;
        VectorXd *min=NULL,*max=NULL;
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList list = line.split(separator);
                VectorXd row(fields);
                if(min==NULL) {
                    min=new VectorXd(fields);
                    max=new VectorXd(fields);
                    for(int i=0;i<fields;i++) {
                        double v=list.at(i).toDouble();
                        (*min)(i)=v;
                        (*max)(i)=v;
                    }
                }
                for(int i=0;i<fields;i++) {
                    row(i)=list.at(i).toDouble();
                    if(row(i)<(*min)(i)) (*min)(i)=row(i);
                    if(row(i)>(*max)(i)) (*max)(i)=row(i);
                }
                tmp.push_back(row);
            }
        } else return NULL;
        file.close();
        MatrixXd *r=new MatrixXd(tmp.size(),fields);
        int ri=0;
        for(VectorXd tmpv:tmp) r->row(ri++)=tmpv;
        
        if(randomize) {
            VectorXi indices = VectorXi::LinSpaced(r->rows(), 0, r->rows());
            mt19937 eng(chrono::high_resolution_clock::now().time_since_epoch().count());
            shuffle(indices.data(), indices.data()+r->rows(), eng);
            *r = indices.asPermutation()*(*r);
        }
        if(normalize) {
            for(unsigned i=0;i<r->rows();i++) {
                for(int j=0;j<fields;j++) {
                    double tot=(*max)(j)-(*min)(j);
                    double v1=(*r)(i,j)-(*min)(j);
                    double scale=100;
                    if(minNorm!=NULL && maxNorm!=NULL)
                        scale=(*maxNorm)(j)-(*minNorm)(j);
                    (*r)(i,j)=(v1/tot)*scale;
                }
            }
            for(int j=0;j<fields;j++) {
                if(minNorm!=NULL && maxNorm!=NULL) {
                    (*min)(j)=(*minNorm)(j);
                    (*max)(j)=(*maxNorm)(j);
                } else {
                    (*min)(j)=0;
                    (*max)(j)=100;
                }
            }
        }
        return new pair<MatrixXd*,pair<VectorXd*,VectorXd*>>(r,make_pair(min, max));
    }
}

MatrixXd *generateSimplePopulation(double x,double y,double variance,long elements) {
    MatrixXd *clus1_res=new MatrixXd(elements, 2);
    VectorXd pos1(2);
    pos1 << x,y;
    MatrixXd cov1(2,2);
    cov1 << variance,0,0,variance;
    EigenMultivariateNormal<double> clus1(pos1,cov1,false,chrono::high_resolution_clock::now().time_since_epoch().count());
    *clus1_res << clus1.samples(elements).transpose();
    return clus1_res;
}
